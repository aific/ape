/*
 * Component.h
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


#ifndef __COMPONENT_H
#define __COMPONENT_H

#include <vector>

#include "Events.h"
#include "TerminalControl.h"

#define ANCHOR_LEFT		0x1
#define ANCHOR_RIGHT	0x2
#define ANCHOR_TOP		0x4
#define ANCHOR_BOTTOM	0x8
#define ANCHOR_ALL		0xf

class Container;
class Manager;
class Window;


/**
 * Class Component
 *
 * @author Peter Macko
 */
class Component
{
	friend class Container;
	friend class Manager;

	Container* parent;

	int row, col;
	int screenRow, screenCol;
	
	int rows, cols;
	int minRows, minCols;
	int anchor;

	int cursRow;
	int cursCol;
	bool cursVisible;
	
	bool visible;

	/**
	 * True if the component can receive focus
	 */
	bool canReceiveFocus;

	/**
	 * The collection of event handlers
	 */
	std::vector<EventHandler*> eventHandlers;


protected:

	/**
	 * The terminal control window
	 */
	TerminalControlWindow* tcw;

	/**
	 * The foreground color
	 */
	int fg;
	
	/**
	 * The background color
	 */
	int bg;

	/**
	 * Clear the contents of the component
	 */
	virtual void Clear(void);

	/**
	 * Show and move the cursor
	 *
	 * @param r the row
	 * @param c the column
	 */
	virtual void MoveCursor(int r, int c);

	/**
	 * Focus the next component
	 */
	virtual void FocusNext(void);

	/**
	 * A notification handler for gaining focus
	 */
	void NotifyActive(void);

	/**
	 * A notification handler for losing focus
	 */
	void NotifyInactive(void);
	
	/**
	 * A handler for when the container was moved
	 */
	virtual void ContainerMoved(void);

	/**
	 * An event handler for pressing a key
	 *
	 * @param key the key code
	 */
	virtual void OnKeyPressed(int key);
	
	/**
	 * An event handler for a mouse event
	 *
	 * @param row the row
	 * @param column the column
	 * @param buttonState the button state bits
	 */
	virtual void OnMouseEvent(int row, int column, mmask_t buttonState);

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
	 * An event handler for raising the component
	 */
	virtual void OnRaise(void);

	/**
	 * An event handler for a time step
	 */
	virtual void OnStep(void);

	/**
	 * Fire the OnAction event
	 */
	void FireOnAction(void);

	/**
	 * Fire the OnValueChanged event
	 */
	void FireOnValueChanged(void);


public:

	/**
	 * Create an instance of class Container
	 *
	 * @param parent the parent container
	 * @param focusable true if the component can receive focus
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param anchor set the anchor
	 */
	Component(Container* parent, bool focusable = false, int row = 0,
			int col = 0, int rows = 1, int cols = 1,
			int anchor = ANCHOR_LEFT | ANCHOR_TOP);

	/**
	 * Destroy the object
	 */
	virtual ~Component(void);

	/**
	 * Return true if the component is a container
	 *
	 * @return true if this is a container
	 */
	virtual bool InstanceOfContainer(void) { return false; }
	
	/**
	 * Return the parent container
	 *
	 * @return the parent container
	 */
	inline Container* Parent(void) { return parent; }
	
	/**
	 * Return the parent window, or this if this is the window
	 *
	 * @return the parent window
	 */
	virtual Window* ParentWindow(void);

	/**
	 * Return the Terminal Control Window buffer
	 *
	 * @return the TCW buffer
	 */
	inline TerminalControlWindow* TcwBuffer(void) { return tcw; }

	/**
	 * Return whether the component can receive focus
	 *
	 * @return true if it can receive focus
	 */
	inline bool CanReceiveFocus(void) { return canReceiveFocus; }
	
	/**
	 * Return whether the component is active
	 *
	 * @return true if the component is active
	 */
	virtual bool Active(void);

	/**
	 * Determine if the component is set to be visible
	 *
	 * @return true if the component is visible
	 */
	bool Visible(void);

	/**
	 * Set the component visibility
	 *
	 * @param visible true for the component to be be visible
	 */
	virtual void SetVisible(bool visible);

	/**
	 * Return the component height
	 *
	 * @return the number of rows
	 */
	inline int Rows(void) { return rows; }

	/**
	 * Return the component width
	 *
	 * @return the number of columns
	 */
	inline int Columns(void) { return cols; }

	/**
	 * Return the minimum component height
	 *
	 * @return the number of rows
	 */
	inline int MinRows(void) { return minRows; }

	/**
	 * Return the minimum component width
	 *
	 * @return the number of columns
	 */
	inline int MinColumns(void) { return minCols; }

	/**
	 * Return the component row
	 *
	 * @return the row
	 */
	inline int Row(void) { return row; }

	/**
	 * Return the component column
	 *
	 * @return the column
	 */
	inline int Column(void) { return col; }

	/**
	 * Return the anchor
	 *
	 * @return a logical combination of the ANCHOR_* flags
	 */
	inline int Anchor(void) { return anchor; }

	/**
	 * Return the absolute component row
	 *
	 * @return the row
	 */
	inline int ScreenRow(void) { return screenRow; }

	/**
	 * Return the absolute component column
	 *
	 * @return the column
	 */
	inline int ScreenColumn(void) { return screenCol; }
	
	/**
	 * Determine whether the given coordinate is in the component
	 *
	 * @param row the row
	 * @param column the column
	 * @return true if it contains
	 */
	bool Contains(int row, int column);

    /**
     * Return the first row within the component where the contents can be displayed
     *
     * @return the first row of the client area
     */
    virtual int ClientRow(void) { return 0; }

    /**
     * Return the first column within the component where the contents can be displayed
     *
     * @return the first column of the client area
     */
    virtual int ClientColumn(void) { return 0; }

    /**
     * Return the number of rows that can be used for displaying the contents
     *
     * @return the number of rows in the client area
     */
    virtual int ClientRows(void) { return Rows(); }

    /**
     * Return the number of columns that can be used for displaying the contents
     *
     * @return the number of columns in the client area
     */
    virtual int ClientColumns(void) { return Columns(); }

	/**
	 * Return the background color
	 *
	 * @return the color code
	 */
	inline int BGColor(void) { return bg; }

	/**
	 * Return the foreground color
	 *
	 * @return the color code
	 */
	inline int FGColor(void) { return fg; }

	/**
	 * Get the desired cursor row
	 *
	 * @return the desired cursor row
	 */
	virtual int CursorRow(void) { return cursRow; }

	/**
	 * Get the desired cursor column
	 *
	 * @return the desired cursor column
	 */
	virtual int CursorColumn(void) { return cursCol; }

	/**
	 * Get the cursor visibility
	 *
	 * @return true if the cursor should be visible
	 */
	virtual bool CursorVisible(void) { return cursVisible; }

	/**
	 * Refresh the component
	 */
	virtual void Refresh(void);

	/**
	 * Paint the component
	 */
	virtual void Paint(void);

	/**
	 * Set the background color
	 *
	 * @param color the new background color
	 */
	void SetBGColor(int color);

	/**
	 * Set the foreground color
	 *
	 * @param color the new foreground color
	 */
	void SetFGColor(int color);

	/**
	 * Move the component
	 *
	 * @param row the new row
	 * @param col the new column
	 */
	void Move(int row, int col);

	/**
	 * Resize the component (this also calls Paint)
	 *
	 * @param rows the number of rows
	 * @param cols the number of columns
	 */
	void Resize(int rows, int cols);

	/**
	 * Set the minimum size of the component
	 *
	 * @param rows the number of rows
	 * @param cols the number of columns
	 */
	void SetMinSize(int rows, int cols);

	/**
	 * Resize and move the component (this also calls Paint)
	 *
	 * @param row the new row
	 * @param col the new column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 */
	void SetBounds(int row, int col, int rows, int cols);

	/**
	 * Bring focus to this component
	 */
	void Focus(void);

	/**
	 * Register an event handler
	 *
	 * @param handler the handler
	 */
	virtual void RegisterEventHandler(EventHandler* handler);

	/**
	 * Send a key event
	 *
	 * @param key the key code
	 */
	inline void SendKey(int key) { OnKeyPressed(key); }
};

#endif

