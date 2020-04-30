/*
 * Container.cpp
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
#include "Container.h"

#include "Manager.h"
#include "ScrollBar.h"


/**
 * Create an instance of class Container
 *
 * @param _parent the parent container
 * @param _row the initial row
 * @param _col the initial column
 * @param _rows the number of rows
 * @param _cols the number of columns
 * @param _anchor set the anchor
 */
Container::Container(Container* _parent, int _row, int _col,
		int _rows, int _cols, int _anchor)
	: Component(_parent, false, _row, _col, _rows, _cols, _anchor)
{
	activeComponent = 0;
	canHandleMultiClicks = true;


	// Set the defaults
	
	hasFrame = false;
	capturesFocus = false;
	
	horizScrollStart = 3;
	vertScrollStart = 3;
	horizScrollEnd = 3;
	vertScrollEnd = 3;
	horizScroll = NULL;
	vertScroll = NULL;
}


/**
 * Destroy the object
 */
Container::~Container(void)
{
	if (horizScroll != NULL) delete horizScroll;
	if (vertScroll  != NULL) delete vertScroll;

	for (unsigned u = 0; u < components.size(); u++) {
		delete components[u];
	}
}


/**
 * Add a component (called from the inside of Component's constructor)
 *
 * @param component the component to add
 */
void Container::Add(Component* component)
{
	if (!Visible()) component->SetVisible(false);

	components.push_back(component);

	if (component->CanReceiveFocus() && !canReceiveFocus) {
		activeComponent = components.size() - 1;

		for (Container* c = this; c != NULL; c = c->Parent()) {
			if (c->canReceiveFocus) break;
			
			c->canReceiveFocus = true;

			Container* p = c->Parent();
			if (p != NULL && !p->canReceiveFocus) {
				ssize_t i = p->ComponentIndex(c);
				assert(i >= 0);
				p->activeComponent = i;
			}
		}
	}
}


/**
 * Paint the frame
 */
void Container::PaintFrame(void)
{
	tcw->SetColor(bg, fg);
	tcw->OutBox(0, 0, rows, cols, false);
}


/**
 * Clear the contents of the component
 */
void Container::Clear(void)
{
	Component::Clear();

	if (hasFrame) PaintFrame();

	if (horizScroll != NULL) horizScroll->Paint();
	if (vertScroll  != NULL) vertScroll ->Paint();

	tcw->SetColor(bg, fg);
}


/**
 * Paint the contents of the container
 */
void Container::PaintContents(void)
{
}


/**
 * Paint the component
 */
void Container::Paint(void)
{
	if (!Visible()) return;

	Clear();
	PaintContents();

	for (unsigned u = 0; u < components.size(); u++) {
		if (!components[u]->Visible()) continue;

		tcw->SetColor(bg, fg);
		components[u]->Paint();

		tcw->OutBuffer(ClientRow() + components[u]->Row(),
				ClientColumn() + components[u]->Column(),
				components[u]->TcwBuffer());
	}
}


/**
 * Set the component visibility
 *
 * @param _visible true for the component to be be visible
 */
void Container::SetVisible(bool _visible)
{
	if (visible == _visible) return;

	if (_visible) Component::SetVisible(_visible);

	for (unsigned u = 0; u < components.size(); u++) {
		components[u]->SetVisible(_visible);
	}

	if (!_visible) Component::SetVisible(_visible);
}


/**
 * Add a scroll bar
 *
 * @param horiz true if the scrollbar should be horizontal
 * @param start the leadway space to the left/top of the scrollbar
 * @param end the empty space behinf the scrollbar
 */
void Container::AddScrollBar(bool horiz, int start, int end)
{
	if (horiz) {

		horizScrollStart = start;
		horizScrollEnd = end;

		if (horizScroll == NULL) horizScroll = new ScrollBar(this, horiz);

		horizScroll->SetLocation(Rows() - 1, horizScrollStart);
		horizScroll->SetLength(Columns() - horizScrollStart - horizScrollEnd);
	}
	else {

		vertScrollStart = start;
		vertScrollEnd = end;

		if (vertScroll == NULL) vertScroll = new ScrollBar(this, horiz);

		vertScroll->SetLocation(vertScrollStart, Columns() - 1);
		vertScroll->SetLength(Rows() - vertScrollStart - vertScrollEnd);
	}
}


/**
 * A handler for when the container was moved
 */
void Container::ContainerMoved(void)
{
	Component::ContainerMoved();
	for (unsigned u = 0; u < components.size(); u++) {
		components[u]->ContainerMoved();
	}
}


/**
 * A handler for when a child component's minimum size was set
 *
 * @param child the child component
 * @param newRows the new number of rows
 * @param newCols the new number of columns
 */
void Container::ChildMinSizeChanged(Component* child, int newRows, int newCols)
{
	int newMinRows = minRows;
	int newMinCols = minCols;
	int f = hasFrame ? 2 : 0;

	if (newRows + child->Row() + f > minRows) {
		newMinRows = newRows + child->Row() + f;
	}

	if (newCols + child->Column() + f > minCols) {
		newMinCols = newCols + child->Column() + f;
	}

	if (newMinRows != minRows && newMinCols != minCols) {
		SetMinSize(newMinRows, newMinCols);
	}
}


/**
 * An event handler for moving the component
 */
void Container::OnMove(void)
{
	for (unsigned u = 0; u < components.size(); u++) {
		components[u]->ContainerMoved();
	}
}


/**
 * An event handler for resizing the component
 *
 * @param oldRows the old number of rows
 * @param oldCols the old number of columns
 * @param newRows the new number of rows
 * @param newCols the new number of columns
 */
void Container::OnResize(int oldRows, int oldCols, int newRows, int newCols)
{
	int dh = newRows - oldRows;
	int dw = newCols - oldCols;

	if (horizScroll != NULL) {
		horizScroll->SetLocation(Rows() - 1, horizScrollStart);
		horizScroll->SetLength(Columns() - horizScrollStart - horizScrollEnd);
	}

	if (vertScroll != NULL) {
		vertScroll->SetLocation(vertScrollStart, Columns() - 1);
		vertScroll->SetLength(Rows() - vertScrollStart - vertScrollEnd);
	}
	
	
	// Resize components according to their anchors
	
	for (unsigned u = 0; u < components.size(); u++) {
		int r = components[u]->Row();
		int c = components[u]->Column();
		int h = components[u]->Rows();
		int w = components[u]->Columns();
		int anchor = components[u]->Anchor();

		int new_r = r;
		int new_h = h;
		if ((anchor & ANCHOR_BOTTOM) == ANCHOR_BOTTOM) {
			if ((anchor & ANCHOR_TOP) == ANCHOR_TOP) {
				new_h += dh;
			}
			else {
				new_r += dh;
			}
		}

		int new_c = c;
		int new_w = w;
		if ((anchor & ANCHOR_RIGHT) == ANCHOR_RIGHT) {
			if ((anchor & ANCHOR_LEFT) == ANCHOR_LEFT) {
				new_w += dw;
			}
			else {
				new_c += dw;
			}
		}

		if (r != new_r || c != new_c) {
			if (h != new_h || w != new_w) {
				if (new_h < h) components[u]->Resize(new_h, w);
				if (new_w < w) components[u]->Resize(h, new_w);
				components[u]->Move(new_r, new_c);
				if (new_h > h) components[u]->Resize(new_h, w);
				if (new_w > w) components[u]->Resize(h, new_w);
			}
			else {
				// Move only
				components[u]->Move(new_r, new_c);
			}
		}
		else if (h != new_h || w != new_w) {
			// Resize only
			components[u]->Resize(new_h, new_w);
		}
	}
}


/**
 * An event handler for a time step
 */
void Container::OnStep(void)
{
	for (unsigned u = 0; u < components.size(); u++) {
		components[u]->OnStep();
	}
}


/**
 * An event handler for pressing a key
 *
 * @param key the key code
 */
void Container::OnKeyPressed(int key)
{
	Component* c = ActiveComponent();
	if (c) c->OnKeyPressed(key);
}


/**
 * An event handler for mouse press
 *
 * @param row the row
 * @param column the column
 * @param button the button
 * @param shift whether shift was pressed
 */
void Container::OnMousePress(int row, int column, int button, bool shift)
{
	Component* component = ComponentAt(row, column);
	if (component == NULL) return;
	
	if (component->CanReceiveFocus()) {
		component->Focus();
	}
	
	component->OnMousePress(row - component->Row() - ClientRow(),
		column - component->Column() - ClientColumn(), button, shift);
}


/**
 * An event handler for mouse release
 *
 * @param row the row
 * @param column the column
 * @param button the button
 * @param shift whether shift was pressed
 */
void Container::OnMouseRelease(int row, int column, int button, bool shift)
{
	Component* component = ComponentAt(row, column);
	if (component == NULL) return;
	
	if (component->CanReceiveFocus()) {
		component->Focus();
	}
	
	component->OnMouseRelease(row - component->Row() - ClientRow(),
		column - component->Column() - ClientColumn(), button, shift);
}


/**
 * An event handler for mouse click
 *
 * @param row the row
 * @param column the column
 * @param button the button
 * @param shift whether shift was pressed
 */
void Container::OnMouseClick(int row, int column, int button, bool shift)
{
	Component* component = ComponentAt(row, column);
	if (component == NULL) return;
	
	if (component->CanReceiveFocus()) {
		component->Focus();
	}
	
	component->OnMouseClick(row - component->Row() - ClientRow(),
		column - component->Column() - ClientColumn(), button, shift);
}


/**
 * An event handler for mouse double-click
 *
 * @param row the row
 * @param column the column
 * @param button the button
 * @param shift whether shift was pressed
 */
void Container::OnMouseDoubleClick(int row, int column, int button, bool shift)
{
	Component* component = ComponentAt(row, column);
	if (component == NULL) return;
	
	if (component->CanReceiveFocus()) {
		component->Focus();
	}
	
	component->OnMouseDoubleClick(row - component->Row() - ClientRow(),
		column - component->Column() - ClientColumn(), button, shift);
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
void Container::OnMouseMultipleClick(int row, int column, int button, int count, bool shift)
{
	Component* component = ComponentAt(row, column);
	if (component == NULL) return;
	
	if (component->CanReceiveFocus()) {
		component->Focus();
	}
	
	component->OnMouseMultipleClick(row - component->Row() - ClientRow(),
		column - component->Column() - ClientColumn(), button, count, shift);
}


/**
 * An event handler for mouse drag
 *
 * @param row the row
 * @param column the column
 * @param button the button
 * @param shift whether shift was pressed
 */
void Container::OnMouseDrag(int row, int column, int button, bool shift)
{
	Component* component = ComponentAt(row, column);
	if (component == NULL) return;
	
	if (component->CanReceiveFocus()) {
		component->Focus();
	}
	
	component->OnMouseDrag(row - component->Row() - ClientRow(),
		column - component->Column() - ClientColumn(), button, shift);
}


/**
 * An event handler for finishing mouse drag
 *
 * @param row the row
 * @param column the column
 * @param button the button
 * @param shift whether shift was pressed
 */
void Container::OnMouseDragFinish(int row, int column, int button, bool shift)
{
	Component* component = ComponentAt(row, column);
	if (component == NULL) return;
	
	if (component->CanReceiveFocus()) {
		component->Focus();
	}
	
	component->OnMouseDragFinish(row - component->Row() - ClientRow(),
		column - component->Column() - ClientColumn(), button, shift);
}


/**
 * An event handler for mouse wheel
 *
 * @param row the row
 * @param column the column
 * @param wheel the wheel direction
 */
void Container::OnMouseWheel(int row, int column, int wheel)
{
	Component* component = ComponentAt(row, column);
	if (component == NULL) return;
	
	component->OnMouseWheel(row - component->Row() - ClientRow(),
		column - component->Column() - ClientColumn(), wheel);
}


/**
 * Get the desired cursor row
 *
 * @return the desired cursor row
 */
int Container::CursorRow(void)
{
	Component* c = ActiveComponent();
	if (c && c->CursorVisible())
		return c->Row() + c->ClientRow() + c->CursorRow();
	else
		return Component::CursorRow();
}


/**
 * Get the desired cursor column
 *
 * @return the desired cursor column
 */
int Container::CursorColumn(void)
{
	Component* c = ActiveComponent();
	if (c && c->CursorVisible())
		return c->Column() + c->ClientColumn() + c->CursorColumn();
	else
		return Component::CursorColumn();
}


/**
 * Get the cursor visibility
 *
 * @return true if the cursor should be visible
 */
bool Container::CursorVisible(void)
{
	Component* c = ActiveComponent();
	if (c && c->CursorVisible())
		return true;
	else
		return Component::CursorVisible();
}


/**
 * Find the component index
 *
 * @param component the component
 * @return the index, or -1 if not found
 */
ssize_t Container::ComponentIndex(Component* component)
{
	for (size_t i = 0; i < components.size(); i++) {
		if (components[i] == component) return i;
	}

	return -1;
}


/**
 * Get the active child component
 *
 * @return the active component, or NULL if none is active
 */
Component* Container::ActiveComponent(void)
{
	if (canReceiveFocus) {
		Component* c = components[activeComponent];
		if (c != NULL && c->Visible())
			return c;
		else
			return NULL;
	}
	else {
		return NULL;
	}
}


/**
 * Get the active top-level component
 *
 * @return the active top-level component, or NULL if none is active
 */
Component* Container::ActiveTopLevelComponent(void)
{
	Component* c = ActiveComponent();
	if (c == NULL) return NULL;

	if (c->InstanceOfContainer())
		return ((Container*) c)->ActiveTopLevelComponent();
	else
		return c;
}


/**
 * Return the component at the given coordinate
 *
 * @param row the row
 * @param column the column
 * @return the component, or NULL if none
 */
Component* Container::ComponentAt(int row, int column)
{
	for (int i = ((int) components.size()) - 1; i >= 0; i--) {
		if (!components[i]->Visible()) continue;
		if (!components[i]->Contains(row - ClientRow(), column - ClientColumn())) continue;
		return components[i];
	}
	return NULL;
}


/**
 * Focus the first component
 */
void Container::FocusFirst(void)
{
	for (size_t i = 0; i < components.size(); i++) {
		if (components[i]->CanReceiveFocus() && components[i]->Visible()) {
			if (components[i]->InstanceOfContainer())
				((Container*) components[i])->FocusFirst();
			else
				components[i]->Focus();
			break;
		}
	}
}


/**
 * Focus the last component
 */
void Container::FocusLast(void)
{
	for (ssize_t i = components.size() - 1; i >= 0; i--) {
		if (components[i]->CanReceiveFocus() && components[i]->Visible()) {
			if (components[i]->InstanceOfContainer())
				((Container*) components[i])->FocusLast();
			else
				components[i]->Focus();
			break;
		}
	}
}


/**
 * Focus the next component
 */
void Container::FocusNext(void)
{
	// This component must have focus for this method to make sense
	
	if (!canReceiveFocus || !Active() || components.empty()) return;


	// Attempt to find the next component that can receive focus
	
	for (size_t i = activeComponent + 1; i < components.size(); i++) {
		if (components[i]->CanReceiveFocus() && components[i]->Visible()) {
			if (components[i]->InstanceOfContainer())
				((Container*) components[i])->FocusFirst();
			else
				components[i]->Focus();
			return;
		}
	}


	// If that fails, go to the parent or back to the beginning if appropriate

	if (Parent() != NULL && !capturesFocus) {
		Parent()->FocusNext();
	}
	else {
		FocusFirst();
	}
}


/**
 * Focus the previous component
 */
void Container::FocusPrevious(void)
{
	// This component must have focus for this method to make sense
	
	if (!canReceiveFocus || !Active() || components.empty()) return;


	// Attempt to find the previous component that can receive focus
	
	for (ssize_t i = ((ssize_t) activeComponent) - 1; i >= 0; i--) {
		if (components[i]->CanReceiveFocus() && components[i]->Visible()) {
			if (components[i]->InstanceOfContainer())
				((Container*) components[i])->FocusLast();
			else
				components[i]->Focus();
			return;
		}
	}


	// If that fails, go to the parent or back to the beginning if appropriate

	if (Parent() != NULL && !capturesFocus) {
		Parent()->FocusPrevious();
	}
	else {
		FocusLast();
	}
}



