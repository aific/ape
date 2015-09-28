/*
 * Label.cpp
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
#include "Label.h"

#include "Container.h"
#include "Window.h"


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
Label::Label(Container* parent, const char* str, int row, int col,
		int rows, int cols, int anchor)
	: Component(parent, false, row, col, rows, cols, anchor)
{
	text = str;

	align = ALIGN_LEFT;
	tabSize = 4;
}


/**
 * Destroy the object
 */
Label::~Label(void)
{
}


/**
 * Format the next line
 *
 * @param p the position in the string (will be modified)
 * @param maxLine the max number of characters in a line (0 = current size)
 * @return the formatted line
 */
std::string Label::FormatLine(size_t& p, size_t maxLine)
{
	if (maxLine == 0) maxLine = Columns();

	std::string s = "";
	const char* splitBy = " \t\n-";

	while (p < text.length()) {

		// Find the next token

		size_t n = std::string::npos;
		char sep = '\0';
		for (const char* c = splitBy; *c != '\0'; c++) {
			size_t x = text.find_first_of(*c, p);
			if (x != std::string::npos
					&& (x < n || n == std::string::npos)) {
				n = x;
				sep = *c;
			}
		}


		// Process the next token

		size_t newLength = s.length();
		size_t tokenLength;
		if (n == std::string::npos) {
			tokenLength = text.length() - p;
		}
		else {
			tokenLength = n - p;
			if (sep == '-') tokenLength++;
		}
		newLength += tokenLength;


		// The token fits

		if (newLength <= maxLine) {
			s += text.substr(p, tokenLength);
			switch (sep) {
				case ' ':
					if (newLength + 1 <= maxLine) s += " ";
					break;
				case '\t':
					size_t x = ((s.length()+tabSize) / tabSize) * tabSize;
					while (x > s.length() && s.length() <= maxLine) {
						s += " ";
					}
					break;
			}

			if (n == std::string::npos)
				p = text.length();
			else
				p = n + 1;


			// Force finish the line

			if (sep == '\n') break;
		}
		else {

			// The token does not fit

			size_t spaceLeft = maxLine - s.length();
			if (s == "" || (tokenLength > maxLine && spaceLeft > 4)) {

				// Force split the token

				size_t m = spaceLeft;
				if (tokenLength < m) m = tokenLength; else m--;
				s += text.substr(p, m);

				p += m;
				if (p < text.length()) {
					s += "-";
				}

				break;
			}
			else {

				// Just go to the next line

				break;
			}
		}
	}

	return s;
}


/**
 * Paint the contents of the component
 */
void Label::Paint(void)
{
	if (!Visible()) return;

	Clear();

	
	// Paint one line at the time
	
	size_t p = 0;
	size_t lineNo = 0;
	size_t maxLine = Columns();

	while (p < text.length() && lineNo < Rows()) {

		std::string s = FormatLine(p);


		// We finished the line, so let's paint it

		if (p < text.length() && lineNo + 1 == Rows()) {
			if (s.length() + 3 > maxLine && s.length() > 3) {
				s = s.substr(0, maxLine - 3);
			}
			s += "...";
		}

		PaintLine(lineNo, s.c_str());
		lineNo++;
	}
}


/**
 * Paint a line
 * 
 * @param n the line number
 * @param str the string
 */
void Label::PaintLine(int n, const char* str)
{
	size_t l = strlen(str);

	tcw->SetColor(bg, fg);

	switch (align) {
		
		case ALIGN_LEFT:
			tcw->OutText(n, 0, str);
			break;
		
		case ALIGN_RIGHT:
			tcw->OutText(n, Columns() - l, str);
			break;
		
		case ALIGN_CENTER:
			tcw->OutText(n, (Columns() - l) / 2, str);
			break;

		default:
			tcw->OutText(n, 0, "++ ERROR ++");
	}
}


/**
 * Set the text
 *
 * @param str the new text
 */
void Label::SetText(const char* str)
{
	text = str;
	Paint();
}


/**
 * Set the alignment
 *
 * @param a the new alignment
 */
void Label::SetAlignment(int a)
{
	assert(a == ALIGN_LEFT || a == ALIGN_RIGHT || a == ALIGN_CENTER);

	align = a;
}


/**
 * Get the actual number of lines
 *
 * @param columns the number of columns (0 = current)
 * @return the actual number of lines
 */
int Label::NumberOfLines(int columns)
{
	size_t p = 0;
	size_t lines = 0;
	size_t maxLine = columns <= 0 ? Columns() : columns;

	while (p < text.length()) {
		FormatLine(p, maxLine);
		lines++;
	}

	return lines;
}


/**
 * Get the length of the label needed for the text to not have any added
 * line breaks
 *
 * @return the required number of columns
 */
int Label::MaximumLineLength(void)
{
	size_t p = 0;
	size_t maxLength = 0;

	while (p < text.length()) {
		size_t l = FormatLine(p, (size_t) -1).length();
		maxLength = std::max(maxLength, l);
	}

	return maxLength;
}

