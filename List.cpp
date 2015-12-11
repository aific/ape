/*
 * List.cpp
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


#include "stdafx.h"
#include "List.h"

#include "Container.h"


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
		const char* highlightPattern)
{
	tcw->PutText(item);
}


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
AbstractList::AbstractList(Container* parent, bool _sorted,
		int _row, int _col, int rows, int cols, int anchor)
	: Component(parent, true, _row, _col, rows, cols, anchor)
{
	sorted = _sorted;


	// Configure and initialize the list

	bg = 6;
	fg = 0;
	cursorBg = 0;
	cursorFg = 6;
	selBg = 4;
	selFg = 7;
	
	
	// Set the component properties
	
	SetMinSize(2, 10);
	
	
	// Set the cursor position

	cursor = 0;
	pageStart = 0;

	selection = false;
	selStart = 0;
	
	
	// Initialize the scroll bar

	scrollBarsReflectCursor = false;
	
	internalVertScroll = new ScrollBar(this, false);
	internalVertScroll->SetLocation(0, Columns()-1);
	internalVertScroll->SetLength(Rows());
	
	vertScroll = internalVertScroll;
	vertScroll->SetRange(0, 0);
}


/**
 * Destroy the object
 */
AbstractList::~AbstractList(void)
{
	if (internalVertScroll != NULL) delete internalVertScroll;
}


/**
 * Set the scroll bar
 *
 * @param vert the vertical scroll bar
 */
void AbstractList::SetScrollBar(ScrollBar* vert)
{
	vertScroll = vert;

	int m = Size();
	if (m > 0) m--;

	if (vertScroll != NULL) {
		vertScroll->SetRange(0, m);
	}

	UpdateScrollBarPosition();
}


/**
 * Update the scroll bar position
 */
void AbstractList::UpdateScrollBarPosition(void)
{
	if (scrollBarsReflectCursor) {
		if (vertScroll != NULL) {
			vertScroll->SetPosition(cursor);
		}
	}
	else {
		if (vertScroll != NULL) {
			vertScroll->SetPosition(pageStart, Rows());
		}
	}
}


/**
 * Paint an element
 * 
 * @param index the element index
 */
void AbstractList::PaintElement(int index)
{
	int ncols = Columns();
	
	assert(index >= pageStart && index < Size()
			&& index < pageStart + Rows());


	// Determine if we are in selection

	bool insel = false;
	if (selection) {
		if (cursor <= index && index <= selStart) insel = true;
		if (cursor >= index && index >= selStart) insel = true;
	}


	// Initialize the buffer for painting items and set the color

	bool hasScroll = internalVertScroll != NULL
	              && internalVertScroll == vertScroll;
	TerminalControlWindow w(1, ClientColumns() - (hasScroll ? 1 : 0));
	
	if (index == cursor) {
		w.SetColor(cursorBg, cursorFg);
	}
	else if (insel) {
		w.SetColor(selBg, selFg);
	}
	else {
		w.SetColor(bg, fg);
	}

	w.Clear();
	w.SetCursor(0, 0);


	// Paint

	PaintListItem(&w, index, index == cursor, insel);


	// Paste to the buffer
	
	tcw->OutBuffer(index - pageStart, 0, &w);
}


/**
 * Paint the contents of the window
 */
void AbstractList::Paint(void)
{
	Clear();
	

	// Compute the bounds
	
	int nElements = Rows();
	if (nElements + pageStart > Size()) {
		nElements = Size() - pageStart;
	}
	
	
	// Paint the elements
	
	for (int i = 0; i < nElements; i++) {
		PaintElement(i + pageStart);
	}
	
	
	// Paint the scroll bar
	
	if (internalVertScroll != NULL && internalVertScroll == vertScroll) {
		internalVertScroll->Paint();
	}
}


/**
 * Move the cursor up
 * 
 * @param shift whether the Shift button was held
 */
void AbstractList::MoveCursorUp(bool shift)
{
	if (cursor <= 0) return;
	
	EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selStart = cursor;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
	}
	
	
	// Cursor movement and scrolling logic
	
	if (pageStart >= cursor) {
		cursor--;
		pageStart = cursor;
	}
	else if (cursor - pageStart >= Rows()) {
		cursor--;
		pageStart = cursor - Rows() + 1;
	}
	else {
		cursor--;
	}
	
	CursorMoved();
	Paint();
}


/**
 * Move the cursor down
 * 
 * @param shift whether the Shift button was held
 */
void AbstractList::MoveCursorDown(bool shift)
{
	int numElements = Size();
	if (cursor >= numElements - 1) return;
	
	EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selStart = cursor;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
	}
	
	
	// Cursor movement and scrolling logic
	
	if (cursor - pageStart >= Rows() - 1) {
		cursor++;
		pageStart = cursor - Rows() + 1;
	}
	else if (pageStart >= cursor + 1) {
		cursor++;
		pageStart = cursor;
	}
	else {
		cursor++;
	}
	
	CursorMoved();
	Paint();
}


/**
 * Move the cursor one page up
 */
void AbstractList::MoveCursorPageUp(void)
{
	EnsureValidScroll();
	
	selection = false;
	
	
	// Set the new page start
	
	int p = pageStart;
	int d = Rows();

	if (p == 0 && cursor == 0) {
		return;
	}
	
	if (p - d < 0) d = p;
	pageStart = p - d;
	
	
	// Move the cursor
	
	if (d < Rows()) {
		cursor -= Rows() - 1;
		if (pageStart == 0) cursor = 0;
		if (cursor < 0) cursor = 0;
	}
	else {
		cursor -= d;
	}
	
	
	// Finish
	
	CursorMoved();
	Paint();
}


/**
 * Move the cursor one page down
 */
void AbstractList::MoveCursorPageDown(void)
{
	EnsureValidScroll();
	
	selection = false;
	
	
	// Set the new page start
	
	int p = pageStart;
	int d = Rows();
	
	int m = Size() - Rows();
	if (p + d > m) d = m - p;

	if (d == 0 && cursor == Size() - 1) {
		return;
	}
	
	pageStart = p + d;
	if (pageStart < 0) pageStart = 0;
	
	
	// Move the cursor
	
	if (d < Rows()) {
		cursor += Rows() - 2;
		if (d == 0) cursor++;
		if (pageStart + Rows() == Size()) {
			cursor = Size() - 1;
		}
		if (cursor > Size() - 1) {
			cursor = Size() - 1;
		}
	}
	else {
		cursor += d;
	}
	
	
	// Finish
	
	CursorMoved();
	Paint();
}


/**
 * Move the cursor to the beginning
 */
void AbstractList::MoveCursorToBeginning(void)
{
	EnsureValidScroll();
	
	selection = false;
	
	
	// Set the new page start
	
	pageStart = 0;
	
	
	// Move the cursor
	
	cursor = 0;
	
	
	// Finish
	
	CursorMoved();
	Paint();
}


/**
 * Move the cursor to the end
 */
void AbstractList::MoveCursorToEnd(void)
{
	EnsureValidScroll();
	
	selection = false;
	
	
	// Set the new page start
	
	pageStart = Size() - Rows();
	if (pageStart < 0) pageStart = 0;
	
	
	// Move the cursor
	
	cursor = Size() - 1;
	
	
	// Finish
	
	CursorMoved();
	Paint();
}


/**
 * Scroll the window such that the cursor is visible
 * 
 * @return true if the scrolling actually happened
 */
bool AbstractList::EnsureValidScroll(void)
{
	if (cursor - pageStart >= Rows() - 1) {
		pageStart = cursor - Rows() + 1;
		return true;
	}
	
	if (pageStart >= cursor + 1) {
		pageStart = cursor;
		return true;
	}

	if (pageStart > 0 && Size() - pageStart < Rows()) {
		int p = Size() - Rows();
		if (p < 0) p = 0;
		pageStart = p;
		return true;
	}
	
	return false;
}


/**
 * Perform any necessary actions after moving a cursor
 */
void AbstractList::CursorMoved(void)
{
	UpdateScrollBarPosition();
}


/**
 * Perform any necessary actions after adding an element
 *
 * @param index the index of the new element
 */
void AbstractList::ElementAdded(int index)
{
	// Update the cursor position

	bool cursorMoved = false;
	if (index <= cursor && Size() > 1) {
		cursor++;
		cursorMoved = true;
	}


	// Update the scroll bar and do other related things

	if (vertScroll != NULL) {
		vertScroll->SetRange(0, Size());
	}

	EnsureValidScroll();

	if (cursorMoved) {
		CursorMoved();
	}
	else {
		UpdateScrollBarPosition();
	}


	// Finish

	Paint();
}


/**
 * An event handler for pressing a key
 * 
 * @param key the key code
 */
void AbstractList::OnKeyPressed(int key)
{
	if (key >= 0x20 && key <= 0x7E) {
		// TODO Search
		return;
	}

	if (key == KEY_ENTER || key == KEY_RETURN) {
		FireOnAction();
		return;
	}


	// Cursor movement

	if (key == KEY_UP) {
		MoveCursorUp();
		return;
	}

	if (key == KEY_DOWN) {
		MoveCursorDown();
		return;
	}
	
	if (key == KEY_PPAGE) {
		MoveCursorPageUp();
		return;
	}

	if (key == KEY_NPAGE) {
		MoveCursorPageDown();
		return;
	}
	
	if (key == KEY_HOME) {
		MoveCursorToBeginning();
		return;
	}

	if (key == KEY_END) {
		MoveCursorToEnd();
		return;
	}
	
	
	// Cursor movement with selection
	
	if (key == KEY_SHIFT_UP) {
		MoveCursorUp(true);
		return;
	}

	if (key == KEY_SHIFT_DOWN) {
		MoveCursorDown(true);
		return;
	}


	// The default action
	
	Component::OnKeyPressed(key);
}


/**
 * An event handler for resizing the window
 *
 * @param oldRows the old number of rows
 * @param oldCols the old number of columns
 * @param newRows the new number of rows
 * @param newCols the new number of columns
 */
void AbstractList::OnResize(int oldRows, int oldCols, int newRows, int newCols)
{
	if (internalVertScroll != NULL) {
		internalVertScroll->SetLocation(0, Columns());
		internalVertScroll->SetLength(Rows());
	}

	Component::OnResize(oldRows, oldCols, newRows, newCols);
	EnsureValidScroll();
}

