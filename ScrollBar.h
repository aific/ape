/*
 * ScrollBar.h
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


#ifndef __SCROLL_BAR_H
#define __SCROLL_BAR_H

class Component;


/**
 * The scroll bar
 *
 * @author Peter Macko
 */
class ScrollBar
{
	Component* component;
	bool horiz;
	
	int min, max;
	int pos, psize;
	
	int row, col, length;
	
	int bg, fg;
	int barbg, barfg;
	
	
public:
	
	/**
	 * Create an instance of class ScrollBar
	 * 
	 * @param component the parent component
	 * @param horiz whether the scroll bar is horizontal
	 */
	ScrollBar(Component* component, bool horiz);

	/**
	 * Destroy the object
	 */
	virtual ~ScrollBar(void);
	
	/**
	 * Set the range of the scrollbar
	 * 
	 * @param min the minimum
	 * @param max the maximum
	 */
	void SetRange(int min, int max);
	
	/**
	 * Set the location of the scrollbar
	 * 
	 * @param row the row
	 * @param col the col
	 */
	void SetLocation(int row, int col);
	
	/**
	 * Set the length of the scrollbar
	 * 
	 * @param len the new length
	 */
	void SetLength(int len);
	
	/**
	 * Set the position and repaint the bar
	 * 
	 * @param pos the new position
	 */
	void SetPosition(int pos);
	
	/**
	 * Set the position and the position bar size, and then repaint the bar
	 * 
	 * @param pos the new position
	 * @param size the new size
	 */
	void SetPosition(int pos, int size);
	
	/**
	 * Paint the scroll bar
	 */
	void Paint(void);
};

#endif
