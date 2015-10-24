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

#include <sstream>
#include <string>
#include <vector>
#include "ScrollBar.h"


class AbstractList;


/**
 * The item renderer
 *
 * @author Peter Macko
 */
template <typename T>
class ListItemRenderer
{

public:

	/**
	 * Create an instance of class ListItemRenderer
	 */
	inline ListItemRenderer() {}

	/**
	 * Destroy an instance of class ListItemRenderer
	 */
	virtual ~ListItemRenderer() {}

	/**
	 * Paint a string list item
	 *
	 * @param list the list component
	 * @param tcw the output buffer
	 * @param item the item
	 * @param active true if the item is active (under cursor)
	 * @param selected true if the item is selected
	 * @param highlightPattern the highlight pattern
	 */
	static void PaintStringListItem(AbstractList* list, TerminalControlWindow* tcw,
			const std::string& item, bool active, bool selected,
			const char* highlightPattern);

	/**
	 * Paint a list item
	 *
	 * @param list the list component
	 * @param tcw the output buffer
	 * @param item the item
	 * @param active true if the item is active (under cursor)
	 * @param selected true if the item is selected
	 * @param highlightPattern the highlight pattern
	 */
	virtual void Paint(AbstractList* list, TerminalControlWindow* tcw,
			const T& item, bool active, bool selected,
			const char* highlightPattern) = 0;
};


/**
 * Paint a string list item
 *
 * @param list the list component
 * @param tcw the output buffer
 * @param item the item
 * @param active true if the item is active (under cursor)
 * @param selected true if the item is selected
 * @param highlightPattern the highlight pattern
 */
void PaintStringListItem(AbstractList* list, TerminalControlWindow* tcw,
		const char* item, bool active, bool selected,
		const char* highlightPattern);


/**
 * The default item renderer
 *
 * @author Peter Macko
 */
template <typename T>
class DefaultListItemRenderer : ListItemRenderer<T>
{

public:

	/**
	 * Create an instance of class DefaultListItemRenderer
	 */
	inline DefaultListItemRenderer() {}

	/**
	 * Destroy an instance of class DefaultListItemRenderer
	 */
	virtual ~DefaultListItemRenderer() {}

	/**
	 * Paint a list item
	 *
	 * @param list the list component
	 * @param tcw the output buffer
	 * @param item the item
	 * @param active true if the item is active (under cursor)
	 * @param selected true if the item is selected
	 * @param highlightPattern the highlight pattern
	 */
	virtual void Paint(AbstractList* list, TerminalControlWindow* tcw,
			const T& item, bool active, bool selected,
			const char* highlightPattern)
	{
		std::ostringstream ss; ss << item;
		PaintStringListItem(list, tcw, ss.str().c_str(), active, selected,
		                    highlightPattern);
	}
};


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

	/**
	 * Paint a list item
	 *
	 * @param tcw the output buffer
	 * @param index the item index
	 * @param active true if the item is active (under cursor)
	 * @param selected true if the item is selected
	 * @param highlightPattern the highlight pattern
	 */
	virtual void PaintListItem(TerminalControlWindow* tcw, int index,
			bool active, bool selected) = 0;
	

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
	 * Get the vertical scroll bar, if any
	 *
	 * @return the vertical scroll bar, or NULL if none
	 */
	inline ScrollBar* VertScrollBar() { return vertScroll; }

	/**
	 * Get the cursor position (index into the list)
	 *
	 * @return the cursor position (index)
	 */
	inline int Cursor(void) { return cursor; }

	/**
	 * Get the number of elements
	 *
	 * @return the number of elements
	 */
	virtual int Size(void) = 0;
};


/**
 * The list
 *
 * @author Peter Macko
 */
template <typename T, class Renderer = DefaultListItemRenderer<T>>
class List : public AbstractList
{
	std::vector<T> elements;
	Renderer renderer;


protected:

	/**
	 * Paint a list item
	 *
	 * @param tcw the output buffer
	 * @param index the item index
	 * @param active true if the item is active (under cursor)
	 * @param selected true if the item is selected
	 * @param highlightPattern the highlight pattern
	 */
	virtual void PaintListItem(TerminalControlWindow* tcw, int index,
			bool active, bool selected)
	{
		renderer.Paint(this, tcw, elements[index], active, selected, "");
	}
	

public:
	
	/**
	 * Create an instance of class List
	 *
	 * @param parent the parent container
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param anchor set the anchor
	 */
	List(Container* parent, int row = 0, int col = 0,
			int rows = 10, int cols = 16,
			int anchor = ANCHOR_LEFT | ANCHOR_TOP)
		: AbstractList(parent, row, col, rows, cols, anchor)
	{
		// For testing...

		for (int i = 0; i < 20; i++) {
			std::string s = "Test "; s += 'A' + i;
			elements.push_back((T) s);
		}

		SetScrollBar(VertScrollBar());	// XXX Hack
	}

	/**
	 * Destroy the object
	 */
	virtual ~List(void)
	{
		// TODO How to delete the contents?
	}

	/**
	 * Get the number of elements
	 *
	 * @return the number of elements
	 */
	virtual int Size(void) { return elements.size(); }
};

#endif

