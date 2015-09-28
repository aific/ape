/*
 * DialogWindow.h
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


#ifndef __DIALOG_WINDOW_H
#define __DIALOG_WINDOW_H

#include <vector>

#include "Button.h"
#include "Events.h"
#include "Label.h"
#include "Window.h"


/**
 * Common dialog types
 */
enum DialogType
{
	DIALOG_TYPE_NORMAL,
	DIALOG_TYPE_ERROR,
};


/**
 * Common dialog button types
 */
enum DialogButton
{
	DIALOG_BUTTON_OK,
	DIALOG_BUTTON_CANCEL,
};


/**
 * The dialog window
 *
 * @author Peter Macko
 */
class DialogWindow : public Window
{
	Window* parentWindow;


public:
	
	/**
	 * Create an instance of class DialogWindow
	 *
	 * @param parent the parent window
	 * @param title the title
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 */
	DialogWindow(Window* parent, const char* title, int row=1, int col=0,
			int rows=20, int cols=40);

	/**
	 * Destroy the object
	 */
	virtual ~DialogWindow(void);

	/**
	 * Return the parent window
	 *
	 * @return the parent window, or NULL if none
	 */
	inline Window* DialogParent(void) { return parentWindow; }
};


/**
 * A simple dialog window
 *
 * @param Peter Macko
 */
class SimpleDialogWindow : public DialogWindow, protected EventHandler
{
	Label* label;
	std::vector<Button*> buttons;
	std::vector<DialogButton> buttonCodes;

	DialogButton returnCode;


public:
	
	/**
	 * Create an instance of class SimpleDialogWindow
	 *
	 * @param parent the parent window
	 * @param type the dialog type
	 * @param title the title
	 * @param text the text
	 */
	SimpleDialogWindow(Window* parent, DialogType type,
			const char* title, const char* text);

	/**
	 * Destroy the object
	 */
	virtual ~SimpleDialogWindow(void);

	/**
	 * Display and run the dialog window
	 *
	 * @return the clicked button, or DIALOG_BUTTON_CANCEL if the window was
	 *         closed
	 */
	DialogButton Run(void);


protected:

	/**
	 * Initialize the instance
	 *
	 * @param type the dialog type
	 * @param title the title
	 * @param text the text
	 * @param dialogButtons the buttons
	 */
	void Init(DialogType type, const char* title, const char* text,
	          const std::vector<DialogButton>& dialogButtons);

	/**
	 * Get the text of a button
	 *
	 * @param button the button code
	 * @return the text
	 */
	const char* ButtonText(DialogButton button);

	/**
	 * An event handler for an action
	 *
	 * @param sender the sender
	 */
	virtual void OnAction(Component* sender);
};


/**
 * A collection of dialogs
 */
class Dialogs
{

public:

	/**
	 * A simple error dialog
	 *
	 * @param parent the parent window
	 * @param text the text
	 * @param title the title
	 */
	static void Error(Window* parent, const char* text,
			const char* title="Error")
	{
		SimpleDialogWindow* w = new SimpleDialogWindow(parent,
				DIALOG_TYPE_ERROR, title, text);
		w->Run();
	}
};

#endif
