/*
 * Manager.h
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


#ifndef __MANAGER_H
#define __MANAGER_H

#include <vector>

#include "MenuWindow.h"
#include "Window.h"
#include "WindowSwitcher.h"

#define APE_NUM_MOUSE_BUTTONS	5


/**
 * Window manager
 *
 * @author Peter Macko
 */
class Manager
{
	bool initialized;

	int rows, cols;
	bool validsize;

	std::vector<Window*> windows;
	std::vector<Window*> zombies;
	std::vector<MenuWindow*> menuWindows;
	WindowSwitcher* windowSwitcher;

	WINDOW* win;
	TerminalControlWindow* tcw;
	
	std::string status;
	std::string clipboard;

	int processMessagesDepth;
	Window* openDialog;
	
	bool mouseButtonStates[APE_NUM_MOUSE_BUTTONS];
	int lastMouseX, lastMouseY, lastMouseState;
	int lastEffectiveMouseState;


	/**
	 * Find an interator for a window
	 *
	 * @param w the window
	 * @return the interator, or end() if not found
	 */
	std::vector<Window*>::iterator Find(Window* w);

	/**
	 * Paint the main window
	 */
	void PaintMain(void);

	/**
	 * Paint the status bar
	 */
	void PaintStatus(void);

	/**
	 * Paint the menu bar
	 */
	void PaintMenuBar(void);


protected:

	/**
	 * Notification for resizing the terminal
	 */
	void TerminalResized(void);


public:

	/**
	 * Create an instance of class Manager
	 */
	Manager(void);

	/**
	 * Destroy the object
	 */
	virtual ~Manager(void);

	/**
	 * Initialize
	 */
	void Initialize(void);

	/**
	 * Shutdown
	 */
	void Shutdown(void);

	/**
	 * Get the number of rows of the screen
	 *
	 * @return the size of the screen
	 */
	inline int Rows(void) { return rows; }

	/**
	 * Get the number of columns of the screen
	 *
	 * @return the size of the screen
	 */
	inline int Columns(void) { return cols; }

	/**
	 * Add a window
	 *
	 * @param w the window (will be destroyed by the window manager)
	 */
	void Add(Window* w);

	/**
	 * Close and destroy a window
	 *
	 * @param w the window
	 */
	void Close(Window* w);

	/**
	 * Return the collection of all regular windows
	 *
	 * @return a reference to the vector of windows
	 */
	inline const std::vector<Window*>& Windows(void) { return windows; }

	/**
	 * Return the window at the given coordinate
	 *
	 * @param row the row
	 * @param column the column
	 * @return the window, or NULL if none
	 */
	Window* WindowAt(int row, int column);

	/**
	 * Open a menu window
	 *
	 * @param w the window (will NOT be destroyed by the window manager)
	 */
	void OpenMenu(MenuWindow* w);

	/**
	 * Close all menu window
	 *
	 * @param code the exit code
	 */
	void CloseMenus(int code = -1);

	/**
	 * Close the topmost menu window
	 *
	 * @param code the exit code
	 */
	void CloseTopMenu(int code = -1);

	/**
	 * Repaint the screen
	 */
	void Refresh(void);

	/**
	 * Update the cursor position
	 */
	void UpdateCursor(void);

	/**
	 * Raise a window to the top
	 *
	 * @param w the window
	 */
	void Raise(Window* w);

	/**
	 * Ensure that the window is within the valid screen area
	 *
	 * @param w the window
	 */
	void EnsureValidWindowArea(Window* w);

	/**
	 * Paint all windows
	 */
	void Paint(void);

	/**
	 * Get the top window
	 *
	 * @return the top window, or NULL, if there are no windows
	 */
	Window* Top(void);

	/**
	 * Process pending messages
	 */
	void ProcessMessages(void);

	/**
	 * Return the screen
	 *
	 * @return the screen handle
	 */
	inline WINDOW* Screen(void) { return win; }
	
	/**
	 * Set the contents of the status bar
	 *
	 * @param s the new status
	 */
	void SetStatus(const char* s);

	/**
	 * Get the contents of the clipboard
	 *
	 * @return the contents of the clipboard
	 */
	inline const char* Clipboard(void) { return clipboard.c_str(); }

	/**
	 * Set the contents of the clipboard
	 *
	 * @param s the new contents of the clipboard
	 */
	inline void SetClipboard(const char* s) { clipboard = s; }

	/**
	 * Set the contents of the clipboard
	 *
	 * @param s the new contents of the clipboard
	 */
	inline void SetClipboard(const std::string& s) { clipboard = s; }
};

extern Manager wm;

#endif

