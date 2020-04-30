/*
 * Container.h
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


#ifndef __CONTAINER_H
#define __CONTAINER_H

#include <cstdlib>
#include <vector>

class ScrollBar;

#include "Component.h"


/**
 * Class Container
 *
 * @author Peter Macko
 */
class Container : public Component
{
	friend class Component;
	friend class Manager;

	int horizScrollStart;
	int vertScrollStart;
	int horizScrollEnd;
	int vertScrollEnd;
	ScrollBar* horizScroll;
	ScrollBar* vertScroll;
	
	std::vector<Component*> components;

	size_t activeComponent;
	bool capturesFocus;


protected:

	/**
	 * Whether the container has a frame
	 */
	bool hasFrame;
	
	/**
	 * Add a component (called from the inside of Component's constructor)
	 *
	 * @param component the component to add
	 */
	virtual void Add(Component* component);

	/**
	 * Clear the contents of the component
	 */
	virtual void Clear(void);

	/**
	 * Paint the frame
	 */
	virtual void PaintFrame(void);
	
	/**
	 * Paint the contents of the container
	 */
	virtual void PaintContents(void);
	
	/**
	 * A handler for when the container was moved
	 */
	virtual void ContainerMoved(void);

	/**
	 * A handler for when a child component's minimum size was set
	 *
	 * @param child the child component
	 * @param newRows the new number of rows
	 * @param newCols the new number of columns
	 */
	virtual void ChildMinSizeChanged(Component* child, int newRows, int newCols);

	/**
	 * Focus the first component
	 */
	virtual void FocusFirst(void);

	/**
	 * Focus the last component
	 */
	virtual void FocusLast(void);

	/**
	 * Focus the next component
	 */
	virtual void FocusNext(void);

	/**
	 * Focus the previous component
	 */
	virtual void FocusPrevious(void);

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
	 * An event handler for mouse release
	 *
	 * @param row the row
	 * @param column the column
	 * @param button the button
	 * @param shift whether shift was pressed
	 */
	virtual void OnMouseRelease(int row, int column, int button, bool shift);
	
	/**
	 * An event handler for mouse click
	 *
	 * @param row the row
	 * @param column the column
	 * @param button the button
	 * @param shift whether shift was pressed
	 */
	virtual void OnMouseClick(int row, int column, int button, bool shift);
	
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
	 * An event handler for mouse triple-click and beyond
	 *
	 * @param row the row
	 * @param column the column
	 * @param button the button
	 * @param count the number of clicks
	 * @param shift whether shift was pressed
	 */
	virtual void OnMouseMultipleClick(int row, int column, int button, int count, bool shift);
	
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
	 * An event handler for moving the component
	 */
	virtual void OnMove(void);

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
	 * An event handler for a time step
	 */
	virtual void OnStep(void);

	/**
	 * Add a scroll bar
	 *
	 * @param horiz true if the scrollbar should be horizontal
	 * @param start the leadway space to the left/top of the scrollbar
	 * @param end the empty space behinf the scrollbar
	 */
	void AddScrollBar(bool horiz, int start, int end);

	/**
	 * Find the component index
	 *
	 * @param component the component
	 * @return the index, or -1 if not found
	 */
	ssize_t ComponentIndex(Component* component);


public:

	/**
	 * Create an instance of class Container
	 *
	 * @param parent the parent container
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param anchor set the anchor
	 */
	Container(Container* parent, int row = 0, int col = 0, int rows = 1,
			int cols = 1, int anchor = ANCHOR_LEFT | ANCHOR_RIGHT);

	/**
	 * Destroy the object
	 */
	virtual ~Container(void);

	/**
	 * Return true if the component is a container
	 *
	 * @return true if this is a container
	 */
	virtual bool InstanceOfContainer(void) { return true; }

	/**
	 * Return the horizontal scroll bar
	 *
	 * @return the scroll bar, or NULL if not available
	 */
	inline ScrollBar* HorizScrollBar(void) { return horizScroll; }

	/**
	 * Return the vertical scroll bar
	 *
	 * @return the scroll bar, or NULL if not available
	 */
	inline ScrollBar* VertScrollBar(void) { return vertScroll; }
	
	/**
	 * Return the first row within the component where children can be displayed
	 *
	 * @return the first row of the client area
	 */
	virtual int ClientRow(void) { return hasFrame ? 1 : 0; }
	
	/**
	 * Return the first column within the component where children can be displayed
	 *
	 * @return the first column of the client area
	 */
	virtual int ClientColumn(void) { return hasFrame ? 1 : 0; }
	
	/**
	 * Return the number of rows that can be used for displaying the children
	 *
	 * @return the number of rows in the client area
	 */
	virtual int ClientRows(void) { return Rows()
		- (hasFrame ? 2 : (horizScroll == NULL ? 0 : 1)); }
	
	/**
	 * Return the number of columns that can be used for displaying the children
	 *
	 * @return the number of columns in the client area
	 */
	virtual int ClientColumns(void) { return Columns()
		- (hasFrame ? 2 : (vertScroll == NULL ? 0 : 1)); }

	/**
	 * Get the desired cursor row
	 *
	 * @return the desired cursor row
	 */
	virtual int CursorRow(void);

	/**
	 * Get the desired cursor column
	 *
	 * @return the desired cursor column
	 */
	virtual int CursorColumn(void);

	/**
	 * Get the cursor visibility
	 *
	 * @return true if the cursor should be visible
	 */
	virtual bool CursorVisible(void);

	/**
	 * Set the component visibility
	 *
	 * @param visible true for the component to be be visible
	 */
	void SetVisible(bool visible);

	/**
	 * Get the active child component
	 *
	 * @return the active component, or NULL if none is active
	 */
	Component* ActiveComponent(void);

	/**
	 * Get the active top-level component
	 *
	 * @return the active top-level component, or NULL if none is active
	 */
	Component* ActiveTopLevelComponent(void);
	
	/**
	 * Return the component at the given coordinate
	 *
	 * @param row the row
	 * @param column the column
	 * @return the component, or NULL if none
	 */
	Component* ComponentAt(int row, int column);

	/**
	 * Return true if this container captures focus (i.e., calling FocusNext()
	 * makes sure that the focus does not leave from this container)
	 *
	 * @return true if it captures focus
	 */
	inline bool CapturesFocus(void) { return capturesFocus; }

	/**
	 * Set whether this container should capture focus (i.e., calling FocusNext()
	 * would make sure that the focus does not leave from this container)
	 *
	 * @param capture whether to capture focus
	 */
	inline void SetCapturesFocus(bool capture) { capturesFocus = capture; }

	/**
	 * Paint the component
	 */
	virtual void Paint(void);
};

#endif

