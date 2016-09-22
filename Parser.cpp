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

#include "Document.h"


/**
 * Create a new parser rule
 *
 * @param _environment the environment
 * @param _token the token
 * @param _closeCurrent true to close the current environment
 * @param _openEnvironment the new environment to open (or NULL)
 */
ParserRule::ParserRule(ParserEnvironment* _environment, const char* _token,
                       bool _closeCurrent, ParserEnvironment* _openEnvironment)
{
	environment = _environment;
	token = _token;
	closeCurrent = _closeCurrent;
	openEnvironment = _openEnvironment;
	mustStartLine = false;
	mustEndLine = false;
}


/**
 * Destroy the rule
 */
ParserRule::~ParserRule()
{
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
			for (ParserRule*& rule : *ruleTable[i]) delete rule;
			delete ruleTable[i];
		}
	}
}


/**
 * Add a rule
 *
 * @param rule the rule (will take ownership of the class)
 */
void ParserEnvironment::AddRule(ParserRule* rule)
{
	assert(rule->Environment() == this);
	
	char firstLetter = rule->Token()[0];
	if (firstLetter >= 127) firstLetter = 127;
	
	std::vector<ParserRule*>*& bucket = ruleTable[firstLetter];
	if (bucket == NULL) bucket = new std::vector<ParserRule*>();
	bucket->push_back(rule);
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
 * Parse the next line
 *
 * @param line the document line
 */
void Parser::Parse(DocumentLine& line)
{
	// TODO
}

