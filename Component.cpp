/*
 * Component.cpp
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
#include "Component.h"

#include "Container.h"
#include "Manager.h"


/**
 * Create an instance of class Component
 *
 * @param _parent the parent container
 * @param _focusable true if the component can receive focus
 * @param _row the initial row
 * @param _col the initial column
 * @param _rows the number of rows
 * @param _cols the number of columns
 * @param _anchor set the anchor
 */
Component::Component(Container* _parent, bool _focusable, int _row, int _col,
		int _rows, int _cols, int _anchor)
{
	assert(_rows > 0);
	assert(_cols > 0);

	parent = _parent;
	canReceiveFocus = _focusable;
	canHandleMultiClicks = false;

	row = _row;
	col = _col;
	
	rows = _rows;
	cols = _cols;
	
	anchor = _anchor;
	
	
	// Set the default parameters
	
	minRows = 1;
	minCols = 1;
	
	cursVisible = false;
	cursRow = 0;
	cursCol = 0;
	
	visible = true;
	
	if (_parent != NULL) {
		bg = _parent->BGColor();
		fg = _parent->FGColor();
	}
	else {
		fg = 7;
		bg = 0;
	}


	// Create the window buffer
	
	screenRow = parent == NULL
		? row : parent->ScreenRow() + parent->ClientRow() + row;
	screenCol = parent == NULL
		? col : parent->ScreenColumn() + parent->ClientColumn() + col;

	tcw = new TerminalControlWindow(rows, cols);
	
	
	// Add the component
	
	if (parent != NULL) parent->Add(this);
}


/**
 * Destroy the object
 */
Component::~Component(void)
{
	delete tcw;
}


/**
 * Return the parent window, or this if this is the window
 *
 * @return the parent window
 */
Window* Component::ParentWindow(void)
{
	return parent == NULL ? NULL : parent->ParentWindow();
}


/**
 * Return whether the component is active
 *
 * @return true if the component is active
 */
bool Component::Active(void)
{
	if (!ParentWindow()->Active()) return false;
	if (!Parent()->Active()) return false;
	return Parent()->ActiveComponent() == this;
}


/**
 * Set the component visibility
 *
 * @param _visible true for the component to be be visible
 */
void Component::SetVisible(bool _visible)
{
	if (visible == _visible) return;

	visible = _visible;

	if (Visible()) {
		Paint();
	}
	else {
		ParentWindow()->Refresh();
		ParentWindow()->Paint();
	}

	wm.Refresh();
}


/**
 * Determine if the component is set to be visible
 *
 * @return true if the component is visible
 */
bool Component::Visible(void) {
	return visible
		&& (parent == NULL ? true : ((Component*) parent)->Visible());
}


/**
 * Determine whether the given coordinate is in the component
 *
 * @param row the row
 * @param column the column
 * @return true if it contains
 */
bool Component::Contains(int row, int column)
{
	return row >= Component::row && row < Component::row + rows
	    && column >= Component::col && column < Component::col + cols;
}


/**
 * Paint the component
 */
void Component::Paint(void)
{
	Clear();
}


/**
 * Set the background color
 *
 * @param color the new background color
 */
void Component::SetBGColor(int color)
{
	bg = color;
}


/**
 * Set the foreground color
 *
 * @param color the new foreground color
 */
void Component::SetFGColor(int color)
{
	fg = color;
}


/**
 * Clear the contents of the component
 */
void Component::Clear(void)
{
	tcw->SetColor(bg, fg);
	tcw->Clear();
}


/**
 * Set the minimum size of the component
 *
 * @param rows the number of rows
 * @param cols the number of columns
 */
void Component::SetMinSize(int rows, int cols)
{
	minRows = rows;
	minCols = cols;

	if (minRows < 1) minRows = 1;
	if (minCols < 1) minCols = 1;

	if (parent != NULL) parent->ChildMinSizeChanged(this, minRows, minCols);
}


/**
 * Refresh the component
 */
void Component::Refresh(void)
{
	if (Active()) {
		wm.UpdateCursor();
	}
	else {
		wm.Refresh();
	}
}


/**
 * Show and move the cursor
 *
 * @param r the row
 * @param c the column
 */
void Component::MoveCursor(int r, int c)
{
	if (r < 0) r = 0;
	if (c < 0) c = 0;
	if (r > rows) r = rows;
	if (c > cols) c = cols;

	cursRow = r;
	cursCol = c;
	cursVisible = true;

	if (Active()) {
		move(ScreenRow() + ClientRow() + r, ScreenColumn() + ClientColumn() + c);
		curs_set(TRUE);
	}
}


/**
 * Hide the cursor
 */
void Component::HideCursor(void)
{
	cursVisible = false;

	if (Active()) {
		curs_set(FALSE);
	}
}


/**
 * A notification handler for gaining focus
 */
void Component::NotifyActive(void)
{
}


/**
 * A notification handler for losing focus
 */
void Component::NotifyInactive(void)
{
}


/**
 * Resize the component (this also calls Paint)
 *
 * @param _rows the number of rows
 * @param _cols the number of columns
 */
void Component::Resize(int _rows, int _cols)
{
	int prevRows = rows;
	int prevCols = cols;

	rows = _rows;
	cols = _cols;

	if (row > wm.Rows() - rows - 1) rows = wm.Rows() - row - 1;
	if (col > wm.Columns() - cols) cols = wm.Columns() - col;
	
	if (rows < minRows) rows = minRows;
	if (cols < minCols) cols = minCols;

	tcw->Resize(rows, cols);

	OnResize(prevRows, prevCols, rows, cols);
	if (Visible()) Paint();
}
	

/**
 * A handler for when the container was moved
 */
void Component::ContainerMoved(void)
{
	screenRow = parent == NULL
		? row : parent->ScreenRow() + parent->ClientRow() + row;
	screenCol = parent == NULL
		? col : parent->ScreenColumn() + parent->ClientColumn() + col;
}


/**
 * Move the component
 *
 * @param _row the new row
 * @param _col the new column
 */
void Component::Move(int _row, int _col)
{
	row = _row;
	col = _col;

	if (row < 0) row = 0;
	if (col < 0) col = 0;
	
	if (parent == NULL) {
		if (row > wm.Rows() - rows - 1) row = wm.Rows() - rows - 1;
		if (col > wm.Columns() - cols) col = wm.Columns() - cols;
	}
	else {
		if (row > parent->ClientRows()    - rows)
			row = parent->ClientRows()    - rows;
		if (col > parent->ClientColumns() - cols)
			col = parent->ClientColumns() - cols;
	}

	if (row < 0) row = 0;
	if (col < 0) col = 0;
	
	screenRow = parent == NULL
		? row : parent->ScreenRow() + parent->ClientRow() + row;
	screenCol = parent == NULL
		? col : parent->ScreenColumn() + parent->ClientColumn() + col;

	OnMove();
}


/**
 * Resize and move the component (this also calls Paint)
 *
 * @param _row the new row
 * @param _col the new column
 * @param _rows the number of rows
 * @param _cols the number of columns
 */
void Component::SetBounds(int _row, int _col, int _rows, int _cols)
{
	// Set the bounds

	Resize(_rows, _cols);
	Move(_row, _col);
}


/**
 * An event handler for pressing a key
 *
 * @param key the key code
 */
void Component::OnKeyPressed(int key)
{
	// Focus
	
	if (key == '\t') {
		FocusNext();
		return;
	}
	
	if (key == KEY_BTAB) {
		FocusPrevious();
		return;
	}
}


/**
 * An event handler for mouse double-click
 *
 * @param row the row
 * @param column the column
 * @param button the button
 * @param shift whether shift was pressed
 */
void Component::OnMouseDoubleClick(int row, int column, int button, bool shift)
{
	if (!canHandleMultiClicks) {
		OnMouseClick(row, column, button, shift);
	}
}


/**
 * An event handler for mouse triple-click and beyond
 *
 * @param row the row
 * @param column the column
 * @param button the button
 * @param count the number of clicks
 * @param shift whether shift was pressed
 */
void Component::OnMouseMultipleClick(int row, int column, int button, int count, bool shift)
{
	if (!canHandleMultiClicks) {
		OnMouseClick(row, column, button, shift);
	}
}


/**
 * An event handler for moving the component
 */
void Component::OnMove(void)
{
}


/**
 * An event handler for resizing the component
 *
 * @param oldRows the old number of rows
 * @param oldCols the old number of columns
 * @param newRows the new number of rows
 * @param newCols the new number of columns
 */
void Component::OnResize(int oldRows, int oldCols, int newRows, int newCols)
{
}


/**
 * An event handler for raising the component
 */
void Component::OnRaise(void)
{
}


/**
 * An event handler for a time step
 */
void Component::OnStep(void)
{
}


/**
 * Fire the OnAction event
 */
void Component::FireOnAction(void)
{
	for (size_t i = 0; i < eventHandlers.size(); i++) {
		if (eventHandlers[i] == NULL) continue;
		eventHandlers[i]->OnAction(this);
	}
}


/**
 * Fire the OnValueChanged event
 */
void Component::FireOnValueChanged(void)
{
	for (size_t i = 0; i < eventHandlers.size(); i++) {
		if (eventHandlers[i] == NULL) continue;
		eventHandlers[i]->OnValueChanged(this);
	}
}


/**
 * Bring focus to this component
 */
void Component::Focus(void)
{
	// If the component cannot receive focus, or if it already has focus, then
	// do nothing
	
	if (!canReceiveFocus) return;
	if (Active()) return;


	// Activate the correct window
	
	if (wm.Top() != ParentWindow()) {
		wm.Raise(ParentWindow());
	}


	// Find the common parent with the currently active component

	Component* current = ParentWindow()->ActiveTopLevelComponent();
	Container* ancestor = current == NULL ? NULL : current->Parent();

	for (bool found = false; ancestor != NULL; ancestor = ancestor->Parent()) {
		for (Container* c = Parent(); c != NULL; c = c->Parent()) {
			if (c == ancestor) {
				found = true;
				break;
			}
		}
		if (found) break;
	}


	// Set the correct active component indices
	
	Component* last = this;
	Container* stopAt = ancestor == NULL ? NULL : ancestor->Parent();

	for (Container* c = last->Parent();
			c != NULL && c != stopAt;
			last = c, c = c->Parent()) {

		ssize_t i = c->ComponentIndex(last);
		assert(i >= 0);
		c->activeComponent = i;
	}


	// Notify inactive components
	
	if (current != NULL) {
		current->NotifyInactive();
		for (Container* c = current->Parent();
				c != NULL && c != ancestor;
				c = c->Parent()) {
			c->NotifyInactive();
		}
	}


	// Notify active components
	
	Component* c = ancestor == NULL ? ParentWindow() : ancestor->ActiveComponent();

	while (true) {
		c->NotifyActive();

		if (c->InstanceOfContainer())
			c = ((Container*) c)->ActiveComponent();
		else
			break;
	}


	// Paint
	
	if (ancestor != NULL) {
		ancestor->Paint();
		ancestor->Refresh();
	}
	else {
		ParentWindow()->Paint();
		ParentWindow()->Refresh();
	}
}


/**
 * Focus the next component
 */
void Component::FocusNext(void)
{
	// This component must have focus for this method to make sense
	
	if (!canReceiveFocus || !Active()) return;


	// Pass this up to the container
	
	if (Parent() != NULL) Parent()->FocusNext();
}


/**
 * Focus the previous component
 */
void Component::FocusPrevious(void)
{
	// This component must have focus for this method to make sense
	
	if (!canReceiveFocus || !Active()) return;


	// Pass this up to the container
	
	if (Parent() != NULL) Parent()->FocusPrevious();
}


/**
 * Register an event handler
 *
 * @param handler the handler
 */
void Component::RegisterEventHandler(EventHandler* handler)
{
	assert(handler != NULL);
	eventHandlers.push_back(handler);
}


