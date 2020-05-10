/*
 * Window.h
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


#ifndef __WINDOW_H
#define __WINDOW_H

#include "Container.h"

#define WM_NORMAL		0
#define WM_MOVE			1
#define WM_RESIZE		2
#define WM_CLOSED		0xDEAD

class Manager;
class MenuWindow;



/**
 * Class Window
 *
 * @author Peter Macko
 */
class Window : public Container
{
	friend class Manager;

	int o_row, o_col;
	int o_rows, o_cols;
	bool maximized;

	char title[256];
	char title_buf[256];
	int tbuf_len;

	int wmMode;

	MenuWindow* windowMenu;
	int wmMoveItem;
	int wmResizeItem;
	int wmMaximizeItem;


	/**
	 * Ensure that the window menu is created
	 */
	void EnsureWindowMenu(void);


protected:

	/**
	 * True if the window has a window menu
	 */
	bool hasWindowMenu;

	/**
	 * True if the window can be moved
	 */
	bool allowMove;

	/**
	 * True if the window can be resized
	 */
	bool allowResize;

	/**
	 * True if the window can be maximizes
	 */
	bool allowMaximize;

	/**
	 * The color of the window frame when inactive
	 */
	int inactiveFrameColor;

	/**
	 * Use the frame color and attributes
	 */
	void UseFrameStyle(void);

	/**
	 * Paint the frame
	 */
	virtual void PaintFrame(void);

	/**
	 * Center the window
	 */
	virtual void Center(void);

	/**
	 * An event handler for pressing a key
	 *
	 * @param key the key code
	 */
	virtual void OnKeyPressed(int key);

	/**
	 * An event handler for exiting a window menu
	 *
	 * @param code the menu exit code
	 */
	virtual void OnWindowMenu(int code);
	
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
	 * @param event the mouse event
	 */
	virtual void OnMouseDrag(const MouseDragEvent& event);
	
	/**
	 * An event handler for finishing mouse drag
	 *
	 * @param event the mouse event
	 */
	virtual void OnMouseDragFinish(const MouseDragEvent& event);

public:

	/**
	 * Create an instance of class Window
	 *
	 * @param title the title of the window
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param bg the background color
	 * @param fg the foreground color
	 */
	Window(const char* title = "", int row = 1, int col = 0,
			int rows = 20, int cols = 40, int bg = 7, int fg = 0);

	/**
	 * Destroy the object
	 */
	virtual ~Window(void);
	
	/**
	 * Return the parent window, or this if this is the window
	 *
	 * @return the parent window
	 */
	virtual Window* ParentWindow(void) { return this; }

	/**
	 * Return true if the window is active
	 *
	 * @return true if the window is active
	 */
	virtual bool Active(void);

	/**
	 * Return true if the window is maximized
	 *
	 * @return true if the window is maximized
	 */
	inline bool Maximized(void) { return maximized; }

	/**
	 * Return the title of the window
	 *
	 * @return the title of the window
	 */
	inline const char* Title(void) const { return title; }

	/**
	 * Set the title of the window
	 *
	 * @param title the new title of the window
	 */
	void SetTitle(const char* title);

	/**
	 * Raise the window to the top
	 */
	void Raise(void);

	/**
	 * Close the window
	 */
	void Close(void);

	/**
	 * Maximize the window
	 */
	void Maximize(void);

	/**
	 * Restore the maximized window
	 */
	void Restore(void);

	/**
	 * Return action the mode of the window
	 *
	 * @return WM_NORMAL, WM_MOVE, or WM_RESIZE (or WM_CLOSED if closed)
	 */
	inline int Mode(void) { return wmMode; }

	/**
	 * Determine if this is a dialog window
	 *
	 * @return true if this is a dialog window
	 */
	virtual bool Dialog(void) { return false; }

	/**
	 * Determine if this is a menu window
	 *
	 * @return true if this is a menu window
	 */
	virtual bool Menu(void) { return false; }

	/**
	 * Determine if this is a regular window
	 *
	 * @return true if this is a regular window
	 */
	virtual bool Regular(void) { return !Dialog() && !Menu(); }
};

#endif
