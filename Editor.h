/*
 * Editor.h
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


#ifndef __EDITOR_H
#define __EDITOR_H

#include "Component.h"

#include "Document.h"
#include "ScrollBar.h"


/**
 * A type of an editor edit action
 */
typedef enum {
	EEAT_None,
	EEAT_Type,
	EEAT_TypeWhitespace,
	EEAT_Delete,
	EEAT_Backspace,
	EEAT_Indent,
	EEAT_Enter,
	EEAT_Cut
} EditorEditActionType;


/**
 * The text and source code editor
 *
 * @author Peter Macko
 */
class Editor : public Component
{
	EditorDocument* doc;
	bool multiline;
	int colStart;
	
	int tabSize;
	bool displayTabs;
	
	int row;
	int col;
	int actualCol;
	int offsetWithinLine;
	int wheelSpeed;
	
	int selRow;
	int selCol;
	bool selection;
	
	bool overwriteMode;
	bool scrollBarsReflectCursor;
	
	EditorEditActionType lastAction;

	ScrollBar* horizScroll;
	ScrollBar* vertScroll;

	std::string highlightPattern;


	/**
	 * Move the cursor in the document
	 *
	 * @param newRow the new row
	 * @param newCol the new column
	 * @param shift whether the Shift button was held
	 */
	void MoveDocumentCursor(int newRow, int newCol, bool shift = false);
	
	/**
	 * Move the cursor up
	 * 
	 * @param shift whether the Shift button was held
	 */
	void MoveCursorUp(bool shift = false);
	
	/**
	 * Move the cursor down
	 * 
	 * @param shift whether the Shift button was held
	 */
	void MoveCursorDown(bool shift = false);
	
	/**
	 * Move the cursor left
	 * 
	 * @param shift whether the Shift button was held
	 */
	void MoveCursorLeft(bool shift = false);
	
	/**
	 * Move the cursor right
	 * 
	 * @param shift whether the Shift button was held
	 */
	void MoveCursorRight(bool shift = false);
	
	/**
	 * Move the cursor to the beginning of the word or to the previous word
	 * 
	 * @param shift whether the Shift button was held
	 */
	void MoveCursorBeginningWord(bool shift = false);
	
	/**
	 * Move the cursor to the end of the word or to the next word
	 * 
	 * @param shift whether the Shift button was held
	 */
	void MoveCursorEndWord(bool shift = false);
	
	/**
	 * Move the cursor to the very left
	 */
	void MoveCursorVeryLeft(void);
	
	/**
	 * Move the cursor to the beginning of the line
	 * 
	 * @param shift whether the Shift button was held
	 */
	void MoveCursorToTheBeginningOfTheLine(bool shift = false);
	
	/**
	 * Move the cursor to the very right
	 * 
	 * @param shift whether the Shift button was held
	 */
	void MoveCursorVeryRight(bool shift = false);
	
	/**
	 * Move the cursor one page up
	 * 
	 * @param shift whether the Shift button was held
	 */
	void MoveCursorPageUp(bool shift = false);
	
	/**
	 * Move the cursor one page down
	 * 
	 * @param shift whether the Shift button was held
	 */
	void MoveCursorPageDown(bool shift = false);
	
	/**
	 * Update the actual cursor position
	 */
	void UpdateActualCursorPosition(void);
	
	/**
	 * Update the cursor location and refresh
	 *
	 * @param scroll whether to scroll to make the cursor visible
	 */
	void UpdateCursor(bool scroll = true);
	
	/**
	 * Insert a character at the cursor position
	 * 
	 * @param c the character
	 */
	void InsertChar(char c);
	
	/**
	 * Insert a new line at the cursor position
	 */
	void NewLine(void);
	
	/**
	 * Delete a character at the cursor position
	 */
	void DeleteChar(void);
	
	/**
	 * Delete a character before the cursor position
	 */
	void Backspace(void);
	
	/**
	 * Indent the current line or selection
	 */
	void Indent(void);
	
	/**
	 * Unindent the current line or selection
	 */
	void Unindent(void);
	
	/**
	 * Copy a string to the clipboard
	 */
	void Copy(void);
	
	/**
	 * Cut a string and copy it to the clipboard
	 */
	void Cut(void);
	
	/**
	 * Paste a string from the clipboard
	 */
	void Paste(void);
	
	/**
	 * Perform an undo
	 */
	void Undo(void);
	
	/**
	 * Perform a redo
	 */
	void Redo(void);
	
	/**
	 * Perform the necessary operations after an edit
	 */
	void AfterEdit(void);
	
	/**
	 * Delete the selection, if applicable
	 */
	void DeleteSelection(void);
	
	/**
	 * Paint a line
	 * 
	 * @param line the line number
	 */
	void PaintLine(int line);
	
	/**
	 * Scroll the window such that the cursor is visible
	 * 
	 * @return true if the scrolling actually happened
	 */
	bool EnsureValidScroll(void);
	

protected:
	
	/**
	 * An event handler for pressing a key
	 * 
	 * @param key the key code
	 */
	virtual void OnKeyPressed(int key);
	
	/**
	 * An event handler for mouse press
	 *
	 * @param row the row
	 * @param column the column
	 * @param button the button
	 * @param shift whether shift was pressed
	 */
	virtual void OnMousePress(int row, int column, int button, bool shift);
	
	/**
	 * An event handler for mouse drag
	 *
	 * @param row the row
	 * @param column the column
	 * @param button the button
	 * @param shift whether shift was pressed
	 */
	virtual void OnMouseDrag(int row, int column, int button, bool shift);
	
	/**
	 * An event handler for finishing mouse drag
	 *
	 * @param row the row
	 * @param column the column
	 * @param button the button
	 * @param shift whether shift was pressed
	 */
	virtual void OnMouseDragFinish(int row, int column, int button, bool shift);
	
	/**
	 * An event handler for mouse wheel
	 *
	 * @param row the row
	 * @param column the column
	 * @param wheel the wheel direction
	 */
	virtual void OnMouseWheel(int row, int column, int wheel);

	/**
	 * An event handler for resizing the component
	 *
	 * @param oldRows the old number of rows
	 * @param oldCols the old number of columns
	 * @param newRows the new number of rows
	 * @param newCols the new number of columns
	 */
	virtual void OnResize(int oldRows, int oldCols, int newRows, int newCols);
	

public:
	
	/**
	 * Create an instance of class Editor
	 *
	 * @param parent the parent container
	 * @param multiline true for a multiline editor
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param anchor set the anchor
	 */
	Editor(Container* parent, bool multiline = false, int row = 0, int col = 0,
			int rows = 1, int cols = 16,
			int anchor = ANCHOR_LEFT | ANCHOR_TOP);

	/**
	 * Destroy the object
	 */
	virtual ~Editor(void);

	/**
	 * Determine if this is a multiline editor
	 *
	 * @return true if this is a multiline editor
	 */
	inline bool Multiline(void) { return multiline; }

	/**
	 * Load from file, and set the associated document file name
	 *
	 * @param file the file name
	 * @param a ReturnExt
	 */
	ReturnExt LoadFromFile(const char* file);

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
	 * Paint the contents of the window
	 */
	virtual void Paint(void);

	/**
	 * Set the scroll bars
	 *
	 * @param horiz the horizontal scroll bar
	 * @param vert the vertical scroll bar
	 */
	void SetScrollBars(ScrollBar* horiz, ScrollBar* vert);
	
	/**
	 * Get the logical cursor row (position) within the document
	 *
	 * @return the logical row number
	 */
	inline int DocumentCursorRow(void) { return row; }
	
	/**
	 * Get the logical cursor column (position) within the document
	 *
	 * @return the logical column number
	 */
	inline int DocumentCursorColumn(void) { return actualCol; }
	
	/**
	 * Get the editor document
	 *
	 * @return the document
	 */
	inline EditorDocument* Document(void) { return doc; }
	 
	 /**
	  * Determine whether the editor is in the overwrite mode
	  *
	  * @return true if it is in the overwrite mode
	  */
	inline bool OverwriteMode(void) { return overwriteMode; }
	
	/**
	 * Select all
	 */
	void SelectAll(void);

	/**
	 * Return the search pattern
	 *
	 * @return the pattern, or NULL if none 
	 */
	inline const char* HighlightPattern(void)
	{
		return highlightPattern == "" ? NULL : highlightPattern.c_str();
	}

	/**
	 * Set the highlight pattern
	 *
	 * @param pattern the pattern (use an empty string or NULL to clear)
	 */
	void SetHighlightPattern(const char* pattern);

	/**
	 * Move the cursor to the next pattern match
	 *
	 * @param forward true to search forward, false to search backwards
	 * @param keepIfOnMatch if the cursor is already at a match, keep it there
	 * @param wrap true to wrap around
	 * @return true if there is at least a single match
	 */
	bool FindNext(bool forward, bool keepIfOnMatch, bool wrap = true);
};

#endif







