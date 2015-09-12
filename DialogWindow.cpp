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
}


/**
 * Destroy the object
 */
DialogWindow::~DialogWindow(void)
{
}


/**
 * Create an instance of class SimpleDialogWindow
 *
 * @param parent the parent window
 * @param title the title
 * @param text the text
 */
SimpleDialogWindow::SimpleDialogWindow(Window* parent, const char* title,
		const char* text)
	: DialogWindow(parent, title)
{
	int buttonWidth = 8;
	int buttonSpacing = 2;

	int paddingLR = 4 + (Columns() - ClientColumns());
	int maxWidth = wm.Columns();
	if (maxWidth >= 80) maxWidth = 80;
	if (maxWidth >= 16 + paddingLR) maxWidth -= paddingLR + 8;


	// Create the label

	label = new Label(this, text, 1, 2, ClientRows() - 4, ClientColumns() - 4,
			ANCHOR_ALL);


	// Determine the appropriate dialog width and resize accordingly

	size_t numButtons = 1;
	int minWidth = numButtons * buttonWidth + (numButtons-1) * buttonSpacing;

	int width = label->MaximumLineLength();
	if (width > maxWidth) width = maxWidth;
	if (width < minWidth) width = minWidth;

	Resize(label->NumberOfLines(width) + 4 + (Rows() - ClientRows()),
	       width + paddingLR);


	// Center

	if (parent == NULL) {
		Move((wm.Rows() - Rows()) / 2, (wm.Columns() - Columns()) / 2);
	}
	else {
		Move(parent->Row() + (parent->Rows() - Rows()) / 2,
		     parent->Column() + (parent->Columns() - Columns()) / 2);
	}


	// Create the buttons

	button = new Button(this, "OK", ClientRows() - 2, 2, buttonWidth,
			ANCHOR_LEFT | ANCHOR_BOTTOM);


	// Finish

	allowResize = false;
	allowMaximize = false;
}


/**
 * Destroy the object
 */
SimpleDialogWindow::~SimpleDialogWindow(void)
{
}

