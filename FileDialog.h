/*
 * FileDialog.h
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


#ifndef __FILE_DIALOG_H
#define __FILE_DIALOG_H

#include "DialogWindow.h"
#include "List.h"


/**
 * Common dialog types
 */
enum FileDialogType
{
	FILE_DIALOG_OPEN,
	FILE_DIALOG_SAVE,
};


/**
 * A simple dialog window
 *
 * @param Peter Macko
 */
class FileDialog : public DialogWindow, protected EventHandler
{
	FileDialogType type;

	List<std::string>* fileList;

	Button* okButton;
	Button* cancelButton;

	bool returnValue;


public:
	
	/**
	 * Create an instance of class FileDialog
	 *
	 * @param parent the parent window
	 * @param type the dialog type
	 * @param title the title
	 */
	FileDialog(Window* parent, FileDialogType type, const char* title);

	/**
	 * Destroy the object
	 */
	virtual ~FileDialog(void);

	/**
	 * Display and run the dialog window
	 *
	 * @return true on okay, false on cancel
	 */
	bool Run(void);


protected:

	/**
	 * An event handler for an action
	 *
	 * @param sender the sender
	 */
	virtual void OnAction(Component* sender);
};

#endif
