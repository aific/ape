/*
 * MenuWindow.cpp
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
#include "MenuWindow.h"

#include "Manager.h"


/**
 * Create an instance of class MenuWindow
 * 
 * @param _parent the parent window
 * @param row the initial row of the menu
 * @param col the initial column of the menu
 */
MenuWindow::MenuWindow(Window* _parent, int row, int col)
	: Window("", row, col, 4, 10, 7, 0)
{
	parent = _parent;
	updated = false;
	cursorWraps = true;
	cursor = 0;

	allowMove = false;
	allowResize = false;
	allowMaximize = false;
	hasWindowMenu = false;
}


/**
 * Destroy the object
 */
MenuWindow::~MenuWindow(void)
{
	// Nothing to do
}


/**
 * Initialize a menu item
 * 
 * @param m the menu item to initialize
 * @param title the title
 * @param accel the accelerator string
 * @param highlight the index of the title character to highlight
 * @param code the return code
 */
void MenuWindow::InitItem(MenuItem& m, const char* title, const char* accel, int highlight, int code)
{
	std::strncpy(m.title, title, MENU_ITEM_MAX);
	std::strncpy(m.accel, accel, ACCEL_MAX);
	
	m.title[MENU_ITEM_MAX] = '\0';
	m.accel[ACCEL_MAX] = '\0';
	
	m.highlight = highlight;
	if (m.highlight < 0) m.highlight = -1;
	if (m.highlight >= std::strlen(m.title)) m.highlight = -1;
	
	m.code = code;
	m.enabled = true;
}


/**
 * Add a menu item
 * 
 * @param title the title
 * @param accel the accelerator string
 * @param highlight the index of the title character to highlight
 * @param code the return code
 * @return the menu item handle (index)
 */
int MenuWindow::Add(const char* title, const char* accel, int highlight, int code)
{
	MenuItem m;
	InitItem(m, title, accel, highlight, code);
	items.push_back(m);
	updated = false;
	return items.size() - 1;
}


/**
 * Add a menu item
 * 
 * @param title the title
 * @param highlight the index of the title character to highlight
 * @param code the return code
 * @return the menu item handle (index)
 */
int MenuWindow::Add(const char* title, int highlight, int code)
{
	return Add(title, "", highlight, code);
}


/**
 * Add a menu separator
 */
void MenuWindow::AddSeparator(void)
{
	Add("", "", -1, -1);
}


/**
 * Replace a menu item
 * 
 * @param index the index to replace
 * @param title the title
 * @param accel the accelerator string
 * @param highlight the index of the title character to highlight
 * @param code the return code
 */
void MenuWindow::Replace(int index, const char* title, const char* accel, int highlight, int code)
{
	if (index < 0 || index >= items.size()) return;
	
	MenuItem& m = items[index];
	InitItem(m, title, accel, highlight, code);
	updated = false;
}


/**
 * Replace a menu item
 * 
 * @param index the index to replace
 * @param title the title
 * @param highlight the index of the title character to highlight
 * @param code the return code
 */
void MenuWindow::Replace(int index, const char* title, int highlight, int code)
{
	if (index < 0 || index >= items.size()) return;
	
	MenuItem& m = items[index];
	InitItem(m, title, "", highlight, code);
	updated = false;
}


/**
 * Enable a menu item
 * 
 * @param index the menu item index
 */
void MenuWindow::Enable(int index)
{
	if (index < 0 || index >= items.size()) return;
	MenuItem& m = items[index];
	m.enabled = true;
	updated = false;
}


/**
 * Disable a menu item
 * 
 * @param index the menu item index
 */
void MenuWindow::Disable(int index)
{
	if (index < 0 || index >= items.size()) return;
	MenuItem& m = items[index];
	m.enabled = false;
	updated = false;
}


/**
 * Return the length of the longest menu item
 * 
 * @return the length of the longest menu item
 */
int MenuWindow::LargestTitleLength(void)
{
	int m = 0;
	for (int i = 0; i < items.size(); i++) {
		int l = std::strlen(items[i].title);
		if (l > m) m = l;
	}
	return m;
}


/**
 * Return the length of the longest menu item accelerator
 * 
 * @return the length of the longest menu item accelerator
 */
int MenuWindow::LargestAccelLength(void)
{
	int m = 0;
	for (int i = 0; i < items.size(); i++) {
		int l = std::strlen(items[i].accel);
		if (l > m) m = l;
	}
	return m;
}


/**
 * Update and repaint the menu (call this after you modified using Add)
 */
void MenuWindow::UpdateMenu(void)
{
	updated = true;
	
	if (items.empty()) {	// Treat a menu full of separators just as an empty menu
		cursor = 0;
		Resize(10, 3);
		return;
	}
	
	
	// Get some information about the menu
	
	int maxTitle = LargestTitleLength();
	int maxAccel = LargestAccelLength();
	
	if (maxTitle == 0) {		
		Resize(10, 3);
		return;
	}
	
	
	// Set the properties
	
	int tlen = maxTitle < MENU_ITEM_MIN ? MENU_ITEM_MIN : maxTitle;
	
	hasAccel = maxAccel > 0;
	accelCol = tlen + MENU_SPACE;
	innerWidth = hasAccel ? accelCol + maxAccel : tlen;

	if (innerWidth + 4 < MinColumns()) {
		innerWidth = MinColumns() - 4;
	}
	
	
	// Validate the cursor
	
	if (cursor < 0) cursor = 0;
	if (cursor >= items.size()) cursor = items.size() - 1;
	
	while (cursor >= 1 && (items[cursor].title[0] == '\0'
				|| !items[cursor].enabled)) cursor--;
	if (!items[cursor].enabled) {
		while (cursor < items.size() - 1 && (items[cursor].title[0] == '\0'
					|| !items[cursor].enabled)) cursor++;
		if (!items[cursor].enabled) cursor = 0;
	}
	
	if (cursor < 0) cursor = 0;
	if (cursor >= items.size()) cursor = items.size() - 1;
	
	
	// Resize and repaint
	
	Resize(2 + items.size(), 4 + innerWidth);
	Paint();
}


/**
 * Paint the contents of the window
 */
void MenuWindow::PaintContents(void)
{
	if (!updated) UpdateMenu();
	
	for (int i = 0; i < items.size(); i++) {
		MenuItem& m = items[i];
		int r = i + 1;
		int c = 2;
		bool active = cursor == i;
		
		if (active) {
			tcw->SetColor(2, m.enabled ? 0 : 4);
		}
		else {
			tcw->SetColor(7, m.enabled ? 0 : 4);
		}
		tcw->SetAttribute(A_DIM, true);
		
		if (active) {
			tcw->OutHorizontalLine(r, c - 1, innerWidth + 2, ' ');
		}
		
		if (m.title[0] == '\0') {
			
			// Separator
			
			tcw->OutHorizontalLine(r, c - 1, innerWidth + 2, ACS_HLINE);
			tcw->OutChar(r, c - 2, ACS_LTEE);
			tcw->OutChar(r, c + innerWidth + 1, ACS_RTEE);
		}
		else {
			
			// Menu entry
		
			tcw->OutText(r, c, m.title);
			
			if (m.accel[0] != '\0') {
				tcw->OutText( r, c + accelCol, m.accel);
			}
			
			if (m.enabled && m.highlight >= 0) {
				tcw->SetColor(active ? 2 : 7, 1);
				tcw->OutChar(r, c + m.highlight, m.title[m.highlight]);
			}
		}
	}
}


/**
 * An event handler for pressing a key
 * 
 * @param key the key code
 */
void MenuWindow::OnKeyPressed(int key)
{
	int o_cur = cursor;
	
	if (key == KEY_UP) {
		for (cursor--; cursor >= 1
				&& (items[cursor].title[0] == '\0' || !items[cursor].enabled);
				cursor--) {}
		if (cursorWraps && cursor == 0) {
            if (!items[cursor].enabled || items[cursor].title[0] == '\0') cursor--;
        }
		if (cursor < 0) {
            cursor = cursorWraps ? items.size() - 1 : 0;
            for (; cursor >= 1
					&& (items[cursor].title[0] == '\0' || !items[cursor].enabled);
					cursor--) {}
            if (cursor < 0) cursor = 0;
        }
		if (!items[cursor].enabled || items[cursor].title[0] == '\0') cursor = o_cur;
		Paint(); wm.Refresh();
		return;
	}

	if (key == KEY_DOWN) {
		for (cursor++; cursor < items.size() - 1
				&& (items[cursor].title[0] == '\0' || !items[cursor].enabled);
				cursor++) {}
        if (cursorWraps && cursor == items.size() - 1) {
            if (!items[cursor].enabled || items[cursor].title[0] == '\0') cursor++;
        }
		if (cursor > items.size() - 1) {
            cursor = cursorWraps ? 0 : items.size() - 1;
            for (; cursor < items.size() - 1
					&& (items[cursor].title[0] == '\0' || !items[cursor].enabled);
					cursor++) {}
            if (cursor > items.size() - 1) cursor = items.size() - 1;
        }
		if (!items[cursor].enabled || items[cursor].title[0] == '\0') cursor = o_cur;
		Paint(); wm.Refresh();
		return;
	}
	
	if (key == KEY_ENTER || key == KEY_RETURN) {
		if (items[cursor].enabled && items[cursor].title[0] != '\0') {
			wm.CloseTopMenu(items[cursor].code);
		}
		return;
	}
	
	if (key == KEY_ESC) {
		wm.CloseTopMenu(-27);
		return;
	}
	
	if ((key >= '0' && key <= '9') || (key >= 'a'  && key <= 'z') || (key >= 'A'  && key <= 'Z')) {
		char k = tolower(key);
		bool match = false;
		bool multi = false;
		int fmatch = -1;
		int nmatch = -1;
		int lmatch = -1;
		bool curmat = false;
		
		// If there it only one applicable menu item, choose it
		// If there is more then one, then cycle between them
		
		for (int i = 0; i < items.size(); i++) {
			MenuItem& m = items[i];
			if (m.highlight >= 0 && m.enabled) {
				char c = tolower(m.title[m.highlight]);
				if (c == k) {
					
					if (match) {
						multi = true;
					}
					else {
						fmatch = i;
					}
					
					match = true;
					
					if (cursor == i) curmat = true;
					
					if (multi) {
						if (lmatch == cursor && nmatch == -1) {
							nmatch = i;
						}
					}
							
					lmatch = i;
				}
			}
		}
		
		if (match) {
			
			if (multi) {
				cursor = curmat ? nmatch : fmatch;
				if (cursor < 0) cursor = fmatch;
				Paint(); wm.Refresh();
			}
			else {
				cursor = fmatch;
				wm.CloseTopMenu(items[cursor].code);
			}
		}
		
		return;
	}
}


/**
 * Return the parent window
 * 
 * @return the parent window
 */
Window* MenuWindow::Parent(void)
{
	return parent;
}
