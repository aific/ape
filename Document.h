/*
 * Document.h
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


#ifndef __DOCUMENT_H
#define __DOCUMENT_H

#include <deque>
#include <string>
#include <vector>

#include "EditAction.h"
#include "Histogram.h"

class EditorDocument;


/**
 * A line
 */
typedef struct _DocumentLine
{
	std::string str;
	int displayLength;
	
	_DocumentLine(void) {
		str = std::string();
		displayLength = 0;
	}
	
	_DocumentLine(const _DocumentLine& l) {
		*this = l;
	}
	
	_DocumentLine& operator=(const _DocumentLine& l) {
		str = l.str;
		displayLength = l.displayLength;
		return *this;
	}
	
} DocumentLine;


/**
 * An undo entry
 */
class UndoEntry
{
	friend class EditorDocument;
	
	EditorDocument* document;
	CompoundEditAction* action;
	
	int cursorRow;
	int cursorColumn;
	bool modified;
	
	int redo_cursorRow;
	int redo_cursorColumn;
	bool redo_modified;
	
	
public:
	
	/**
	 * Create an instance of class UndoEntry
	 * 
	 * @param document the document
	 * @param cursorRow the cursor row
	 * @param cursorColumn the cursor column
	 * @param modified the modification status of a document
	 */
	UndoEntry(EditorDocument* document, int cursorRow, int cursorColumn, bool modified);
	
	/**
	 * Destroy the object
	 */
	~UndoEntry(void);
	
	/**
	 * Add an edit action
	 * 
	 * @param a the action (will be destroyed together with the object)
	 */
	void Add(EditAction* a);
	
	/**
	 * Perform an undo
	 */
	void Undo(void);
	
	/**
	 * Perform a redo
	 */
	void Redo(void);
};


/**
 * The document
 *
 * @author Peter Macko
 */
class EditorDocument
{
	friend class EditAction;
	friend class UndoEntry;

	std::string fileName;
	
	std::vector<DocumentLine> lines;
	Histogram displayLengths;
	
	int pageStart;
	bool modified;
	int tabSize;
	
	int cursorRow;
	int cursorColumn;
	
	UndoEntry* currentUndo;
	std::deque<UndoEntry*> undo;
	std::deque<UndoEntry*> redo;
	
	
	/**
	 * Update the line metadata
	 * 
	 * @param l the line object
	 */
	void UpdateLineMetadata(DocumentLine& l);
	
	/**
	 * Prepare the document for an edit
	 */
	void PrepareEdit(void);
	
	/**
	 * Just insert a string
	 * 
	 * @param line the first line
	 * @param pos the string position
	 * @param str the string to insert
	 */
	void InsertStringEx(int line, int pos, const char* str);
	
	/**
	 * Just delete a string
	 * 
	 * @param line the first line
	 * @param pos the string position
	 * @param toline the last line
	 * @param topos the position within the last line
	 */
	void DeleteStringEx(int line, int pos, int toline, int topos);
	
	
public:
	
	/**
	 * Create an instance of class EditorDocument
	 */
	EditorDocument(void);

	/**
	 * Destroy the object
	 */
	virtual ~EditorDocument(void);
	
	/**
	 * Clear the text document
	 */
	void Clear(void);

	/**
	 * Load from file, and set the associated document file name
	 *
	 * @param file the file name
	 */
	void LoadFromFile(const char* file);

	/**
	 * Save to file
	 *
	 * @param file the file name
	 * @param switchFile whether to set the associated file name and clear
	 *                   the modified flag
	 * @return a ReturnExt
	 */
	ReturnExt SaveToFile(const char* file, bool switchFile=false);

	/**
	 * Save to the current file
	 *
	 * @return a ReturnExt
	 */
	ReturnExt Save(void);

	/**
	 * Get the associated file name
	 *
	 * @return the associated file name, or NULL if none
	 */
	inline const char* FileName(void) {
		const char* s = fileName.c_str();
		return *s == '\0' ? NULL : s;
	}
	
	/**
	 * Get the number of lines
	 * 
	 * @return the number of lines
	 */
	inline int NumLines(void) { return lines.size(); }
	
	/**
	 * Set the start of the page
	 * 
	 * @param start the page start line
	 */
	void SetPageStart(int start);
	
	/**
	 * Return the line that is the first on the current page
	 * 
	 * @return the number of the first line of the page (screen)
	 */
	inline int PageStart(void) { return pageStart; }
	
	/**
	 * Return a line
	 * 
	 * @param line the line number
	 * @return the line string
	 */
	inline const char* Line(int line)
	{
		return line >= lines.size() ? "" : lines[line].str.c_str();
	}
	
	/**
	 * Return a line relative to the page start
	 * 
	 * @param line the line number relative to the page start
	 * @return the line string
	 */
	inline const char* LineRel(int line) { return Line(pageStart + line); }
	
	/**
	 * Determine whether the document was modified since the last save
	 * 
	 * @return true if the document was modified
	 */
	inline bool Modified(void) { return modified; }
	
	/**
	 * Get the display length of the line
	 * 
	 * @param line the line string
	 * @return the display length
	 */
	int DisplayLength(const char* line);
	
	/**
	 * Return the display length of a line
	 * 
	 * @param line the line number
	 * @return the display length
	 */
	inline int DisplayLength(int line)
	{
		return line >= lines.size() ? 0 : lines[line].displayLength;
	}
	
	/**
	 * Return the maximum display length
	 * 
	 * @return the maximum display length
	 */
	int MaxDisplayLength(void);
	
	/**
	 * Return the string position corresponding to the given cursor position
	 * 
	 * @param line the line number
	 * @param cursor the cursor position
	 * @return the string position index
	 */
	int StringPosition(int line, int cursor);

	/**
	 * Return the cursor position corresponding to the given string offset
	 * 
	 * @param line the line number
	 * @param cursor the cursor position
	 * @return the cursor position, or the maximum position if out of bounds
	 */
	int CursorPosition(int line, size_t offset);
	
	/**
	 * Set the cursor location (used for undo logging)
	 * 
	 * @param row the row
	 * @param column the column
	 */
	void SetCursorLocation(int row, int column);
	
	/**
	 * Return the cursor row
	 * 
	 * @param the row
	 */
	inline int CursorRow(void) { return cursorRow; }
	
	/**
	 * Return the cursor column
	 * 
	 * @param the column
	 */
	inline int CursorColumn(void) { return cursorColumn; }
	
	/**
	 * Append a line
	 * 
	 * @param line the line
	 */
	void Append(const char* line);
	
	/**
	 * Insert a new line
	 * 
	 * @param pos the line before which to insert
	 * @param line the new line
	 */
	void Insert(int pos, const char* line);
	
	/**
	 * Replace a line
	 * 
	 * @param pos the line to replace
	 * @param line the new contents of the line
	 */
	void Replace(int pos, const char* line);
	
	/**
	 * Insert a character to a line
	 * 
	 * @param line the line
	 * @param ch the character to insert
	 * @param pos the string position
	 */
	void InsertCharToLine(int line, char ch, int pos);
	
	/**
	 * Delete a character from line
	 * 
	 * @param line the line
	 * @param pos the string position
	 */
	void DeleteCharFromLine(int line, int pos);

	/**
	 * Join two subsequent lines
	 * 
	 * @param line the first line to join
	 */
	void JoinTwoLines(int line);
	
	/**
	 * Insert a string
	 * 
	 * @param line the first line
	 * @param pos the string position
	 * @param str the string to insert
	 */
	void InsertString(int line, int pos, const char* str);
	
	/**
	 * Delete a string
	 * 
	 * @param line the first line
	 * @param pos the string position
	 * @param toline the last line
	 * @param topos the position within the last line
	 */
	void DeleteString(int line, int pos, int toline, int topos);
	
	/**
	 * Perform an undo
	 */
	void Undo(void);
	
	/**
	 * Perform a redo
	 */
	void Redo(void);
	
	/**
	 * Get a substring
	 * 
	 * @param line the first line
	 * @param pos the string position
	 * @param toline the last line
	 * @param topos the position within the last line
	 */
	std::string Get(int line, int pos, int toline, int topos);
	
	/**
	 * Finalize an edit action (for undo purposes)
	 */
	void FinalizeEditAction(void);
};

#endif
