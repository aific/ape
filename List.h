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

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "ScrollBar.h"
#include "util.h"


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
class DefaultListItemRenderer : public ListItemRenderer<T>
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

	bool sorted;
	

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
	

protected:
	
	/**
	 * Scroll the window such that the cursor is visible
	 * 
	 * @return true if the scrolling actually happened
	 */
	bool EnsureValidScroll(void);

	/**
	 * Update the scroll bar position
	 */
	void UpdateScrollBarPosition(void);

	/**
	 * Perform any necessary actions after moving a cursor
	 */
	void CursorMoved(void);
	
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
	 * An event handler for mouse double-click
	 *
	 * @param row the row
	 * @param column the column
	 * @param button the button
	 * @param shift whether shift was pressed
	 */
	virtual void OnMouseDoubleClick(int row, int column, int button, bool shift);
	
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

	/**
	 * Perform any necessary actions after adding an element
	 *
	 * @param index the index of the new element
	 */
	void ElementAdded(int index);
	

public:
	
	/**
	 * Create an instance of class AbstractList
	 *
	 * @param parent the parent container
	 * @param sorted true if the elements need to be always sorted
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param anchor set the anchor
	 */
	AbstractList(Container* parent, bool sorted = false,
			int row = 0, int col = 0,
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
	 * Set the cursor position
	 *
	 * @param cursor the new cursor position
	 */
	void SetCursor(int cursor);

	/**
	 * Get the number of elements
	 *
	 * @return the number of elements
	 */
	virtual int Size(void) = 0;

	/**
	 * Determine if the list is defined as sorted
	 *
	 * @return true if the list is defined as sorted
	 */
	inline bool Sorted(void) { return sorted; }
};


/**
 * The list
 *
 * @author Peter Macko
 */
template <typename T>
class List : public AbstractList
{
	std::vector<T> elements;
	ListItemRenderer<T>* renderer;
	bool ownRenderer;


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
		renderer->Paint(this, tcw, elements[index], active, selected, "");
	}
	

public:
	
	/**
	 * Create an instance of class List
	 *
	 * @param parent the parent container
	 * @param sorted true if the elements need to be always sorted
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param anchor set the anchor
	 */
	List(Container* parent, bool sorted = false, int row = 0, int col = 0,
			int rows = 10, int cols = 16,
			int anchor = ANCHOR_LEFT | ANCHOR_TOP)
		: AbstractList(parent, sorted, row, col, rows, cols, anchor)
	{
		renderer = new DefaultListItemRenderer<T>();
		ownRenderer = true;
	}

	/**
	 * Destroy the object
	 */
	virtual ~List(void)
	{
		if (renderer && ownRenderer) delete renderer;

		// TODO How to delete the contents?
	}

	/**
	 * Set the list item renderer
	 *
	 * @param renderer the renderer
	 * @param autoDestroy true to destroy the object together with this list
	 */
	void SetRenderer(ListItemRenderer<T>* renderer, bool autoDestroy=false)
	{
		this->renderer = renderer;
		ownRenderer = autoDestroy;
	}

	/**
	 * Get the number of elements
	 *
	 * @return the number of elements
	 */
	virtual int Size(void) { return elements.size(); }

	/**
	 * Add an element to the list
	 *
	 * @param element the element
	 */
	void Add(const T& element)
	{
		if (Sorted()) {
			// Perhaps we should do this in O(log n) instead of O(n)...
			size_t i = 0;
			for ( ; i < elements.size(); i++) {
				if (!(element > elements[i])) break;
			}
			elements.insert(elements.begin() + i, element);
			ElementAdded(i);
		}
		else {
			elements.push_back(element);
			ElementAdded(elements.size() - 1);
		}
	}

	/**
	 * Get the element by index
	 *
	 * @param index the index
	 * @return the reference to the element
	 */
	inline T& operator[] (int index)
	{
		return elements[index];
	}

	/**
	 * Get the element by index
	 *
	 * @param index the index
	 * @return the reference to the element
	 */
	inline T& Item(int index)
	{
		return elements[index];
	}

	/**
	 * Set the contents
	 *
	 * @param contents the new contents
	 */
	void SetContents(std::vector<T>& contents)
	{
		elements = contents;
		if (Sorted()) {
			std::stable_sort(elements.begin(), elements.end());
		}

		if (VertScrollBar() != NULL) {
			VertScrollBar()->SetRange(0, Size());
		}

		SetCursor(0);
		UpdateScrollBarPosition();
	}

	/**
	 * Find an item by value
	 *
	 * @param item the item
	 * @return the index, or -1 if not found
	 */
	int Find(const T& item)
	{
		for (int i = 0; i < Size(); i++) {
			if (Item(i) == item) return i;
		}
		return -1;
	}
};

#endif

