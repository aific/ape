/*
 * TerminalControl.h
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


#ifndef __TERMINAL_CONTROL_H
#define __TERMINAL_CONTROL_H

#include <curses.h>
#include <vector>


/**
 * Terminal control window
 *
 * @author Peter Macko
 */
class TerminalControlWindow
{
	/**
	 * A character
	 */
	struct Character
	{
		char character;
		int attributes;
	};


	/**
	 * A line in the buffer
	 */
	class Line
	{
		std::vector<Character> contents;

	
	public:

		/**
		 * Create a new empty line
		 *
		 * @param character the character
		 * @param length the length
		 */
		Line(const Character& character, int length);

		/**
		 * Get the length of the line
		 *
		 * @return the size
		 */
		inline int Length() const { return contents.size(); }

		/**
		 * Get the given character
		 *
		 * @param index the index
		 * @return the character
		 */
		inline Character& operator[] (int index) { return contents[index]; }

		/**
		 * Get the given character
		 *
		 * @param index the index
		 * @return the character
		 */
		inline const Character& operator[] (int index) const
		{
			return contents[index];
		}

		/**
		 * Clear the line
		 *
		 * @param character the character
		 */
		void Clear(const Character& character);

		/**
		 * Resize the line
		 *
		 * @param character the fill character
		 * @param length the length
		 */
		void Resize(const Character& character, int length);
	};


private:

	bool visible;
	std::vector<Line*> lines;
	std::vector<TerminalControlWindow*> children;

	Character prototype;

	int posRow;
	int posCol;


public:

	/**
	 * Create a new window
	 *
	 * @param rows the number of rows
	 * @param cols the number of columns
	 */
	TerminalControlWindow(int rows, int cols);

	/**
	 * Destroy the window
	 */
	~TerminalControlWindow();

	/**
	 * Return true if the window is visible
	 *
	 * @return true if window is visible
	 */
	inline bool Visible() { return visible; }

	/**
	 * Resize
	 *
	 * @param rows the number of rows
	 * @param cols the number of columns
	 */
	void Resize(int rows, int cols);

	/**
	 * Paint onto the given curses window
	 *
	 * @param win the curses window
	 * @param row the row
	 * @param col the column
	 */
	void Paint(WINDOW* win, int row = 0, int col = 0);

	/**
	 * Clear
	 */
	void Clear();

	/**
	 * Set the color
	 *
	 * @param bg the background color
	 * @param fg the foreground color
	 */
	void SetColor(int bg, int fg);

	/**
	 * Set an attribute
	 *
	 * @param attribute an ncurses attribute
	 * @param value true to turn on, false to turn off
	 */
	void SetAttribute(int attribute, bool value);

	/**
	 * Write the character onto the buffer (does not wrap)
	 *
	 * @param row the row
	 * @param col the column
	 * @param c the character
	 * @return the number of characters written
	 */
	int OutChar(int row, int col, int c);

	/**
	 * Write the text onto the buffer (does not wrap)
	 *
	 * @param row the row
	 * @param col the column
	 * @param str the string
	 * @return the number of characters written
	 */
	int OutText(int row, int col, const char* str);

	/**
	 * Write the text onto the buffer (does not wrap)
	 *
	 * @param row the row
	 * @param col the column
	 * @param format the format
	 * @return the number of characters written
	 */
	int OutFormattedText(int row, int col, const char* format, ...);

	/**
	 * Draw a horizontal line
	 *
	 * @param row the row
	 * @param col the column
	 * @param length the length
	 * @param character the character
	 * @return the number of characters written
	 */
	int OutHorizontalLine(int row, int col, int length,
			int character = ACS_HLINE);

	/**
	 * Draw a vertical line
	 *
	 * @param row the row
	 * @param col the column
	 * @param length the length
	 * @param character the character
	 * @return the number of characters written
	 */
	int OutVerticalLine(int row, int col, int length,
			int character = ACS_VLINE);

	/**
	 * Draw a box
	 *
	 * @param row the row
	 * @param col the column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param fill true to fill
	 */
	void OutBox(int row, int col, int rows, int cols, bool fill = true);

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
	void OutBuffer(int row, int col, const TerminalControlWindow* source,
	               int srcRow=0, int srcCol=0, int rows=-1, int cols=-1);

	/**
	 * Move the output cursor
	 *
	 * @param row the row
	 * @param col the column
	 */
	void SetCursor(int row, int col);

	/**
	 * Write the character onto the buffer (does not wrap)
	 *
	 * @param c the character
	 * @return the number of characters written
	 */
	int PutChar(int c);

	/**
	 * Write the text onto the buffer (does not wrap)
	 *
	 * @param str the string
	 * @return the number of characters written
	 */
	int PutText(const char* str);
};


/**
 * Terminal control manager
 *
 * @author Peter Macko
 */
class TerminalControl
{

public:

	/**
	 * Create the manager
	 */
	TerminalControl();

	/**
	 * Destroy the manager
	 */
	~TerminalControl();
};


/**
 * The global terminal control
 */
extern TerminalControl terminal;

#endif
