/*
 * WindowSwitcher.cpp
 *
 * Copyright (c) 2016, Peter Macko
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
#include "WindowSwitcher.h"

#include "Manager.h"


/**
 * Create an instance of class WindowSwitcher
 *
 * @param transient true for a transient list that disappears after a timeout
 */
WindowSwitcher::WindowSwitcher(bool transient)
	: DialogWindow(NULL, transient ? "" : "Windows", 1, 1,
			2 + std::max(1, std::min((int) wm.Windows().size(),
			                         (wm.Rows() - 2) * 4 / 3)))
{
	this->transient = transient;

	windowList = new List<WindowSwitcherItem>(this, !transient /* sorted */,
			0, 0, ClientRows(), ClientColumns(), ANCHOR_ALL);
	windowList->RegisterEventHandler(this);

	const std::vector<Window*>& windows = wm.Windows();
	for (ssize_t i = ((ssize_t) windows.size()) - 1; i >= 0; i--) {
		windowList->Add(WindowSwitcherItem(windows[i]));
	}
	if (!windows.empty()) {
		windowList->SetCursor(
				windowList->Find(WindowSwitcherItem(windows[windows.size()-1])));
	}
	if (windows.size() < 2) {
		windowList->SetScrollBar(NULL);
	}

	windowList->SetMinSize(std::min((int) windows.size(), 2), 10);

	Center();

	if (transient) {
		allowResize = false;
		allowMaximize = false;
	}
}


/**
 * Destroy the object
 */
WindowSwitcher::~WindowSwitcher(void)
{
}


/**
 * An event handler for an action
 *
 * @param sender the sender
 */
void WindowSwitcher::OnAction(Component* sender)
{
	if (sender == windowList) {
		if (windowList->Size() > 0 && windowList->Cursor() >= 0) {
			WindowSwitcherItem& item = windowList->Item(windowList->Cursor());
			item.Value()->Raise();
		}
		Close();
	}
}


/**
 * An event handler for pressing a key
 *
 * @param key the key code
 */
void WindowSwitcher::OnKeyPressed(int key)
{
	if (key == KEY_ESC) {
		Close();
		return;
	}

	DialogWindow::OnKeyPressed(key);
}

