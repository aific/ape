/*
 * CheckBox.h
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


#ifndef __CHECKBOX_H
#define __CHECKBOX_H

#include "Component.h"

#include <string>


/**
 * A one-line check-box
 *
 * @author Peter Macko
 */
class CheckBox : public Component
{
	std::string text;
	bool checked;
	

protected:

	/**
	 * An event handler for pressing a key
	 * 
	 * @param key the key code
	 */
	virtual void OnKeyPressed(int key);


public:
	
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
	CheckBox(Container* parent, const char* str, bool checked = false,
			int row = 0, int col = 0,
			int cols = -1, int anchor = ANCHOR_LEFT | ANCHOR_TOP);

	/**
	 * Destroy the object
	 */
	virtual ~CheckBox(void);
	
	/**
	 * Paint the contents of the window
	 */
	virtual void Paint(void);
	
	/**
	 * Get the content text
	 *
	 * @return the text
	 */
	inline const char* Text(void) { return text.c_str(); }
	 
	/**
	 * Set the text
	 *
	 * @param str the new text
	 */
	void SetText(const char* str);

	/**
	 * Determine if the component is checked
	 *
	 * @return true if it is checked
	 */
	inline bool Checked(void) { return checked; }
	 
	/**
	 * Check or unchech the CheckBox
	 *
	 * @param c the new value for "checked"
	 */
	void SetChecked(bool c);
};

#endif
