/*
 * Parser.cpp
 *
 * Copyright (c) 2016, Peter Macko
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "stdafx.h"
#include "Parser.h"

#include <cctype>

#include "Document.h"


/**
 * Create a new parser rule
 *
 * @param _token the token
 * @param _closeCurrent true to close the current environment
 * @param _openEnvironment the new environment to open (or NULL)
 */
ParserRule::ParserRule(const char* _token, bool _closeCurrent,
                       ParserEnvironment* _openEnvironment)
{
	token = _token;
	closeCurrent = _closeCurrent;
	openEnvironment = _openEnvironment;
	mustStartLine = false;
	mustEndLine = false;
	wholeWord = false;
	referenceCount = 0;
}


/**
 * Destroy the rule
 */
ParserRule::~ParserRule()
{
}


/**
 * Determine if the rule matches the given string at the specified location
 *
 * @param line the line
 * @param pos the position (character index) in the line
 * @return true if it matches
 */
bool ParserRule::Matches(const char* line, unsigned pos)
{
	if (mustStartLine) {
		for (unsigned i = 0; i < pos; i++) {
			if (!isspace(line[i])) return false;
		}
	}
	
	size_t len = token.length();
	if (strncmp(line + pos, token.c_str(), len) != 0) {
		return false;
	}
	
	if (mustEndLine) {
		for (const char* p = line + pos + len; *p != '\0'; p++) {
			if (!isspace(*p)) return false;
		}
	}
	
	if (wholeWord) {
		if (pos > 0) {
			char c = line[pos-1];
			if (isalnum(c) || c == '_') return false;
		}
		if (pos + len < strlen(line)) {
			char c = line[pos+len];
			if (isalnum(c) || c == '_') return false;
		}
	}
	
	return true;
}


/**
 * Create a new parser environment
 *
 * @param _name the environment name
 * @param _color the default color
 */
ParserEnvironment::ParserEnvironment(const char* _name, int _color)
{
	name = _name;
	color = _color;
	
	memset(&ruleTable, 0, sizeof(ruleTable));
}


/**
 * Destroy the parser environment
 */
ParserEnvironment::~ParserEnvironment()
{
	for (int i = 0; i < 128; i++) {
		if (ruleTable[i] != NULL) {
			for (ParserRule*& rule : *ruleTable[i]) {
				if (--(rule->referenceCount) <= 0) {
					delete rule;
				}
			}
			delete ruleTable[i];
		}
	}
}


/**
 * Add a rule
 *
 * @param rule the rule (will reference-count the object and destroy it at the end if appropriate)
 */
void ParserEnvironment::AddRule(ParserRule* rule)
{
	rule->referenceCount++;
	
	char firstLetter = rule->Token()[0];
	if (firstLetter >= 127) firstLetter = 127;
	
	std::vector<ParserRule*>*& bucket = ruleTable[firstLetter];
	if (bucket == NULL) bucket = new std::vector<ParserRule*>();
	bucket->push_back(rule);
}


/**
 * Find a rule rule matches the given string at the specified location
 *
 * @param line the line
 * @param pos the position (character index) in the line
 * @return the matching rule, or NULL if none
 */
ParserRule* ParserEnvironment::FindMatchingRule(const char* line, unsigned pos)
{
	char c = line[pos];
	if (c >= 127) c = 127;
	
	std::vector<ParserRule*>*& bucket = ruleTable[c];
	if (bucket == NULL) return NULL;
	
	for (ParserRule* r : *bucket) {
		if (r->Matches(line, pos)) {
			return r;
		}
	}
	
	return NULL;
}


/**
 * Create a new parser state
 */
ParserState::ParserState()
{
}


/**
 * Destroy the parser state
 */
ParserState::~ParserState()
{
}


/**
 * Clear the state
 */
void ParserState::Clear()
{
	environmentStack.clear();
}


/**
 * Get the current environment
 *
 * @return the latest environment in the stack
 */
ParserEnvironment* ParserState::Environment() const
{
	if (environmentStack.empty()) return NULL;
	return environmentStack[environmentStack.size() - 1];
}


/**
 * Compare this parser state to another state
 *
 * @param other the other state
 * @return true if they are the equal
 */
bool ParserState::operator== (const ParserState& other) const
{
	if (other.environmentStack != environmentStack) return false;
	return true;
}


/**
 * Create a new instance of the parser
 */
Parser::Parser()
{
	globalEnvironment = NULL;
}


/**
 * Destroy the parser
 */
Parser::~Parser()
{
	for (ParserEnvironment*& env : environments) delete env;
}


/**
 * Add an environment. The first added environment is automatically set as
 * the global environment
 *
 * @param environment the environment
 */
void Parser::AddEnvironment(ParserEnvironment* environment)
{
	if (globalEnvironment == NULL) globalEnvironment = environment;
	environments.push_back(environment);
}


/**
 * Parse the next chunk
 *
 * @param lines the document commection
 * @param line the line number (0 based)
 * @param offset the offset (0 based)
 * @param newLine where to write the new line number
 * @param newOffset where to write the new offset
 */
void Parser::Parse(DocumentLineCollection& lines, unsigned line, unsigned offset,
                   unsigned& newLine, unsigned& newOffset)
{
	Parse(lines[line], line == 0 ? NULL : &lines[line-1]);
	newLine = line + 1;
	newOffset = 0;
}


/**
 * Parse the next line
 *
 * @param line the document line
 * @param previous the previous line, or NULL if not applicable
 */
void Parser::Parse(DocumentLine& line, const DocumentLine* previous)
{
	// TODO Do not always restart from the global environment
	
	ParserState initial;
	
	if (previous == NULL || previous->parserStates.empty()) {
		initial.environmentStack.push_back(globalEnvironment);
	}
	else {
		initial = previous->parserStates[previous->parserStates.size()-1].second;
	}
	
	line.parserStates.clear();
	line.initialParserState = initial;
	
	ParserState current = initial;
	
	for (unsigned i = 0; i <= line.str.length(); i++) {
	
		// Some rules might need to be applied multiple times
		
		bool done = false;
		bool applied = false;
		
		while (!done) {
			done = true;
			
			ParserRule* r = current.Environment()->FindMatchingRule(line.str.c_str(), i);
			if (r == NULL) break;


			// Apply the rule, if found
			
			applied = true;
			
			ParserEnvironment* open = r->OpensEnvironment();
			bool close = r->ClosesCurrentEnvironment();
			
			if (open != NULL) {
				current.environmentStack.push_back(open);
				line.parserStates.push_back(std::pair<unsigned, ParserState>(i, current));
			}
			
			if (close)  {
			
				i += strlen(r->Token());
				
				size_t l = current.environmentStack.size();
				if (l > 0) {
					current.environmentStack.pop_back();
				}
				if (l <= 1) {
					current.environmentStack.push_back(globalEnvironment);
				}
				
				line.parserStates.push_back(std::pair<unsigned, ParserState>(i, current));
			}
			
			
			// Look for more rules for zero-length tokens
			
			if (strlen(r->Token()) == 0 && (open != NULL || close)) {
				done = false;
			}
			
			
			// Look for more rules if we are at the end of the line
			
			if (i == line.str.length() && (open != NULL || close)) {
				done = false;
			}
		}
		
		if (i == 0 && !applied) {
			line.parserStates.push_back(std::pair<unsigned, ParserState>(i, current));
		}
	}
	
	line.validParse = true;
}
