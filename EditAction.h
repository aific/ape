/*
 * EditAction.h
 *
 * Copyright (c) 2015, Peter Macko
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


#ifndef __EDIT_ACTION_H
#define __EDIT_ACTION_H

#include <vector>

class DocumentLine;
class EditorDocument;
class Histogram;


/**
 * The enumeration of atomic edit actions
 */
typedef enum {
	EAT_None,
	EAT_Compound,
	EAT_InsertChar,
	EAT_DeleteChar,
	EAT_InsertLine,
	EAT_ReplaceLine,
	EAT_DeleteLine,
	EAT_InsertString,
	EAT_DeleteString
} EditActionType;


/**
 * An atomic edit action
 *
 * @author Peter Macko
 */
class EditAction
{
	EditActionType type;
	
	int afterCurRow;
	int afterCurColumn;
	

protected:
	
	/**
	 * Create an instance of class EditAction
	 * 
	 * @param actionType the action type
	 */
	EditAction(EditActionType actionType);
	
	/**
	 * Return a line from a document
	 * 
	 * @param doc the document
	 * @param row the row
	 * @return the line
	 */
	DocumentLine& Line(EditorDocument* doc, int row);
	
	/**
	 * Return the line display lengths
	 * 
	 * @param doc the document
	 * @return the line length histogram
	 */
	Histogram& DisplayLengths(EditorDocument* doc);
	
	/**
	 * Insert a line and update the appropriate meta-data
	 * 
	 * @param doc the document
	 * @param row the row before which to insert
	 * @param contents the line contents
	 */
	void InsertLine(EditorDocument* doc, int row, const char* contents);
	
	/**
	 * Delete a line and update the appropriate meta-data
	 * 
	 * @param doc the document
	 * @param row the row
	 */
	void DeleteLine(EditorDocument* doc, int row);
	
	/**
	 * Insert a string
	 * 
	 * @param doc the document
	 * @param line the first line
	 * @param pos the string position
	 * @param str the string to insert
	 */
	void InsertString(EditorDocument* doc, int line, int pos, const char* str);
	
	/**
	 * Delete a string
	 * 
	 * @param doc the document
	 * @param line the first line
	 * @param pos the string position
	 * @param toline the last line
	 * @param topos the position within the last line
	 */
	void DeleteString(EditorDocument* doc, int line, int pos, int toline, int topos);
	
	
public:

	/**
	 * Destroy the object
	 */
	virtual ~EditAction(void);
	
	/**
	 * Return the type of the action
	 * 
	 * @return the type of the action
	 */
	inline EditActionType Type(void) { return type; }
	
	/**
	 * Undo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Undo(EditorDocument* doc) = 0;
	
	/**
	 * Redo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Redo(EditorDocument* doc) = 0;
};


/**
 * A line-level edit action
 *
 * @author Peter Macko
 */
class LineEditAction : public EditAction
{

protected:
	
	char* contents;
	int row;
	
	
	/**
	 * Create an instance of class LineEditAction
	 * 
	 * @param actionType the action type
	 * @param row the row
	 * @param contents the line contents
	 */
	LineEditAction(EditActionType actionType, int row, const char* contents);


public:

	/**
	 * Destroy the object
	 */
	virtual ~LineEditAction(void);
};


/**
 * A character-level edit action
 *
 * @author Peter Macko
 */
class CharacterEditAction : public EditAction
{

protected:
	
	char ch;
	int row;
	int pos;
	
	
	/**
	 * Create an instance of class CharacterEditAction
	 * 
	 * @param actionType the action type
	 * @param row the row
	 * @param pos the string position
	 * @param ch the character
	 */
	CharacterEditAction(EditActionType actionType, int row, int pos, char ch);


public:

	/**
	 * Destroy the object
	 */
	virtual ~CharacterEditAction(void);
};


/**
 * A string-level edit action
 *
 * @author Peter Macko
 */
class StringEditAction : public EditAction
{

protected:
	
	char* contents;
	int row;
	int pos;
	
	int newlines;
	char* last;
	int lastlength;
	
	
	/**
	 * Create an instance of class LineEditAction
	 * 
	 * @param actionType the action type
	 * @param row the row
	 * @param pos the position
	 * @param contents the line contents
	 */
	StringEditAction(EditActionType actionType, int row, int pos, const char* contents);


public:

	/**
	 * Destroy the object
	 */
	virtual ~StringEditAction(void);
};


/**
 * Edit action: insert a character
 * 
 * @author Peter Macko
 */
class EA_InsertChar : public CharacterEditAction
{
	
public:

	/**
	 * Create an instance of class EA_InsertChar
	 * 
	 * @param row the row
	 * @param pos the string position
	 * @param ch the character
	 */
	EA_InsertChar(int row, int pos, char ch);
	
	/**
	 * Destroy the object
	 */
	virtual ~EA_InsertChar(void);
	
	/**
	 * Undo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Undo(EditorDocument* doc);
	
	/**
	 * Redo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Redo(EditorDocument* doc);
};


/**
 * Edit action: delete a character
 * 
 * @author Peter Macko
 */
class EA_DeleteChar : public CharacterEditAction
{
	
public:

	/**
	 * Create an instance of class EA_DeleteChar
	 * 
	 * @param row the row
	 * @param pos the string position
	 * @param ch the character
	 */
	EA_DeleteChar(int row, int pos, char ch);
	
	/**
	 * Destroy the object
	 */
	virtual ~EA_DeleteChar(void);
	
	/**
	 * Undo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Undo(EditorDocument* doc);
	
	/**
	 * Redo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Redo(EditorDocument* doc);
};


/**
 * Edit action: insert a line
 * 
 * @author Peter Macko
 */
class EA_InsertLine : public LineEditAction
{
	
public:

	/**
	 * Create an instance of class EA_InsertLine
	 * 
	 * @param row the row
	 * @param contents the line contents
	 */
	EA_InsertLine(int row, const char* cotents);
	
	/**
	 * Destroy the object
	 */
	virtual ~EA_InsertLine(void);
	
	/**
	 * Undo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Undo(EditorDocument* doc);
	
	/**
	 * Redo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Redo(EditorDocument* doc);
};


/**
 * Edit action: delete a line
 * 
 * @author Peter Macko
 */
class EA_DeleteLine : public LineEditAction
{
	
public:

	/**
	 * Create an instance of class EA_DeleteLine
	 * 
	 * @param row the row
	 * @param contents the line contents
	 */
	EA_DeleteLine(int row, const char* cotents);
	
	/**
	 * Destroy the object
	 */
	virtual ~EA_DeleteLine(void);
	
	/**
	 * Undo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Undo(EditorDocument* doc);
	
	/**
	 * Redo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Redo(EditorDocument* doc);
};


/**
 * Edit action: replace a line
 * 
 * @author Peter Macko
 */
class EA_ReplaceLine : public LineEditAction
{
	char* original;
	
	
public:

	/**
	 * Create an instance of class EA_ReplaceLine
	 * 
	 * @param row the row
	 * @param original the original line contents
	 * @param contents the new line contents
	 */
	EA_ReplaceLine(int row, const char* original, const char* cotents);
	
	/**
	 * Destroy the object
	 */
	virtual ~EA_ReplaceLine(void);
	
	/**
	 * Undo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Undo(EditorDocument* doc);
	
	/**
	 * Redo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Redo(EditorDocument* doc);
};


/**
 * Edit action: insert a string
 * 
 * @author Peter Macko
 */
class EA_InsertString : public StringEditAction
{
	
public:

	/**
	 * Create an instance of class EA_InsertString
	 * 
	 * @param row the row
	 * @param pos the string position
	 * @param contents the contents
	 */
	EA_InsertString(int row, int pos, const char* contents);
	
	/**
	 * Destroy the object
	 */
	virtual ~EA_InsertString(void);
	
	/**
	 * Undo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Undo(EditorDocument* doc);
	
	/**
	 * Redo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Redo(EditorDocument* doc);
};


/**
 * Edit action: delete a string
 * 
 * @author Peter Macko
 */
class EA_DeleteString : public StringEditAction
{
	
public:

	/**
	 * Create an instance of class EA_DeleteString
	 * 
	 * @param row the row
	 * @param pos the string position
	 * @param contents the contents
	 */
	EA_DeleteString(int row, int pos, const char* contents);
	
	/**
	 * Destroy the object
	 */
	virtual ~EA_DeleteString(void);
	
	/**
	 * Undo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Undo(EditorDocument* doc);
	
	/**
	 * Redo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Redo(EditorDocument* doc);
};


/**
 * A compound edit action
 * 
 * @author Peter Macko
 */
class CompoundEditAction : public EditAction
{
	std::vector<EditAction*> actions;
	
	
public:

	/**
	 * Create an instance of class CompoundEditAction
	 */
	CompoundEditAction(void);
	
	/**
	 * Destroy the object
	 */
	virtual ~CompoundEditAction(void);
	
	/**
	 * Add an action
	 * 
	 * @param a the action (will be destroyed together with this object)
	 */
	void Add(EditAction* a);
	
	/**
	 * Determine whether compound action is empty
	 * 
	 * @return true if there are no actions within this compound
	 */
	inline bool Empty(void) { return actions.empty(); }
	
	/**
	 * Undo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Undo(EditorDocument* doc);
	
	/**
	 * Redo the action
	 * 
	 * @param doc the document to which this operation applies to
	 */
	virtual void Redo(EditorDocument* doc);
};


#endif

