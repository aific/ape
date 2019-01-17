/*
 * Parser.h
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


#ifndef __PARSER_H
#define __PARSER_H

#include <string>
#include <vector>


// Forward declarations

/*
 * The document line
 */
class DocumentLine;

/*
 * The document line collection
 */
class DocumentLineCollection;

/*
 * A parser environment
 */
class ParserEnvironment;

/*
 * The parser
 */
class Parser;


/**
 * A parser rule
 */
class ParserRule
{
	friend class ParserEnvironment;
	
	std::string token;
	
	bool closeCurrent;
	ParserEnvironment* openEnvironment;

	bool mustStartLine;
	bool mustEndLine;
	bool wholeWord;
	
	int referenceCount;


public:

	/**
	 * Create a new parser rule
	 *
	 * @param token the token
	 * @param closeCurrent true to close the current environment
	 * @param openEnvironment the new environment to open (or NULL)
	 */
	ParserRule(const char* token,
	           bool closeCurrent, ParserEnvironment* openEnvironment);
	
	/**
	 * Destroy the rule
	 */
	virtual ~ParserRule();
	
	/**
	 * Determine if the rule matches the given string at the specified location
	 *
	 * @param line the line
	 * @param pos the position (character index) in the line
	 * @return true if it matches
	 */
	virtual bool Matches(const char* line, unsigned pos);
	 
	/**
	 * Get the token
	 *
	 * @return the token
	 */
	inline const char* Token() const { return token.c_str(); }
	 
	/**
	 * Determine whether this rule closes the current environment
	 *
	 * @return true if it closes the current environment; false otherwise
	 */
	inline bool ClosesCurrentEnvironment() const { return closeCurrent; }
	
	/**
	 * Get the environment that this rule opens
	 *
	 * @return the new environment, or NULL if it does not open any
	 */
	inline ParserEnvironment* OpensEnvironment() const { return openEnvironment; }
	
	/**
	 * Determine if this token must be at the beginning of the line, at most
	 * ignoring the whitespace
	 *
	 * @return true if the token must be at the beginning of the line
	 */
	inline bool MustStartLine() const { return mustStartLine; }
	
	/**
	 * Set if this token must be at the beginning of the line (ignoring the
	 * whitespace)
	 *
	 * @param value true to enable, false to disable
	 * @return this object (for call chaining)
	 */
	 inline ParserRule* SetMustStartLine(bool value)
	 {
	 	mustStartLine = value;
	 	return this;
	 }
	
	/**
	 * Determine if this token must be at the end of the line, at most
	 * ignoring the whitespace
	 *
	 * @return true if the token must be at the end of the line
	 */
	inline bool MustEndLine() const { return mustEndLine; }
	
	/**
	 * Set if this token must be at the end of the line (ignoring the
	 * whitespace)
	 *
	 * @param value true to enable, false to disable
	 * @return this object (for call chaining)
	 */
	 inline ParserRule* SetMustEndLine(bool value)
	 {
	 	mustEndLine = value;
	 	return this;
	 }
	
	/**
	 * Determine if this token must match a whole word
	 *
	 * @return true if the token must be a whole word
	 */
	inline bool WholeWord() const { return wholeWord; }
	
	/**
	 * Set if this token must be a whole word
	 *
	 * @param value true to enable, false to disable
	 * @return this object (for call chaining)
	 */
	inline ParserRule* SetWholeWord(bool value)
	{
		wholeWord = value;
		return this;
	}
};


/**
 * A parser environment
 */
class ParserEnvironment
{
	std::string name;
	int color;
	
	// The table of rules indexed by the first character of the token for fast
	// access. Empty tokens (such as for the EOL) are accessible from index 0,
	// and characters >= 127 through index 127
	std::vector<ParserRule*>* ruleTable[128];


public:

	/**
	 * Create a new parser environment
	 *
	 * @param name the environment name
	 * @param color the default color
	 */
	ParserEnvironment(const char* name, int color);
	
	/**
	 * Destroy the parser environment
	 */
	virtual ~ParserEnvironment();
	
	/**
	 * Get the name of the parser environment
	 *
	 * @return the name
	 */
	inline const char* Name() const { return name.c_str(); }
	
	/**
	 * Get the default color associated with the environment
	 *
	 * @return the color
	 */
	inline int Color() const { return color; }
	
	/**
	 * Add a rule
	 *
	 * @param rule the rule (will reference-count the object and destroy it at the end if appropriate)
	 */
	void AddRule(ParserRule* rule);
	
	/**
	 * Find a rule rule matches the given string at the specified location
	 *
	 * @param line the line
	 * @param pos the position (character index) in the line
	 * @return the matching rule, or NULL if none
	 */
	ParserRule* FindMatchingRule(const char* line, unsigned pos);
};


/**
 * A parser state
 */
class ParserState
{
	friend class Parser;
	
	std::vector<ParserEnvironment*> environmentStack;
	
	
public:

	/**
	 * Create a new parser state
	 */
	ParserState();
	
	/**
	 * Destroy the parser state
	 */
	virtual ~ParserState();
	
	/**
	 * Clear the state
	 */
	void Clear();
	
	/**
	 * Get the current environment
	 *
	 * @return the latest environment in the stack
	 */
	ParserEnvironment* Environment() const;
	
	/**
	 * Compare this parser state to another state
	 *
	 * @param other the other state
	 * @return true if they are the equal
	 */
	 bool operator== (const ParserState& other) const;
	
	/**
	 * Compare this parser state to another state
	 *
	 * @param other the other state
	 * @return true if they are the not equal
	 */
	 inline bool operator!= (const ParserState& other) const
	 {
	 	return !(*this == other);
	 }
};


/**
 * Parser
 */
class Parser
{
	std::vector<ParserEnvironment*> environments;
	ParserEnvironment* globalEnvironment;


public:

	/**
	 * Create a new instance of the parser
	 */
	Parser();
	
	/**
	 * Destroy the parser
	 */
	virtual ~Parser();
	
	/**
	 * Get the global environment
	 *
	 * @return the environment, or NULL if not set
	 */
	inline ParserEnvironment* GlobalEnvironment() const
	{
		return globalEnvironment;
	}
	
	/**
	 * Add an environment. The first added environment is automatically set as
	 * the global environment
	 *
	 * @param environment the environment
	 */
	void AddEnvironment(ParserEnvironment* environment);
	
	/**
	 * Parse the next chunk
	 *
	 * @param lines the document commection
	 * @param line the line number (0 based)
	 * @param offset the offset (0 based)
	 * @param newLine where to write the new line number
	 * @param newOffset where to write the new offset
	 */
	void Parse(DocumentLineCollection& lines, unsigned line, unsigned offset,
	           unsigned& newLine, unsigned& newOffset);
	
	/**
	 * Parse the next line
	 *
	 * @param line the document line
	 * @param previous the previous line, or NULL if not applicable
	 */
	void Parse(DocumentLine& line, const DocumentLine* previous);
};


#endif
