/*
 * WindowSwitcher.h
 *
 * Copyright (c) 2016, Peter Macko
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


#ifndef __WINDOW_SWITCHER_H
#define __WINDOW_SWITCHER_H

#include "DialogWindow.h"
#include "Events.h"
#include "List.h"


/**
 * An element in window list
 */
class WindowSwitcherItem
{
	Window* window;


public:

	/**
	 * Create an instance of WindowSwitcherItem
	 *
	 * @param w the window
	 */
	WindowSwitcherItem(Window* w)
	{
		window = w;
	}

	/**
	 * Destroy the element
	 */
	~WindowSwitcherItem(void) {}

	/**
	 * Get the window
	 *
	 * @return the window
	 */
	inline Window* Value(void) { return window; }

	/**
	 * Get the window
	 *
	 * @return the window
	 */
	inline const Window* Value(void) const { return window; }

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	inline bool operator== (const WindowSwitcherItem& other) const
	{
		return window == other.window;
	}

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	inline bool operator!= (const WindowSwitcherItem& other) const
	{
		return window != other.window;
	}

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	bool operator< (const WindowSwitcherItem& other) const
	{
		int r = strcmp(window->Title(), other.window->Title());
		if (r != 0) return r < 0;
		return (long) window < (long) other.window;
	}

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	inline bool operator<= (const WindowSwitcherItem& other) const
	{
		return *this < other || *this == other;
	}

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	inline bool operator> (const WindowSwitcherItem& other) const
	{
		return !(*this <= other);
	}

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	inline bool operator>= (const WindowSwitcherItem& other) const
	{
		return !(*this < other);
	}
};


/**
 * The << operator for WindowSwitcherItem
 *
 * @param out the output stream
 * @param item the list item
 * @return the stream
 */
inline std::ostream& operator<< (std::ostream& out, const WindowSwitcherItem& item)
{
	out << item.Value()->Title();
	return out;
}


/**
 * The window switcher or list
 *
 * @author Peter Macko
 */
class WindowSwitcher : public DialogWindow, protected EventHandler
{
	List<WindowSwitcherItem>* windowList;
	bool transient;


public:
	
	/**
	 * Create an instance of class WindowSwitcher
	 *
	 * @param transient true for a transient list that disappears after a timeout
	 */
	WindowSwitcher(bool transient=false);

	/**
	 * Destroy the object
	 */
	virtual ~WindowSwitcher(void);

	/**
	 * Determine if the switcher is transient
	 *
	 * @return true if it is transient
	 */
	inline bool Transient() const { return transient; }


protected:

	/**
	 * An event handler for an action
	 *
	 * @param sender the sender
	 */
	virtual void OnAction(Component* sender);

	/**
	 * An event handler for pressing a key
	 *
	 * @param key the key code
	 */
	virtual void OnKeyPressed(int key);
};

#endif
