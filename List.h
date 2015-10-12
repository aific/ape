/*
 * List.h
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


#ifndef __LIST_H
#define __LIST_H

#include "Component.h"

#include <vector>
#include "ScrollBar.h"


/**
 * The list
 *
 * @author Peter Macko
 */
class AbstractList : public Component
{
	int cursor;
	int pageStart;

	bool selection;
	int selStart;

	int cursorBg;
	int cursorFg;
	int selBg;
	int selFg;

	std::vector<std::string> elements;

	ScrollBar* vertScroll;
	ScrollBar* internalVertScroll;
	bool scrollBarsReflectCursor;
	

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
	 * Move the cursor one page up
	 */
	void MoveCursorPageUp(void);
	
	/**
	 * Move the cursor one page down
	 */
	void MoveCursorPageDown(void);
	
	/**
	 * Move the cursor to the beginning
	 */
	void MoveCursorToBeginning(void);
	
	/**
	 * Move the cursor to the end
	 */
	void MoveCursorToEnd(void);
	
	/**
	 * Paint an element
	 * 
	 * @param index the element index
	 */
	void PaintElement(int index);
	
	/**
	 * Scroll the window such that the cursor is visible
	 * 
	 * @return true if the scrolling actually happened
	 */
	bool EnsureValidScroll(void);

	/**
	 * Perform any necessary actions after moving a cursor
	 */
	void CursorMoved(void);
	

protected:
	
	/**
	 * An event handler for pressing a key
	 * 
	 * @param key the key code
	 */
	virtual void OnKeyPressed(int key);

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
	 * Create an instance of class AbstractList
	 *
	 * @param parent the parent container
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param anchor set the anchor
	 */
	AbstractList(Container* parent, int row = 0, int col = 0,
			int rows = 10, int cols = 16,
			int anchor = ANCHOR_LEFT | ANCHOR_TOP);

	/**
	 * Destroy the object
	 */
	virtual ~AbstractList(void);
	
	/**
	 * Paint the contents of the window
	 */
	virtual void Paint(void);

	/**
	 * Set the scroll bar
	 *
	 * @param vert the vertical scroll bar
	 */
	void SetScrollBar(ScrollBar* vert);

	/**
	 * Get the cursor position (index into the list)
	 *
	 * @return the cursor position (index)
	 */
	inline int Cursor(void) { return cursor; }
};

#endif

