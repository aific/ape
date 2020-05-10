/*
 * Window.cpp
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
#include "Window.h"

#include "Manager.h"
#include "MenuWindow.h"
#include "ScrollBar.h"

#define WCM_MOVE		1
#define WCM_RESIZE		2
#define WCM_MAXIMIZE	3
#define WCM_RESTORE		4
#define WCM_CLOSE		0xDEAD

#define WIN_CORNER_H	3
#define WIN_CORNER_V	2


/**
 * Create an instance of class Window
 *
 * @param _title the title of the window
 * @param _row the initial row
 * @param _col the initial column
 * @param _rows the number of rows
 * @param _cols the number of columns
 * @param _bg the background color
 * @param _fg the foreground color
 */
Window::Window(const char* _title, int _row, int _col, int _rows, int _cols,
		int _bg, int _fg)
	: Container(NULL, _row, _col, _rows, _cols)
{

	canHandleMultiClicks = true;


	// Copy the parameters
	
	fg = _fg;
	bg = _bg;

	std::strncpy(title, _title, 255); title[255] = '\0';
	if (title[0] == '\0') title_buf[0] = '\0'; else std::sprintf(title_buf, " %s ", title);
	tbuf_len = std::strlen(title_buf);


	// Set the default properties

	hasWindowMenu = true;
	allowMove = true;
	allowResize = true;
	allowMaximize = true;

	SetMinSize(3, 10);


	// Other initialization

	SetVisible(false);

	inactiveFrameColor = 4;
	hasFrame = true;
	
	windowMenu = NULL;
	wmMode = WM_NORMAL;
	maximized = false;
}


/**
 * Destroy the object
 */
Window::~Window(void)
{
	if (windowMenu != NULL) delete windowMenu;
}


/**
 * Return true if the window is active
 *
 * @return true if the window is active
 */
bool Window::Active(void)
{
	return wm.Top() == this;
}


/**
 * Use the frame color and attributes
 */
void Window::UseFrameStyle(void)
{
	switch (wmMode) {

		case WM_MOVE:
			tcw->SetColor(bg, 6);
			tcw->SetAttribute(A_DIM, true);
			break;

		case WM_RESIZE:
			tcw->SetColor(bg, 1);
			tcw->SetAttribute(A_DIM, true);
			break;

		default:
			tcw->SetColor(bg, Active() ? fg : inactiveFrameColor);
			break;
	}
}


/**
 * Paint the frame
 */
void Window::PaintFrame(void)
{
	UseFrameStyle();

	tcw->OutBox(0, 0, Rows(), Columns(), false);

	tcw->SetAttribute(A_BOLD, Active());
	tcw->OutFormattedText(0, (Columns() - tbuf_len) / 2, "%s", title_buf);

	tcw->SetAttribute(A_BOLD, false);
}


/**
 * Center the window relative to the parent (or to the screen if there is
 * no parent)
 */
void Window::Center(void)
{
	Move((wm.Rows() - Rows()) / 2, (wm.Columns() - Columns()) / 2);
}


/**
 * Set the title of the window
 *
 * @param _title the new title of the window
 */
void Window::SetTitle(const char* _title)
{
	std::strncpy(title, _title, 255); title[255] = '\0';
	if (title[0] == '\0') title_buf[0] = '\0'; else std::sprintf(title_buf, " %s ", title);
	tbuf_len = std::strlen(title_buf);

	Paint();
}


/**
 * Raise the window to the top
 */
void Window::Raise(void)
{
	wm.Raise(this);
}


/**
 * Close the window
 */
void Window::Close(void)
{
	wm.Close(this);
}


/**
 * Maximize the window
 */
void Window::Maximize(void)
{
	if (maximized) return;


	// Save the current state

	o_row = Row();
	o_col = Column();
	o_rows = Rows();
	o_cols = Columns();


	// Move to the new state

	maximized = true;
	Move(1, 0);
	Resize(wm.Rows() - 2, wm.Columns());
	wm.Paint();


	// Update the menu

	EnsureWindowMenu();
	windowMenu->Replace(wmMaximizeItem, "Restore", 0, WCM_RESTORE);
	windowMenu->Disable(wmMoveItem);
	windowMenu->Disable(wmResizeItem);
	windowMenu->UpdateMenu();
}


/**
 * Restore the maximized window
 */
void Window::Restore(void)
{
	if (!maximized) return;


	// Revert to the old state

	maximized = false;
	Resize(o_rows, o_cols);
	Move(o_row, o_col);
	wm.EnsureValidWindowArea(this);
	wm.Paint();


	// Update the menu

	EnsureWindowMenu();
	windowMenu->Replace(wmMaximizeItem, "Maximize", 2, WCM_MAXIMIZE);
	windowMenu->Enable(wmMoveItem);
	windowMenu->Enable(wmResizeItem);
	windowMenu->UpdateMenu();
}


/**
 * Ensure that the window menu is created
 */
void Window::EnsureWindowMenu(void)
{
	if (windowMenu != NULL) return;

	windowMenu = new MenuWindow(this);
	wmMoveItem = allowMove ? windowMenu->Add("Move", 0, WCM_MOVE) : -1;
	wmResizeItem = allowResize ? windowMenu->Add("Resize", 0, WCM_RESIZE) : -1;
	wmMaximizeItem = allowMaximize ? windowMenu->Add("Maximize", 2, WCM_MAXIMIZE) : -1;
	if (allowMove || allowResize || allowMaximize) windowMenu->AddSeparator();
	windowMenu->Add("Close", 0, WCM_CLOSE);
	windowMenu->UpdateMenu();
}


/**
 * An event handler for pressing a key
 *
 * @param key the key code
 */
void Window::OnKeyPressed(int key)
{
	// Move Mode

	if (wmMode == WM_MOVE) {

		if (key == KEY_LEFT && Column() > 0) {
			Move(Row(), Column() - 1);
			wm.Paint(); wm.Refresh();
		}

		if (key == KEY_RIGHT && Column() < wm.Columns() - Columns()) {
			Move(Row(), Column() + 1);
			wm.Paint(); wm.Refresh();
		}

		if (key == KEY_UP && Row() > 1) {
			Move(Row() - 1, Column());
			wm.Paint(); wm.Refresh();
		}

		if (key == KEY_DOWN && Row() < wm.Rows() - Rows() - 1) {
			Move(Row() + 1, Column());
			wm.Paint(); wm.Refresh();
		}

		if (key == 27 || key == 10) {
			wmMode = WM_NORMAL;
			Paint();
			wm.Refresh();
		}

		if (key == KEY_CTRL(' ') && hasWindowMenu) {
			wmMode = WM_NORMAL;
			Paint();
			EnsureWindowMenu();
			windowMenu->Move(Row() + 1, Column() + 1);
			wm.OpenMenu(windowMenu);
			return;
		}

		return;
	}


	// Resize Mode

	if (wmMode == WM_RESIZE) {

		if (key == KEY_LEFT && Columns() > MinColumns()) {
			Resize(Rows(), Columns() - 1);
			wm.Paint(); wm.Refresh();
		}

		if (key == KEY_RIGHT && Column() < wm.Columns() - Columns()) {
			Resize(Rows(), Columns() + 1);
			wm.Paint(); wm.Refresh();
		}

		if (key == KEY_UP && Rows() > MinRows()) {
			Resize(Rows() - 1, Columns());
			wm.Paint(); wm.Refresh();
		}

		if (key == KEY_DOWN && Row() < wm.Rows() - Rows() - 1) {
			Resize(Rows() + 1, Columns());
			wm.Paint(); wm.Refresh();
		}

		if (key == 27 || key == 10) {
			wmMode = WM_NORMAL;
			Paint();
			wm.Refresh();
		}

		if (key == KEY_CTRL(' ') && hasWindowMenu) {
			wmMode = WM_NORMAL;
			Paint();
			EnsureWindowMenu();
			windowMenu->Move(Row() + 1, Column() + 1);
			wm.OpenMenu(windowMenu);
			return;
		}

		return;
	}


	// Normal Mode

	if (key == KEY_CTRL(' ') && hasWindowMenu) {
		EnsureWindowMenu();
		windowMenu->Move(Row() + 1, Column() + 1);
		wm.OpenMenu(windowMenu);
		return;
	}

	Container::OnKeyPressed(key);
}


/**
 * An event handler for exiting a window menu
 *
 * @param code the menu exit code
 */
void Window::OnWindowMenu(int code)
{
	if (code == WCM_CLOSE) {
		Close();
		return;
	}

	if (code == WCM_MOVE) {
		wmMode = WM_MOVE;
		Paint();
		wm.Refresh();
		return;
	}

	if (code == WCM_RESIZE) {
		wmMode = WM_RESIZE;
		Paint();
		wm.Refresh();
		return;
	}

	if (code == WCM_MAXIMIZE) {
		Maximize();
		wm.Refresh();
		return;
	}

	if (code == WCM_RESTORE) {
		Restore();
		wm.Refresh();
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
void Window::OnMouseDoubleClick(int row, int column, int button, bool shift)
{
	if (allowMaximize && button == 0 && row == 0 && column >= WIN_CORNER_H
	 && column < Columns() - WIN_CORNER_H) {
		if (maximized) {
			Restore();
		}
		else {
			Maximize();
		}
	}
}


/**
 * An event handler for mouse drag
 *
 * @param event the mouse event
 */
void Window::OnMouseDrag(const MouseDragEvent& event)
{
	if (allowMove && event.button == 0 && event.startRow == 0
	 && event.startColumn >= WIN_CORNER_H
	 && event.startColumn < Columns() - WIN_CORNER_H) {
		Move(Row() + event.deltaRow, Column() + event.deltaColumn);
	}
}


/**
 * An event handler for finishing mouse drag
 *
 * @param event the mouse event
 */
void Window::OnMouseDragFinish(const MouseDragEvent& event)
{
}
