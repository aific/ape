/*
 * MenuWindow.h
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


#ifndef __MENU_WINDOW_H
#define __MENU_WINDOW_H

#include "Window.h"
#include <vector>

#define MENU_ITEM_MAX	64
#define ACCEL_MAX		4

#define MENU_ITEM_MIN	4
#define MENU_SPACE		2


/**
 * A menu item
 */
typedef struct {
	char title[MENU_ITEM_MAX + 4];
	char accel[ACCEL_MAX + 4];
	int highlight;
	int code;
	bool enabled;
} MenuItem;


/**
 * Class MenuWindow
 *
 * @author Peter Macko
 */
class MenuWindow : public Window
{
	std::vector<MenuItem> items;
	Window* parent;
	
	bool updated;
	bool hasAccel;
	bool cursorWraps;
	int accelCol;
	int innerWidth;
	
	int cursor;
	
	
	/**
	 * Return the length of the longest menu item
	 * 
	 * @return the length of the longest menu item
	 */
	int LargestTitleLength(void);
	
	/**
	 * Return the length of the longest menu item accelerator
	 * 
	 * @return the length of the longest menu item accelerator
	 */
	int LargestAccelLength(void);
	
	/**
	 * Initialize a menu item
	 * 
	 * @param m the menu item to initialize
	 * @param title the title
	 * @param accel the accelerator string
	 * @param highlight the index of the title character to highlight
	 * @param code the return code
	 */
	void InitItem(MenuItem& m, const char* title, const char* accel, int highlight, int code);
	
	
protected:
	
	/**
	 * Paint the contents of the window
	 */
	virtual void PaintContents(void);
	
	/**
	 * An event handler for pressing a key
	 * 
	 * @param key the key code
	 */
	virtual void OnKeyPressed(int key);

	
public:
	
	/**
	 * Create an instance of class MenuWindow
	 * 
	 * @param parent the parent window
	 * @param row the initial row of the menu
	 * @param col the initial column of the menu
	 */
	MenuWindow(Window* parent, int row = 0, int col = 0);

	/**
	 * Destroy the object
	 */
	virtual ~MenuWindow(void);
	
	/**
	 * Add a menu item
	 * 
	 * @param title the title
	 * @param accel the accelerator string
	 * @param highlight the index of the title character to highlight
	 * @param code the return code
	 * @return the menu item handle (index)
	 */
	int Add(const char* title, const char* accel, int highlight, int code);
	
	/**
	 * Add a menu item
	 * 
	 * @param title the title
	 * @param highlight the index of the title character to highlight
	 * @param code the return code
	 * @return the menu item handle (index)
	 */
	int Add(const char* title, int highlight, int code);
	
	/**
	 * Add a menu separator
	 */
	void AddSeparator(void);
	
	/**
	 * Replace a menu item
	 * 
	 * @param index the index to replace
	 * @param title the title
	 * @param accel the accelerator string
	 * @param highlight the index of the title character to highlight
	 * @param code the return code
	 */
	void Replace(int index, const char* title, const char* accel, int highlight, int code);
	
	/**
	 * Replace a menu item
	 * 
	 * @param index the index to replace
	 * @param title the title
	 * @param highlight the index of the title character to highlight
	 * @param code the return code
	 */
	void Replace(int index, const char* title, int highlight, int code);
	
	/**
	 * Enable a menu item
	 * 
	 * @param index the menu item index
	 */
	void Enable(int index);
	
	/**
	 * Disable a menu item
	 * 
	 * @param index the menu item index
	 */
	void Disable(int index);
	
	/**
	 * Update and repaint the menu (call this after you modified using Add)
	 */
	void UpdateMenu(void);
	
	/**
	 * Return the parent window
	 * 
	 * @return the parent window
	 */
	Window* Parent(void);

	/**
	 * Determine if this is a menu window
	 *
	 * @return true if this is a menu window
	 */
	virtual bool Menu(void) { return true; }
};

#endif

