/*
 * CheckBox.cpp
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
#include "CheckBox.h"

#include "Container.h"
#include "Window.h"

#define CHECKBOX_CHECKED	'X'
#define CHECKBOX_UNCHECKED	' '


/**
 * Create an instance of class CheckBox
 *
 * @param parent the parent container
 * @param str the initial text
 * @param checked the initial value of the CheckBox
 * @param row the initial row
 * @param col the initial column
 * @param cols the number of columns
 * @param anchor set the anchor
 */
CheckBox::CheckBox(Container* parent, const char* str, bool checked,
		int row, int col, int cols, int anchor)
	: Component(parent, true, row, col, 1, cols >= 0 ? cols : 4+strlen(str),
			anchor)
{
	text = str;
	this->checked = checked;

	assert(cols < 0 || cols >= 3);
}


/**
 * Destroy the object
 */
CheckBox::~CheckBox(void)
{
}


/**
 * Paint the contents of the component
 */
void CheckBox::Paint(void)
{
	if (!Visible()) return;
	Clear();

	tcw->SetColor(bg, fg);

	size_t c = Columns();
	if (c < 3) {
		if (c >= 1) tcw->OutText(0, 0, "!");
	}
	else if (c < 5) {
		tcw->OutFormattedText(0, 0, "[%c]",
				checked ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED);
	}
	else if (c <= 4 + text.length()) {
		tcw->OutFormattedText(0, 0, "[%c] %s",
				checked ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED,
				text.c_str());
	}
	else {
		if (c <= 7) {
			tcw->OutFormattedText(0, 0, "[%c] %s%s%s",
					checked ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED,
					c > 4 ? "." : "",
					c > 5 ? "." : "",
					c > 6 ? "." : "");
		}
		else {
			std::string s = text.substr(0, c - 7);
			tcw->OutFormattedText(0, 0, "[%c] %s...",
					checked ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED,
					s.c_str());
		}
	}

	MoveCursor(0, 1);
}


/**
 * Set the text
 *
 * @param str the new text
 */
void CheckBox::SetText(const char* str)
{
	text = str;
	Paint();
}
	 

/**
 * Check or unchech the CheckBox
 *
 * @param c the new value for "checked"
 */
void CheckBox::SetChecked(bool c)
{
	if (checked != c) {
		checked = c;
		Paint();
		Refresh();
	}
}


/**
 * An event handler for pressing a key
 * 
 * @param key the key code
 */
void CheckBox::OnKeyPressed(int key)
{
	if (key == KEY_ENTER || key == KEY_RETURN || key == ' ') {
		SetChecked(!checked);
		return;
	}
	
	Component::OnKeyPressed(key);
}

