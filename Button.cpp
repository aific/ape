/*
 * Button.cpp
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
#include "Button.h"

#include "Container.h"
#include "Window.h"

#define BUTTON_PADDING	" "


/**
 * Create an instance of class Button
 *
 * @param parent the parent container
 * @param str the initial text
 * @param row the initial row
 * @param col the initial column
 * @param cols the number of columns
 * @param anchor set the anchor
 */
Button::Button(Container* parent, const char* str, int row, int col,
		int cols, int anchor)
	: Component(parent, true, row, col, 1,
			cols >= 0 ? cols : 2 * strlen(BUTTON_PADDING) + strlen(str),
			anchor)
{
	text = str;

	bg = 6;
	fg = 0;

	assert(cols < 0 || cols >= 3);
}


/**
 * Destroy the object
 */
Button::~Button(void)
{
}


/**
 * Paint the contents of the component
 */
void Button::Paint(void)
{
	if (!Visible()) return;
	Clear();

	tcw->SetColor(bg, fg);

	size_t c = Columns();
	size_t p = 2 * strlen(BUTTON_PADDING);
	size_t textLength = text.length();

	if (c < p) {
		for (size_t i = 0; i < c; i++) tcw->OutText(0, i, " ");
		MoveCursor(0, c / 2);
	}
	else if (c < p + textLength) {
		std::string s = text.substr(0, c - p);
		tcw->OutFormattedText(0, 0, "%s%s%s", BUTTON_PADDING, s.c_str(),
		                      BUTTON_PADDING);
		MoveCursor(0, strlen(BUTTON_PADDING));
	}
	else {

		// Center the text
		
		size_t l = (c - p - textLength) / 2;
		size_t r = (c - p - textLength) - l;

		tcw->SetCursor(0, 0);
		tcw->PutText(BUTTON_PADDING);
		for (size_t i = 0; i < l; i++) tcw->PutChar(' ');
		tcw->PutText(text.c_str());
		for (size_t i = 0; i < r; i++) tcw->PutChar(' ');
		tcw->PutText(BUTTON_PADDING);

		MoveCursor(0, strlen(BUTTON_PADDING) + l);
	}
}


/**
 * Set the text
 *
 * @param str the new text
 */
void Button::SetText(const char* str)
{
	text = str;
	Paint();
}


/**
 * An event handler for pressing a key
 * 
 * @param key the key code
 */
void Button::OnKeyPressed(int key)
{
	if (key == KEY_ENTER || key == KEY_RETURN || key == ' ') {
		FireOnAction();
		return;
	}
	
	Component::OnKeyPressed(key);
}


/**
 * An event handler for mouse click
 *
 * @param row the row
 * @param column the column
 * @param button the button
 * @param shift whether shift was pressed
 */
void Button::OnMouseClick(int row, int column, int button, bool shift)
{
	if (button == 0 && !shift) {
		FireOnAction();
	}
}
