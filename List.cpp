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
 * Create an instance of class AbstractList
 *
 * @param parent the parent container
 * @param row the initial row
 * @param col the initial column
 * @param rows the number of rows
 * @param cols the number of columns
 * @param anchor set the anchor
 */
AbstractList::AbstractList(Container* parent, int _row, int _col, int rows,
		int cols, int anchor)
	: Component(parent, true, _row, _col, rows, cols, anchor)
{
	// Configure and initialize the list

	bg = 6;
	fg = 0;
	cursorBg = 0;
	cursorFg = 6;
	selBg = 4;
	selFg = 0;

	scrollBarsReflectCursor = false;
	vertScroll = NULL;
	
	
	// Set the component properties
	
	SetMinSize(2, 10);
	
	
	// Set the cursor position

	cursor = 0;
	pageStart = 0;

	selection = false;
	selStart = 0;


	// For testing...
	
	for (int i = 0; i < 20; i++) {
		std::string s = "Test "; s += 'A' + i;
		elements.push_back(s);
	}
}


/**
 * Destroy the object
 */
AbstractList::~AbstractList(void)
{
}


/**
 * Set the scroll bar
 *
 * @param vert the vertical scroll bar
 */
void AbstractList::SetScrollBar(ScrollBar* vert)
{
	vertScroll = vert;

	int m = elements.size();
	if (m > 0) m--;

	if (vertScroll != NULL) {
		vertScroll->SetRange(0, m);
	}
	
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
	
	assert(index >= pageStart && index < (int) elements.size()
			&& index < pageStart + Rows());

	const char* str = elements[index].c_str();
	size_t strLength = strlen(str);

	tcw->SetCursor(index - pageStart, 0);


	// Determine if we are in selection

	bool insel = false;
	if (selection) {
		if (cursor <= index && index <= selStart) insel = true;
		if (cursor >= index && index >= selStart) insel = true;
	}


	// Set the color
	
	if (index == cursor) {
		tcw->SetColor(cursorBg, cursorFg);
		tcw->OutHorizontalLine(index - pageStart, 0, ClientColumns(), ' ');
	}
	else if (insel) {
		tcw->SetColor(selBg, selFg);
		tcw->OutHorizontalLine(index - pageStart, 0, ClientColumns(), ' ');
	}
	else {
		tcw->SetColor(bg, fg);
	}


	// TODO

	tcw->PutText(str);
}


/**
 * Paint the contents of the window
 */
void AbstractList::Paint(void)
{
	Clear();
	

	// Compute the bounds
	
	int nElements = Rows();
	if (nElements + pageStart > (int) elements.size()) {
		nElements = ((int) elements.size()) - pageStart;
	}
	
	
	// Paint the elements
	
	for (int i = 0; i < nElements; i++) {
		PaintElement(i + pageStart);
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
	
	Paint();
}


/**
 * Move the cursor down
 * 
 * @param shift whether the Shift button was held
 */
void AbstractList::MoveCursorDown(bool shift)
{
	int numElements = elements.size();
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
	
	Paint();
}


/**
 * Move the cursor one page up
 */
void AbstractList::MoveCursorPageUp(void)
{
	EnsureValidScroll();
	
	
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
		if (cursor < 0) cursor = 0;
	}
	else {
		cursor -= d;
	}
	
	
	// Finish
	
	Paint();
}


/**
 * Move the cursor one page down
 */
void AbstractList::MoveCursorPageDown(void)
{
	EnsureValidScroll();
	
	
	// Set the new page start
	
	int p = pageStart;
	int d = Rows();
	
	int m = ((int) elements.size()) - Rows();
	if (p + d > m) d = m - p;

	if (d == 0 && cursor == ((int) elements.size()) - 1) {
		return;
	}
	
	pageStart = p + d;
	if (pageStart < 0) pageStart = 0;
	
	
	// Move the cursor
	
	if (d < Rows()) {
		cursor += Rows() - 2;
		if (d == 0) cursor++;
		if (cursor > ((int) elements.size()) - 1) {
			cursor = ((int) elements.size()) - 1;
		}
	}
	else {
		cursor += d;
	}
	
	
	// Finish
	
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

	if (pageStart > 0 && ((int) elements.size()) - pageStart < Rows()) {
		int p = ((int) elements.size()) - Rows();
		if (p < 0) p = 0;
		pageStart = p;
	}
	
	return false;
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
	Component::OnResize(oldRows, oldCols, newRows, newCols);
	EnsureValidScroll();
}
