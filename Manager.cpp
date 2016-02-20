/*
 * Manager.cpp
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
#include "Manager.h"

#include <sys/ioctl.h>
#include <csignal>

#ifdef _MAC
#include <util.h>
#endif

#include "DialogWindow.h"
#include "EditorWindow.h"
#include "FileDialog.h"

Manager wm;


/**
 * Handle the SIGWINCH signal
 *
 * @param sig the signal code
 */
void SigWinChHandler(int sig)
{
	ungetch(KEY_RESIZE);
}


/**
 * Create an instance of class Manager
 */
Manager::Manager(void)
{
	initialized = false;

	processMessagesDepth = 0;
	openDialog = NULL;

	clipboard = "";
}


/**
 * Destroy the object
 */
Manager::~Manager(void)
{
	Shutdown();
}


/**
 * Initialize
 */
void Manager::Initialize(void)
{
	if (initialized) return;
	initialized = true;


	// Configure the Curses
	
	if (getenv("ESCDELAY") == NULL) {
		// Set the delay after ESC to 25ms (this is apparently the VIM default)
		setenv("ESCDELAY", "25", 1);
	}

	
	// Initialize the Curses

	initscr();
	noecho();


	// Initialize keyboard

	raw();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);


	// Get the screen size

	getmaxyx(stdscr, rows, cols);


	// Initialize colors

	if (has_colors()) {
		start_color();
		for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) {
			init_pair(j*8+7-i, i, j);
		}
	}


	// Initialize the main window

	win = newwin(rows, cols, 0, 0);
	tcw = new TerminalControlWindow(rows, cols);


	// Initialize signals

	signal(SIGWINCH, SigWinChHandler);


	// Initialize the internal state

	validsize = true;
	status = "";
}


/**
 * Shutdown
 */
void Manager::Shutdown(void)
{
	if (!initialized) return;
	initialized = false;

	signal(SIGWINCH, SIG_DFL);

	for (int i = 0; i < windows.size(); i++) delete windows[i];
	for (int i = 0; i < zombies.size(); i++) delete zombies[i];

	delete tcw;
	delwin(win);

	endwin();
}


/**
 * Paint the main window
 */
void Manager::PaintMain(void)
{
	tcw->SetColor(0, 8);
	tcw->SetAttribute(A_DIM, true);
	for (int i = 0; i < rows - 2; i++) {
		tcw->OutHorizontalLine(i + 1, 0, cols, ACS_CKBOARD);
	}

	PaintStatus();
	PaintMenuBar();
}


/**
 * Paint the status bar
 */
void Manager::PaintStatus(void)
{
	tcw->SetColor(7, 7);
	tcw->SetAttribute(A_DIM, true);
	tcw->OutHorizontalLine(rows - 1, 0, cols, ' ');
	
	tcw->OutText(rows - 1, 1, status.c_str());
}


/**
 * Paint the menu bar
 */
void Manager::PaintMenuBar(void)
{
	tcw->SetColor(7, 7);
	tcw->SetAttribute(A_DIM, true);
	tcw->OutHorizontalLine(0, 0, cols, ' ');
}


/**
 * Paint all windows
 */
void Manager::Paint(void)
{
	// Handle invalid terminal size first

	if (!validsize) {

		wmove(win, 0, 0);
		wattrset(win, COLOR_PAIR(1 * 8 + 0) | A_BOLD);
		for (int i = 0; i < rows * cols; i++) waddch(win, ' ');

		const char* complaint = "The terminal is too small.";
		int c = (cols - std::strlen(complaint)) / 2; if (c < 0) c = 0;
		mvwaddstr(win, rows / 2, c, complaint);

		return;
	}


	// Otherwise proceed with normal paint

	PaintMain();

	for (int i = 0; i < windows.size(); i++) {
		if (windows[i]->Visible()) {
			windows[i]->Paint();
			tcw->OutBuffer(windows[i]->Row(), windows[i]->Column(),
					windows[i]->TcwBuffer());
		}
	}

	if (windowSwitcher != NULL) {
		windowSwitcher->Paint();
		tcw->OutBuffer(windowSwitcher->Row(), windowSwitcher->Column(),
				windowSwitcher->TcwBuffer());
	}

	for (int i = 0; i < menuWindows.size(); i++) {
		if (menuWindows[i]->Visible()) {
			menuWindows[i]->Paint();
			tcw->OutBuffer(menuWindows[i]->Row(), menuWindows[i]->Column(),
					menuWindows[i]->TcwBuffer());
		}
	}
}


/**
 * Add a window
 *
 * @param w the window (will be destroyed by the window manager)
 */
void Manager::Add(Window* w)
{
	Window* l = windows.empty() ? NULL : windows[windows.size() - 1];
	if (l != NULL) l->NotifyInactive();

	if (w != windowSwitcher) windows.push_back(w);

	w->SetVisible(true);
	w->NotifyActive();
}


/**
 * Close a window
 *
 * @param w the window
 */
void Manager::Close(Window* w)
{
	std::vector<Window*>::iterator i = Find(w);

	if (i != windows.end() || w == windowSwitcher) {

		w->NotifyInactive();
		w->wmMode = WM_CLOSED;

		if (i != windows.end()) windows.erase(i);
		if (w == windowSwitcher) windowSwitcher = NULL;

		zombies.push_back(w);

		Window* l = windows.empty() ? NULL : windows[windows.size() - 1];
		if (l != NULL) l->NotifyActive();
	}

	Refresh();
}


/**
 * Open a menu window
 *
 * @param w the window (will NOT be destroyed by the window manager)
 */
void Manager::OpenMenu(MenuWindow* w)
{
	menuWindows.push_back(w);
	w->UpdateMenu();
	w->SetVisible(true);
	w->NotifyActive();
	Refresh();
}


/**
 * Close all menu windows
 *
 * @param code the exit code
 */
void Manager::CloseMenus(int code)
{
	if (menuWindows.empty()) return;

	MenuWindow* w = menuWindows[0];
	for (int i = 0; i < menuWindows.size(); i++) {
		menuWindows[i]->NotifyInactive();
		menuWindows[i]->SetVisible(false);
	}
	menuWindows.clear();

	Window* p = w->Parent();
	if (p != NULL && code >= 0) p->OnWindowMenu(code);
}


/**
 * Close the topmost menu window
 *
 * @param code the exit code
 */
void Manager::CloseTopMenu(int code)
{
	if (menuWindows.empty()) return;

	MenuWindow* w = menuWindows[menuWindows.size() - 1];
	w->NotifyInactive();
	w->SetVisible(false);
	menuWindows.pop_back();

	Refresh();

	Window* p = w->Parent();
	if (p != NULL) p->OnWindowMenu(code);
}


/**
 * Find an interator for a window
 *
 * @param w the window
 * @return the interator, or end() if not found
 */
std::vector<Window*>::iterator Manager::Find(Window* w)
{
	std::vector<Window*>::iterator i = windows.begin();
	while (i != windows.end()) {
		if (*i == w) return i;
		i++;
	}
	return i;
}


/**
 * Repaint the screen
 */
void Manager::Refresh(void)
{
	// Check whether the terminal has a valid size

	if (!validsize) {
		wrefresh(win);
		move(rows - 1, cols - 1);
		curs_set(FALSE);
		return;
	}


	// Paint
	
	Paint();
	tcw->Paint(win);
	wrefresh(win);


	// Update the cursor location

	UpdateCursor();
}


/**
 * Update the cursor position
 */
void Manager::UpdateCursor(void)
{
	Window* w = Top();
	if (w != NULL) {
		if (w->CursorVisible()) {
			if (w->CursorRow() >= w->Rows() - 1
					|| w->CursorColumn() >= w->Columns() - 1) {
				move(rows - 1, cols - 1);
				curs_set(FALSE);
			}
			else {
                move(w->ScreenRow() + w->ClientRow() + w->CursorRow(),
                     w->ScreenColumn() + w->ClientColumn() + w->CursorColumn());
                curs_set(TRUE);
			}
		}
		else {
			move(rows - 1, cols - 1);
			curs_set(FALSE);
		}
	}
}


/**
 * Raise a window to the top
 *
 * @param w the window
 */
void Manager::Raise(Window* w)
{
	Window* l = windows.empty() ? NULL : windows[windows.size() - 1];
	if (l != NULL) l->NotifyInactive();

	std::vector<Window*>::iterator i = Find(w);
	if (i != windows.end()) {
		windows.erase(i);
		windows.push_back(w);
	}

	w->NotifyActive();
	w->OnRaise();
	Refresh();
}


/**
 * Get the top window
 *
 * @return the top window, or NULL, if there are no windows
 */
Window* Manager::Top(void)
{
	if (!menuWindows.empty()  ) return menuWindows[menuWindows.size() - 1];
	if (windowSwitcher != NULL) return windowSwitcher;
	return windows.empty() ? NULL : windows[windows.size() - 1];
}


/**
 * Ensure that the window is within the valid screen area
 *
 * @param w the window
 */
void Manager::EnsureValidWindowArea(Window* w)
{
	// First handle the maximized window

	if (w->Maximized()) {
		w->Resize(rows - 2, cols);
		return;
	}


	// Calculate the new postion and size deltas

	int er = w->Row() + w->Rows();
	int ec = w->Column() + w->Columns();
	int rr = w->Rows(), dmr = 0;
	int rc = w->Columns(), dmc = 0;

	if (er >= rows - 1) {
		dmr = rows - er - 1;
		if (w->Row() + dmr < 1) {
			rr = rows - 2;
			dmr = 1 - w->Row();
		}
	}

	if (ec >= cols) {
		dmc = cols - ec - 1;
		if (w->Column() + dmc < 0) {
			rc = cols - 1;
			dmc = -w->Column();
		}
	}


	// Update the position and size

	if (dmr != 0 || dmc != 0) {
		w->Move(w->Row() + dmr, w->Column() + dmc);
	}

	if (rr != w->Rows() || rc != w->Columns()) {
		w->Resize(rr, rc);
	}
}


/**
 * Notification for resizing the terminal
 */
void Manager::TerminalResized(void)
{
	// Backup the original settings

	int o_rows = rows;
	int o_cols = cols;


	// Get the screen size

	struct winsize size;
	size.ws_row = size.ws_col = 0;

	ioctl(0, TIOCGWINSZ, &size);
	if (size.ws_row && size.ws_col) {
		rows = size.ws_row;
		cols = size.ws_col;
	}


	// Resize the terminal

	resizeterm(rows, cols);


	// Calculate the deltas

	int drows = rows - o_rows;
	int dcols = cols - o_cols;


	// Resize the main window

	wresize(win, rows, cols);
	tcw->Resize(rows, cols);


	// Get the minimum size of the terminal

	int mr = 3;
	int mc = 10;

	for (int i = 0; i < windows.size(); i++) {
		if (windows[i]->MinRows() > mr) mr = windows[i]->MinRows();
		if (windows[i]->MinColumns() > mc) mc = windows[i]->MinColumns();
	}

	for (int i = 0; i < menuWindows.size(); i++) {
		if (menuWindows[i]->MinRows() > mr) mr = menuWindows[i]->MinRows();
		if (menuWindows[i]->MinColumns() > mc) mc = menuWindows[i]->MinColumns();
	}

	mr += 2;


	// Check the minimum size

	if (rows < mr || cols < mc) {
		validsize = false;
		Paint();
		Refresh();
	}
	else {
		validsize = true;
	}


	// Resize or move the other windows

	for (int i = 0; i < windows.size(); i++) EnsureValidWindowArea(windows[i]);


	// Repaint

	Paint();
	Refresh();
}


/**
 * Process pending messages
 */
void Manager::ProcessMessages(void)
{
	int key;
	processMessagesDepth++;

	Refresh(); // XXX Do I really have to do this?

	while ((key = getch()) != ERR) {


		// Translate the Shift-Arrow key combinations

		if (key == KEY_SR) key = KEY_SHIFT_UP;
		else if (key == KEY_SF) key = KEY_SHIFT_DOWN;
		else if (key == 519)  key = KEY_SHIFT_UP;
		else if (key == 512) key = KEY_SHIFT_DOWN;

		if (key == KEY_SLEFT ) key = KEY_SHIFT_LEFT;
		if (key == KEY_SRIGHT) key = KEY_SHIFT_RIGHT;


		// Translate other escape sequences

		if (key == KEY_ESC) {
			key = getch();
			if (key == ERR) key = KEY_ESC;

			else if (key == 'O') {
				key = getch();
				if (key == '2') {
					key = getch();
					switch (key) {
						case 'A': key = KEY_SHIFT_UP; break;
						case 'B': key = KEY_SHIFT_DOWN; break;
						case 'C': key = KEY_SHIFT_RIGHT; break;
						case 'D': key = KEY_SHIFT_LEFT; break;
					}
				}
				else {
					switch (key) {
						case 'A': key = KEY_UP; break;
						case 'B': key = KEY_DOWN; break;
						case 'C': key = KEY_RIGHT; break;
						case 'D': key = KEY_LEFT; break;
					}
				}
			}

			else if (key == '[') {
				key = getch();
				if (key == '1') {
					key = getch();
					switch (key) {
						case '~': key = KEY_HOME; break;
					}
				}
				else if (key == '4') {
					key = getch();
					switch (key) {
						case '~': key = KEY_END; break;
					}
				}
			}
		}


		// Handle special events

		if (key == KEY_RESIZE) {
			TerminalResized();
			continue;
		}


		// Keyboard events

		if (key != ERR) {

			// Handle system-wide keyboard events

			if (!validsize) {
				if (key == KEY_CTRL('c') || key == KEY_CTRL('q') || key == KEY_ESC) std::exit(1);
			}

			if (key == KEY_CTRL('q')) std::exit(0);

			if (key == KEY_F(2)) {
				if (windowSwitcher == NULL) {
					windowSwitcher = new WindowSwitcher();
					CloseMenus(-1);
					Add(windowSwitcher);
				}
				else {
					if (windowSwitcher->Transient()) {
						// TODO Make it not transient? Or just kill the timer?
					}
					else {
						windowSwitcher->Close();
					}
				}
			}

			else if (key == KEY_CTRL('o')) {

				if (windowSwitcher != NULL) windowSwitcher->Close();
				CloseMenus(-1);

				if (openDialog != NULL) {
					openDialog->Raise();
				}
				else {
					FileDialog* d = new FileDialog(NULL, FILE_DIALOG_OPEN, "Open");
					openDialog = d;
					if (d->Run()) {
						// TODO Window placement
						EditorWindow* w = new EditorWindow(1, 1, 20, 64);
						ReturnExt r = w->LoadFromFile(d->Path().c_str());
						if (!r) {
							w->Close();
							Dialogs::Error(NULL, r);
						}
						else {
							wm.Add(w);
						}
					}
					openDialog = NULL;
				}
			}


			// Otherwise pass the event to the topmost window

			if (Top() != NULL) {
				Top()->OnKeyPressed(key);
			}
		}
	}


	// Time step

	if (Top() != NULL) {
		Top()->OnStep();
	}


	// Finish
	
	processMessagesDepth--;
}


/**
 * Set the contents of the status bar
 *
 * @param s the new status
 */
void Manager::SetStatus(const char* s)
{
	status = s;
	PaintStatus();
}
