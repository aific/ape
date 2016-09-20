/*
 * ScrollBar.cpp
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
#include "ScrollBar.h"

#include "Component.h"
#include "Manager.h"


/**
 * Create an instance of class ScrollBar
 * 
 * @param _component the parent component
 * @param _horiz whether the scroll bar is horizontal
 */
ScrollBar::ScrollBar(Component* _component, bool _horiz)
{
	component = _component;
	horiz = _horiz;
	
	min = 0;
	max = 100;
	pos = 0;
	psize = 1;
	
	row = 0;
	col = 0;
	length = 3;
	
	bg = 0;
	fg = 6;
	barbg = 0;
	barfg = 6;
}


/**
 * Destroy the object
 */
ScrollBar::~ScrollBar(void)
{
	// Nothing to do
}


/**
 * Set the range of the scrollbar
 * 
 * @param _min the minimum
 * @param _max the maximum
 */
void ScrollBar::SetRange(int _min, int _max)
{
	min = _min;
	max = _max;
	
	if (min > max) {
		min = _max;
		max = _min;
	}
}


/**
 * Set the location of the scrollbar
 * 
 * @param _row the row
 * @param _col the col
 */
void ScrollBar::SetLocation(int _row, int _col)
{
	row = _row;
	col = _col;
}


/**
 * Set the length of the scrollbar
 * 
 * @param len the new length
 */
void ScrollBar::SetLength(int len)
{
	length = len < 2 ? 2 : len;
}


/**
 * Paint the scroll bar
 */
void ScrollBar::Paint(void)
{
	TerminalControlWindow* tcw = component->TcwBuffer();


	// Calculate the bar position and size
	
	int bsize = (int)(((length - 2) * psize) / (double) (max - min + 1) + 0.4999);
	if (bsize > (length - 2)) bsize = length - 2;
	if (bsize <= 0) bsize = 1;
	
	int bpos = (int)(((length - 2) * pos) / (double) (max - min + 1) + 0.4999);
	
	
	// Paint the scroll bar
	
	tcw->SetColor(bg, fg);
	
	if (horiz) {
		tcw->OutChar(row, col, ACS_LARROW);
		tcw->OutChar(row, col + length - 1, ACS_RARROW);
		tcw->OutHorizontalLine(row, col + 1, length - 2, ' ');
	}
	else {
		tcw->OutChar(row, col, ACS_UARROW);
		tcw->OutChar(row + length - 1, col, ACS_DARROW);
		tcw->OutVerticalLine(row + 1, col, length - 2, ' ');
	}
	
	
	// Paint the position bar

	if (length > 2) {
		tcw->SetColor(barbg, barfg);

		if (horiz) {
			tcw->OutHorizontalLine(row, col + 1 + bpos, bsize, ACS_CKBOARD);
		}
		else {
			tcw->OutVerticalLine(row + 1 + bpos, col, bsize, ACS_CKBOARD);
		}
	}
}


/**
 * Set the position and repaint the bar
 * 
 * @param _pos the new position
 */
void ScrollBar::SetPosition(int _pos)
{
	pos = _pos;
	
	if (pos < min) pos = min;
	if (pos + psize > max + 1) pos = max + 1 - psize;
	
	Paint();
}


/**
 * Set the position and the position bar size, and then repaint the bar
 * 
 * @param _pos the new position
 * @param size the new size
 */
void ScrollBar::SetPosition(int _pos, int size)
{
	pos = _pos;
	psize = size;
	
	if (pos < min) pos = min;
	if (psize < 1) psize = 1;
	
	if (pos + psize > max + 1) {
		pos = max + 1 - psize;
		if (pos < min) {
			pos = min;
			psize = max - min + 1;
		}
	}
	
	Paint();
}


/**
 * Set the background color
 *
 * @param color the new background color
 */
void ScrollBar::SetBGColor(int color)
{
	bg = color;
	barbg = color;
}


/**
 * Set the foreground color
 *
 * @param color the new foreground color
 */
void ScrollBar::SetFGColor(int color)
{
	fg = color;
	barfg = color;
}

