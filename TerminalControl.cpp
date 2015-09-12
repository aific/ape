/*
 * TerminalControl.cpp
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
#include "TerminalControl.h"


/**
 * The global terminal control
 */
TerminalControl terminal;


/**
 * Create a new empty line
 *
 * @param character the character
 * @param length the length
 */
TerminalControlWindow::Line::Line(const Character& character, int length)
{
	contents.resize(length);
	for (int i = 0; i < length; i++) {
		contents[i] = character;
	}
}


/**
 * Clear the line
 *
 * @param character the character
 */
void TerminalControlWindow::Line::Clear(const Character& character)
{
	for (size_t i = 0; i < contents.size(); i++) {
		contents[i] = character;
	}
}


/**
 * Resize the line
 *
 * @param character the fill character
 * @param length the length
 */
void TerminalControlWindow::Line::Resize(const Character& character,
		int length)
{
	assert(length >= 0);

	if (length < (int) contents.size()) {
		contents.erase(contents.begin() + length, contents.end());
	}

	while (length > (int) contents.size()) {
		contents.push_back(character);
	}
}


/**
 * Create a new window
 *
 * @param rows the number of rows
 * @param cols the number of columns
 */
TerminalControlWindow::TerminalControlWindow(int rows, int cols)
{
	visible = false;

	memset(&prototype, 0, sizeof(prototype));
	prototype.character = ' ';

	for (int r = 0; r < rows; r++) {
		lines.push_back(new Line(prototype, cols));
	}

	posRow = 0;
	posCol = 0;
}


/**
 * Destroy the window
 */
TerminalControlWindow::~TerminalControlWindow()
{
	for (size_t r = 0; r < lines.size(); r++) {
		delete lines[r];
	}
}


/**
 * Resize
 *
 * @param rows the number of rows
 * @param cols the number of columns
 */
void TerminalControlWindow::Resize(int rows, int cols)
{
	assert(rows > 0 && cols > 0);

	if (rows < (int) lines.size()) {
		for (size_t r = rows; r < lines.size(); r++) {
			delete lines[r];
		}
		lines.erase(lines.begin() + rows, lines.end());
	}

	for (int r = 0; r < (int) lines.size(); r++) {
		lines[r]->Resize(prototype, cols);
	}

	while (rows > (int) lines.size()) {
		lines.push_back(new Line(prototype, cols));
	}
}


/**
 * Paint onto the given curses window
 *
 * @param win the curses window
 * @param row the row
 * @param col the column
 */
void TerminalControlWindow::Paint(WINDOW* win, int row, int col)
{
	int winRows, winCols;
	getmaxyx(win, winRows, winCols);
	if (row >= winRows || col >= winCols) return;

	for (int r = 0; r < (int) lines.size(); r++) {
		if (r + row < 0) continue;
		if (r + row >= winRows) break;

		Line& line = *lines[r];
		wmove(win, r + row, col);

		for (int c = col < 0 ? -col : 0; c < line.Length(); c++) {
			if (c + col >= winCols) break;
			Character& ch = line[c];

			char cc = ch.character;
			if (iscntrl(cc)) cc = '?';

			wattrset(win, ch.attributes);
			waddch(win, cc);
		}
	}
}


/**
 * Clear
 */
void TerminalControlWindow::Clear()
{
	for (size_t i = 0; i < lines.size(); i++) {
		lines[i]->Clear(prototype);
	}
}


/**
 * Set the color
 *
 * @param bg the background color
 * @param fg the foreground color
 */
void TerminalControlWindow::SetColor(int bg, int fg)
{
	prototype.attributes = COLOR_PAIR(bg * 8 + 7 - fg);
}


/**
 * Set an attribute
 *
 * @param attribute an ncurses attribute
 * @param value true to turn on, false to turn off
 */
void TerminalControlWindow::SetAttribute(int attribute, bool value)
{
	if (value) {
		prototype.attributes |= attribute;
	}
	else {
		prototype.attributes &= ~attribute;
	}
}


/**
 * Write the character onto the buffer (does not wrap)
 *
 * @param row the row
 * @param col the column
 * @param c the character
 * @return the number of characters written
 */
int TerminalControlWindow::OutChar(int row, int col, int c)
{
	if (row < 0 || row >= (int) lines.size()) return 0;

	Line& line = *lines[row];

	if (col < 0 || col >= line.Length()) return 0;

	Character ch = prototype;
	ch.character = c & 0xff;
	ch.attributes |= c & ~0xff;

	line[col++] = ch;
	return 1;
}


/**
 * Write the text onto the buffer (does not wrap)
 *
 * @param row the row
 * @param col the column
 * @param str the string
 * @return the number of characters written
 */
int TerminalControlWindow::OutText(int row, int col, const char* str)
{
	int n = 0;
	if (row < 0 || row >= (int) lines.size()) return n;

	int l = strlen(str);
	if (col < 0) {
		if (col + l >= 0) {
			str += -col;
			l += col;
			col = 0;
		}
		else {
			return n;
		}
	}

	Line& line = *lines[row];
	int length = line.Length();

	if (col >= length) return n;
	if (col + l > length) {
		l = length - col;
	}

	Character c = prototype;
	for (int i = 0; i < l; i++) {
		c.character = str[i];
		line[col + i] = c;
		n++;
	}

	return n;
}


/**
 * Write the text onto the buffer (does not wrap)
 *
 * @param row the row
 * @param col the column
 * @param format the format
 * @return the number of characters written
 */
int TerminalControlWindow::OutFormattedText(int row, int col,
		const char* format, ...)
{
	va_list ap;
	char* s = NULL;

	va_start(ap, format);
	vasprintf(&s, format, ap);
	va_end(ap);

	if (s == NULL) return -1;

	int r = OutText(row, col, s);

	free(s);
	return r;
}


/**
 * Draw a horizontal line
 *
 * @param row the row
 * @param col the column
 * @param length the length
 * @param character the character
 * @return the number of characters written
 */
int TerminalControlWindow::OutHorizontalLine(int row, int col, int length,
		int character)
{
	if (length <= 0) return 0;
	if (row < 0 || row >= (int) lines.size()) return 0;

	Line& line = *lines[row];

	if (col <= -length || col >= line.Length()) return 0;

	if (col < 0) {
		length += col;
		col = 0;
	}
	if (col + length > line.Length()) {
		length = line.Length() - col;
	}

	Character ch = prototype;
	ch.character = character & 0xff;
	ch.attributes |= character & ~0xff;

	int l = length;
	while (l --> 0) line[col++] = ch;

	return length;
}


/**
 * Draw a vertical line
 *
 * @param row the row
 * @param col the column
 * @param length the length
 * @param character the character
 * @return the number of characters written
 */
int TerminalControlWindow::OutVerticalLine(int row, int col, int length,
		int character)
{
	if (length <= 0) return 0;

	int maxCols = lines[0]->Length();

	if (row <= -length || row >= (int) lines.size()) return 0;
	if (col < 0 || col >= maxCols) return 0;

	if (row < 0) {
		length += row;
		row = 0;
	}
	if (row + length > (int) lines.size()) {
		length = ((int) lines.size()) - row;
	}

	Character ch = prototype;
	ch.character = character & 0xff;
	ch.attributes |= character & ~0xff;

	int l = length;
	while (l --> 0) (*lines[row++])[col] = ch;

	return length;
}


/**
 * Draw a box
 *
 * @param row the row
 * @param col the column
 * @param rows the number of rows
 * @param cols the number of columns
 * @param fill true to fill
 */
void TerminalControlWindow::OutBox(int row, int col,
		int rows, int cols, bool fill)
{
	if (rows <= 0 || cols <= 0) return;

	if (cols > 2) {
		OutHorizontalLine(row +        0, col + 1, cols - 2);
		OutHorizontalLine(row + rows - 1, col + 1, cols - 2);
	}

	if (rows > 2) {
		OutVerticalLine(row + 1, col +        0, rows - 2);
		OutVerticalLine(row + 1, col + cols - 1, rows - 2);
	}

	if (fill && cols > 2) {
		for (int r = row + 1; r < row + rows - 1; r++) {
			OutHorizontalLine(r, col + 1, cols - 2, ' ');
		}
	}

	OutChar(row +        0, col +        0, ACS_ULCORNER);
	OutChar(row +        0, col + cols - 1, ACS_URCORNER);
	OutChar(row + rows - 1, col +        0, ACS_LLCORNER);
	OutChar(row + rows - 1, col + cols - 1, ACS_LRCORNER);
}


/**
 * Draw another buffer on top of this one
 *
 * @param row the row
 * @param col the column
 * @param source the source buffer
 * @param srcRow the source row
 * @param srcCol the source column
 * @param rows the number of rows
 * @param cols the number of columns
 */
void TerminalControlWindow::OutBuffer(int row, int col,
		const TerminalControlWindow* source, int srcRow, int srcCol,
		int rows, int cols)
{
	if (rows == 0 || cols == 0 || srcRow < 0 || srcCol < 0) return;
	if (row < 0 || row >= (int) lines.size()) return;
	if (srcRow < 0 || srcRow >= (int) source->lines.size()) return;

	Line& topLineDst = *lines[row];
	const Line& topLineSrc = *source->lines[srcRow];

	int colsDst = topLineDst.Length();
	int colsSrc = topLineSrc.Length();

	if (rows < 0) rows = source->lines.size();
	if (cols < 0) cols = colsSrc;

	if (col <= -cols || col >= colsDst) return;
	if (srcCol <= -cols || srcCol >= colsSrc || srcRow <= -rows) return;

	if (col < 0) {
		cols += col;
		srcCol -= col;
		col = 0;
	}
	if (col + cols > colsDst) {
		cols = colsDst - col;
	}
	if (srcCol + cols > colsSrc) {
		cols = colsSrc - srcCol;
	}

	if (row < 0) {
		rows += row;
		srcRow -= row;
		row = 0;
	}
	if (row + rows > (int) lines.size()) {
		rows = (int) lines.size() - row;
	}
	if (srcRow + rows > (int) source->lines.size()) {
		rows = (int) source->lines.size() - srcRow;
	}

	if (rows <= 0 || cols <= 0) return;

	for (int r = 0; r < rows; r++) {
		Line& lineDst = *lines[row + r];
		const Line& lineSrc = *source->lines[srcRow + r];
		memcpy(&lineDst[col], &lineSrc[srcCol], sizeof(lineDst[col]) * cols);
	}
}


/**
 * Move the output cursor
 *
 * @param row the row
 * @param col the column
 */
void TerminalControlWindow::SetCursor(int row, int col)
{
	posRow = row;
	posCol = col;
}


/**
 * Write the character onto the buffer (does not wrap)
 *
 * @param c the character
 * @return the number of characters written
 */
int TerminalControlWindow::PutChar(int c)
{
	if (posRow < 0 || posRow >= (int) lines.size()) return 0;
	Line& line = *lines[posRow];

	if (posCol < 0 || posCol >= line.Length()) {
		posCol++;
		return 0;
	}

	Character ch = prototype;
	ch.character = c & 0xff;
	ch.attributes |= c & ~0xff;

	line[posCol++] = ch;
	return 1;
}


/**
 * Write the text onto the buffer (does not wrap)
 *
 * @param str the string
 * @return the number of characters written
 */
int TerminalControlWindow::PutText(const char* str)
{
	int n = OutText(posRow, posCol, str);
	posCol += n;
	return n;
}


/**
 * Create the manager
 */
TerminalControl::TerminalControl()
{
}


/**
 * Destroy the manager
 */
TerminalControl::~TerminalControl()
{
}

