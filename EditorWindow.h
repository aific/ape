/*
 * EditorWindow.h
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


#ifndef __EDITOR_WINDOW_H
#define __EDITOR_WINDOW_H

#include "Window.h"

#include "Editor.h"
#include "Label.h"
#include "SplitPane.h"
#include "Button.h"
#include "CheckBox.h"


/**
 * The text and source code editor
 *
 * @author Peter Macko
 */
class EditorWindow : public Window, protected EventHandler
{
	friend class Editor;

	SplitPane* splitPane;

	Editor* editor;

	Container* toolContainer;
	Editor* patternEditor;
	CheckBox* ignoreCaseCheckBox;
	CheckBox* regexCheckBox;

	Button* searchPrevButton;
	Button* searchNextButton;
	Button* searchCloseButton;


protected:
	
	/**
	 * Paint the window status
	 */
	void PaintEditorStatus(void);

	/**
	 * An event handler for pressing a key
	 *
	 * @param key the key code
	 */
	virtual void OnKeyPressed(int key);

	/**
	 * An event handler for an action
	 *
	 * @param sender the sender
	 */
	virtual void OnAction(Component* sender);

	/**
	 * An event handler for value changed
	 *
	 * @param sender the sender
	 */
	virtual void OnValueChanged(Component* sender);



public:
	
	/**
	 * Create an instance of class EditorWindow
	 *
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 */
	EditorWindow(int row=1, int col=0, int rows=20, int cols=40);

	/**
	 * Destroy the object
	 */
	virtual ~EditorWindow(void);

	/**
	 * Load from file, and set the associated document file name
	 *
	 * @param file the file name
	 */
	void LoadFromFile(const char* file);
	
	/**
	 * Paint the contents of the window
	 */
	virtual void Paint(void);

	/**
	 * Refresh the component
	 */
	virtual void Refresh(void);
};

#endif
