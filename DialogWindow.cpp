/*
 * DialogWindow.cpp
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
#include "DialogWindow.h"

#include "Manager.h"


/**
 * Create an instance of class DialogWindow
 *
 * @param parent the parent window
 * @param title the title
 * @param row the initial row
 * @param col the initial column
 * @param rows the number of rows
 * @param cols the number of columns
 */
DialogWindow::DialogWindow(Window* parent, const char* title, int row, int col,
		int rows, int cols)
	: Window(title, row, col, rows, cols, 7, 0)
{
	parentWindow = parent;


	// Most dialog windows should not be resized

	allowResize = false;
	allowMaximize = false;
}


/**
 * Destroy the object
 */
DialogWindow::~DialogWindow(void)
{
}


/**
 * Center the window relative to the parent (or to the screen if there is
 * no parent)
 */
void DialogWindow::Center(void)
{
	Window* parent = DialogParent();
	if (parent == NULL) {
		Window::Center();
	}
	else {
		Move(parent->Row() + (parent->Rows() - Rows()) / 2,
		     parent->Column() + (parent->Columns() - Columns()) / 2);
	}
}


/**
 * Create an instance of class SimpleDialogWindow
 *
 * @param parent the parent window
 * @param type the dialog type
 * @param title the title
 * @param text the text
 */
SimpleDialogWindow::SimpleDialogWindow(Window* parent, DialogType type,
		const char* title, const char* text)
	: DialogWindow(parent, title)
{
	std::vector<DialogButton> buttons;
	buttons.push_back(DIALOG_BUTTON_OK);
	Init(type, title, text, buttons);
}


/**
 * Destroy the object
 */
SimpleDialogWindow::~SimpleDialogWindow(void)
{
}


/**
 * Initialize the instance
 *
 * @param type the dialog type
 * @param title the title
 * @param text the text
 * @param dialogButtons the buttons
 */
void SimpleDialogWindow::Init(DialogType type, const char* title,
		const char* text, const std::vector<DialogButton>& dialogButtons)
{
	buttonCodes = dialogButtons;
	returnCode = DIALOG_BUTTON_CANCEL;

	switch (type) {
		case DIALOG_TYPE_ERROR:
			bg = 1;
			fg = 7;
			break;
		default:
			break;
	}

	int buttonMinWidth = 8;
	int buttonPaddingLR = 1;
	int buttonSpacing = 2;

	int paddingLR = 4 + (Columns() - ClientColumns());
	int maxWidth = wm.Columns();
	if (maxWidth >= 80) maxWidth = 80;
	if (maxWidth >= 16 + paddingLR) maxWidth -= paddingLR + 8;


	// Create the label

	label = new Label(this, text, 1, 2, ClientRows() - 4, ClientColumns() - 4,
			ANCHOR_ALL);


	// Determine the appropriate dialog width and resize accordingly

	size_t numButtons = dialogButtons.size();
	int buttonWidth = buttonMinWidth;
	for (size_t i = 0; i < dialogButtons.size(); i++) {
		int l = strlen(ButtonText(dialogButtons[i]));
		if (l + buttonPaddingLR * 2 > buttonWidth) {
			buttonWidth = l + buttonPaddingLR * 2;
		}
	}
	int buttonAreaWidth = numButtons * buttonWidth
		+ (numButtons-1) * buttonSpacing;
	int minWidth = buttonAreaWidth;

	int width = label->MaximumLineLength();
	if (width > maxWidth) width = maxWidth;
	if (width < minWidth) width = minWidth;

	Resize(label->NumberOfLines(width) + 4 + (Rows() - ClientRows()),
	       width + paddingLR);


	// Create the buttons

	int c = (ClientColumns() - buttonAreaWidth) / 2;
	for (size_t i = 0; i < dialogButtons.size(); i++) {
		Button* button = new Button(this, ButtonText(dialogButtons[i]),
				ClientRows() - 2, c, buttonWidth, ANCHOR_LEFT | ANCHOR_BOTTOM);
		button->RegisterEventHandler(this);
		buttons.push_back(button);
		c += buttonWidth + buttonSpacing;
	}


	// Finish

	Center();
}


/**
 * Get the text of a button
 *
 * @param button the button code
 * @return the text
 */
const char* SimpleDialogWindow::ButtonText(DialogButton button)
{
	switch (button) {
		case DIALOG_BUTTON_OK     : return "OK";
		case DIALOG_BUTTON_CANCEL : return "Cancel";
		default                   : return "???";
	}
}


/**
 * Display and run the dialog window
 *
 * @return the clicked button, or DIALOG_BUTTON_CANCEL if the window was
 *         closed
 */
DialogButton SimpleDialogWindow::Run(void)
{
	returnCode = DIALOG_BUTTON_CANCEL;
	wm.Add(this);

	// TODO What if we have two dialogs at the same time??? I really need to
	// handle this differently -- either have at most one outstanding Run(),
	// I need to go multi-threaded, or I need to use continuations. That's a
	// lot of work.

	while (Mode() != WM_CLOSED) {
		usleep(20 * 1000);
		wm.ProcessMessages();
	}

	return returnCode;
}


/**
 * An event handler for an action
 *
 * @param sender the sender
 */
void SimpleDialogWindow::OnAction(Component* sender)
{
	for (size_t i = 0; i < buttons.size(); i++) {
		if (sender == buttons[i]) {
			returnCode = buttonCodes[i];
			Close();
			break;
		}
	}
}

