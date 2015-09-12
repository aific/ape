/*
 * Label.h
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


#ifndef __LABEL_H
#define __LABEL_H

#include "Component.h"

#include <string>

#define ALIGN_LEFT		0
#define ALIGN_RIGHT		1
#define ALIGN_CENTER	2


/**
 * A static label
 *
 * @author Peter Macko
 */
class Label : public Component
{
	std::string text;

	int align;
	int tabSize;


	/**
	 * Format the next line
	 *
	 * @param p the position in the string (will be modified)
	 * @param maxLine the max number of characters in a line (0 = current size)
	 * @return the formatted line
	 */
	std::string FormatLine(size_t& p, size_t maxLine = 0);

	/**
	 * Paint a line
	 * 
	 * @param n the line number
	 * @param str the string
	 */
	void PaintLine(int n, const char* str);
	

public:
	
	/**
	 * Create an instance of class Label
	 *
	 * @param parent the parent container
	 * @patam str the initial text
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param anchor set the anchor
	 */
	Label(Container* parent, const char* str, int row = 0, int col = 0,
			int rows = 1, int cols = 16,
			int anchor = ANCHOR_LEFT | ANCHOR_TOP);

	/**
	 * Destroy the object
	 */
	virtual ~Label(void);
	
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
	 * Get the alignment
	 *
	 * @return the alignment
	 */
	inline int Alignment(void) { return align; }
	 
	/**
	 * Set the alignment
	 *
	 * @param a the new alignment
	 */
	void SetAlignment(int a);

	/**
	 * Get the actual number of lines
	 *
	 * @param columns the number of columns (0 = current)
	 * @return the actual number of lines
	 */
	int NumberOfLines(int columns = 0);

	/**
	 * Get the length of the label needed for the text to not have any added
	 * line breaks
	 *
	 * @return the required number of columns
	 */
	int MaximumLineLength(void);
};

#endif
