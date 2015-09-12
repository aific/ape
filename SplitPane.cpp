/*
 * SplitPane.cpp
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
#include "SplitPane.h"


/**
 * Create an instance of class SplitPane
 *
 * @param _parent the parent container
 * @param _type the type - horizontal or vertical
 * @param _row the initial row
 * @param _col the initial column
 * @param _rows the number of rows
 * @param _cols the number of columns
 * @param _anchor set the anchor
 */
SplitPane::SplitPane(Container* _parent, int _type, int _row, int _col,
		int _rows, int _cols, int _anchor)
	: Container(_parent, _row, _col, _rows, _cols, _anchor)
{
	assert(_type == SPLITPANE_HORIZONTAL || _type == SPLITPANE_VERTICAL);

	type = _type;

	split = Horizontal() ? _rows / 2 : _cols / 2;
	splitProportion = 0.5;

	mainComponent = SPLITPANE_COMPONENT_NONE;
	oneComponentMode = SPLITPANE_COMPONENT_NONE;
	
	first = NULL;
	second = NULL;
}


/**
 * Destroy the object
 */
SplitPane::~SplitPane(void)
{
}


/**
 * Set the main component
 *
 * @param component one of the SPLITPANE_COMPONENT_*
 */
void SplitPane::SetMainComponent(int component)
{
	assert(component == SPLITPANE_COMPONENT_NONE
			|| component == SPLITPANE_COMPONENT_FIRST
			|| component == SPLITPANE_COMPONENT_SECOND);

	mainComponent = component;
}


/**
 * Set the one component mode, which can be used to show only one
 * component; use SPLITPANE_COMPONENT_NONE to show both at the same time
 *
 * @param mode one of the SPLITPANE_COMPONENT_*
 */
void SplitPane::SetOneComponentMode(int mode)
{
	assert(mode == SPLITPANE_COMPONENT_NONE
			|| mode == SPLITPANE_COMPONENT_FIRST
			|| mode == SPLITPANE_COMPONENT_SECOND);

	if (mode == oneComponentMode) return;

	bool firstHadFocus = first != NULL ? first->Active() : false;
	bool secondHadFocus = second != NULL ? second->Active() : false;
	
	oneComponentMode = mode;

	if (first != NULL && oneComponentMode == SPLITPANE_COMPONENT_FIRST) {
		first->SetBounds(0, 0, ClientRows(), ClientColumns());
	}
	else if (second != NULL && oneComponentMode == SPLITPANE_COMPONENT_SECOND){
		second->SetBounds(0, 0, ClientRows(), ClientColumns());
	}
	else if (oneComponentMode == SPLITPANE_COMPONENT_NONE) {
		SetSplitInternal(split);
	}

	if (first != NULL)
		first->SetVisible(Visible() && oneComponentMode != SPLITPANE_COMPONENT_SECOND);
	if (second != NULL)
		second->SetVisible(Visible() && oneComponentMode != SPLITPANE_COMPONENT_FIRST);

	if (firstHadFocus && oneComponentMode == SPLITPANE_COMPONENT_SECOND)
		if (second != NULL) second->Focus();
	if (secondHadFocus && oneComponentMode == SPLITPANE_COMPONENT_FIRST)
		if (first != NULL) first->Focus();

	Paint();
	Refresh();
}


/**
 * Paint the contents of the window
 */
void SplitPane::Paint(void)
{
	if (first == NULL || second == NULL) Clear();

	if (oneComponentMode == SPLITPANE_COMPONENT_NONE) {

		if (first  != NULL) {
			first->Paint();
			tcw->OutBuffer(ClientRow() + first->Row(),
					ClientColumn() + first->Column(),
					first->TcwBuffer());
		}
		if (second != NULL) {
			second->Paint();
			tcw->OutBuffer(ClientRow() + second->Row(),
					ClientColumn() + second->Column(),
					second->TcwBuffer());
		}

		int bg = BGColor();
		int fg = FGColor();
		tcw->SetColor(bg, fg);

		if (Horizontal()) {
			tcw->OutHorizontalLine(ClientRow() + split, ClientColumn(),
					ClientColumns());
		}
		else {
			tcw->OutVerticalLine(ClientRow(), ClientColumn() + split,
					ClientRows());
		}
	}
	else {

		Component* c = oneComponentMode == SPLITPANE_COMPONENT_FIRST
			? first : second;
		if (c != NULL) {
			c->Paint();
			tcw->OutBuffer(ClientRow() + c->Row(),
					ClientColumn() + c->Column(),
					c->TcwBuffer());
		}
	}
}


/**
 * Set the split position, but do not call paint or update proportions
 *
 * @param newSplit the new split position
 */
void SplitPane::SetSplitInternal(int newSplit)
{
	if (newSplit < 0) {
		newSplit = (Horizontal() ? ClientRows() : ClientColumns()) + newSplit;
	}
	newSplit = std::min((Horizontal() ? ClientRows() : ClientColumns()) - 1,
			newSplit);
	if (newSplit < 1) newSplit = 1;

	if (oneComponentMode != SPLITPANE_COMPONENT_NONE) {
		split = newSplit;
		return;
	}

	if (Horizontal()) {
		if (first != NULL) {
			if (newSplit < first->MinRows())
				newSplit = first->MinRows();
			if (second != NULL) {
				if (ClientRows() - newSplit - 1 < second->MinRows())
					newSplit = ClientRows() - 1 - second->MinRows();
			}
			first->Resize(newSplit, ClientColumns());
		}
		if (second != NULL) {
			second->SetBounds(newSplit + 1, 0,
					ClientRows() - newSplit - 1, ClientColumns());
		}
	}
	else {
		if (first != NULL) {
			if (newSplit < first->MinColumns())
				newSplit = first->MinColumns();
			if (second != NULL) {
				if (ClientColumns() - newSplit - 1 < second->MinColumns())
					newSplit = ClientColumns() - 1 - second->MinColumns();
			}
			first->Resize(ClientRows(), newSplit);
		}
		if (second != NULL) {
			second->SetBounds(0, newSplit + 1,
					ClientRows(), ClientColumns() - newSplit - 1);
		}
	}

	split = newSplit;
}


/**
 * Set the split position
 *
 * @param newSplit the new split position
 */
void SplitPane::SetSplit(int newSplit)
{
	SetSplitInternal(newSplit);

	if (mainComponent == SPLITPANE_COMPONENT_NONE) {
		splitProportion = split
			/ (double) ((Horizontal() ? ClientRows() : ClientColumns()) - 1);
	}

	Paint();
}


/**
 * Add a component (called from the inside of Component's constructor)
 *
 * @param component the component to add
 */
void SplitPane::Add(Component* component)
{

	// Note that we do not need to update the minimum SplitPane size from the
	// two client components in this function, as this is called from the
	// Component's constructor before the subclass or the creator has a chance
	// to call SetMinSize(). We will handle everything related to minimum sizes
	// in ChildMinSizeChanged().

	if (first == NULL) {
		first = component;

		if (oneComponentMode == SPLITPANE_COMPONENT_NONE
				|| oneComponentMode == SPLITPANE_COMPONENT_SECOND) {
			if (Horizontal()) {
				if (split < first->MinRows())
					split = first->MinRows();
				component->SetBounds(0, 0, split, ClientColumns());
			}
			else {
				if (split < first->MinColumns())
					split = first->MinColumns();
				component->SetBounds(0, 0, ClientRows(), split);
			}
		}

		if (oneComponentMode == SPLITPANE_COMPONENT_FIRST) {
			component->SetBounds(0, 0, ClientRows(), ClientColumns());
		}

		component->SetVisible(Visible()
				&& oneComponentMode != SPLITPANE_COMPONENT_SECOND);
	}
	else if (second == NULL) {
		second = component;

		if (oneComponentMode == SPLITPANE_COMPONENT_NONE
				|| oneComponentMode == SPLITPANE_COMPONENT_FIRST) {
			if (Horizontal()) {
				if (ClientRows() - split - 1 < second->MinRows())
					split = ClientRows() - 1 - second->MinRows();
				component->SetBounds(split + 1, 0,
						ClientRows() - split - 1, ClientColumns());
			}
			else {
				if (ClientColumns() - split - 1 < second->MinColumns())
					split = ClientColumns() - 1 - second->MinColumns();
				component->SetBounds(0, split + 1,
						ClientRows(), ClientColumns() - split - 1);
			}
		}

		if (oneComponentMode == SPLITPANE_COMPONENT_SECOND) {
			component->SetBounds(0, 0, ClientRows(), ClientColumns());
		}

		component->SetVisible(Visible()
				&& oneComponentMode != SPLITPANE_COMPONENT_FIRST);
	}
	else {
		//###
		abort();
	}

	Container::Add(component);
	Paint();
}


/**
 * An event handler for resizing the window
 *
 * @param oldRows the old number of rows
 * @param oldCols the old number of columns
 * @param newRows the new number of rows
 * @param newCols the new number of columns
 */
void SplitPane::OnResize(int oldRows, int oldCols, int newRows, int newCols)
{
	//Component::OnResize(oldRows, oldCols, newRows, newCols);

	if (oneComponentMode == SPLITPANE_COMPONENT_NONE) {

		if (mainComponent == SPLITPANE_COMPONENT_FIRST) {
			SetSplitInternal(split
					+ (Horizontal() ? (newRows-oldRows) : (newCols-oldCols)));
		}
		else if (mainComponent == SPLITPANE_COMPONENT_SECOND) {
			SetSplitInternal(split);
		}
		else {
			SetSplitInternal((int) (0.499 + splitProportion
						* (Horizontal() ? newRows : newCols)));
		}
	}
	else {

		if (mainComponent == SPLITPANE_COMPONENT_FIRST) {
			split += Horizontal() ? (newRows - oldRows) : (newCols - oldCols);
		}

		Component* c = oneComponentMode == SPLITPANE_COMPONENT_FIRST
			? first : second;
		if (c != NULL) c->Resize(newRows, newCols);

		c = oneComponentMode == SPLITPANE_COMPONENT_SECOND
			? first : second;
		if (c != NULL) {
			if (Horizontal()) {
				c->Resize(c->Rows(), newCols);
			}
			else {
				c->Resize(newRows, c->Columns());
			}
		}
	}
}


/**
 * A handler for when a child component's minimum size was set
 *
 * @param child the child component
 * @param newRows the new number of rows
 * @param newCols the new number of columns
 */
void SplitPane::ChildMinSizeChanged(Component* child, int newRows, int newCols)
{
	/* I think this needs to be disabled so that the component will have
	 * proper minimum size after disabling the one-component mode. Maybe we
	 * would need to change this in the future so that we will update our
	 * minimum size and resize the parent component accordingly when we go into
	 * the one-component mode and back.

	 if (oneComponentMode != SPLITPANE_COMPONENT_NONE) {
		if ((child == first && oneComponentMode == SPLITPANE_COMPONENT_FIRST)
			|| (child == second && oneComponentMode == SPLITPANE_COMPONENT_SECOND)) {

			SetMinSize(newRows, newCols);
			if (child->Rows() < newRows || child->Columns() < newCols) {
				Resize(std::max(newRows, child->Rows()),
						std::max(newCols, child->Columns()));
			}
		}
		return;
	}
	
	*/


	// First, update the min size of the SplitPane

	if (Horizontal()) {

		int minRows = 1 + (first == NULL ? 1 : first->MinRows())
			+ (second == NULL ? 1 : second->MinRows());
		int minCols = std::max(first == NULL ? 1 : first->MinColumns(),
				second == NULL ? 1 : second->MinColumns());

		SetMinSize(minRows, minCols);
	}
	else {

		int minRows = std::max(first == NULL ? 1 : first->MinRows(),
				second == NULL ? 1 : second->MinRows());
		int minCols = 1 + (first == NULL ? 1 : first->MinColumns())
			+ (second == NULL ? 1 : second->MinColumns());

		SetMinSize(minRows, minCols);
	}

	if (Rows() < MinRows() || Columns() < MinColumns()) {
		Resize(std::max(MinRows(), Rows()),
				std::max(MinColumns(), Columns()));
	}


	// Now check and update the split position

	SetSplitInternal(split);
}


/**
 * Set the component visibility
 *
 * @param visible true for the component to be be visible
 */
void SplitPane::SetVisible(bool _visible)
{
	if (Visible() == _visible) return;

	if (_visible) {
		Component::SetVisible(true);
		if (first != NULL)
			first->SetVisible(oneComponentMode != SPLITPANE_COMPONENT_SECOND);
		if (second != NULL)
			second->SetVisible(oneComponentMode != SPLITPANE_COMPONENT_FIRST);
	}
	else {
		if (first  != NULL) first->SetVisible(false);
		if (second != NULL) second->SetVisible(false);
		Component::SetVisible(false);
	}
}

