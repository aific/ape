/*
 * EditorWindow.cpp
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
#include "EditorWindow.h"

#include <libgen.h>

#include "DialogWindow.h"
#include "FileDialog.h"
#include "Manager.h"
#include "ScrollBar.h"


/**
 * Create an instance of class EditorWindow
 *
 * @param row the initial row
 * @param col the initial column
 * @param rows the number of rows
 * @param cols the number of columns
 */
EditorWindow::EditorWindow(int row, int col, int rows, int cols)
	: Window("Untitled", row, col, rows, cols, 4, 7)
{
	editor = NULL;
	
	inactiveFrameColor = 6;


	// Add scroll bars
	
	AddScrollBar(true, 16, 3);
	AddScrollBar(false, 2, 2);
	
	HorizScrollBar()->SetBGColor(6);
	HorizScrollBar()->SetFGColor(4);
	VertScrollBar()->SetBGColor(6);
	VertScrollBar()->SetFGColor(4);


	// Add the split pane
	
	splitPane = new SplitPane(this, SPLITPANE_HORIZONTAL, 0, 0, ClientRows(),
			ClientColumns());
	splitPane->SetSplit(ClientRows() - 3);
	splitPane->SetMainComponent(SPLITPANE_COMPONENT_FIRST);
	splitPane->SetOneComponentMode(SPLITPANE_COMPONENT_FIRST);
	
	
	// Add the editor
	
	editor = new Editor(splitPane, true, 0, 0, splitPane->Rows(),
			splitPane->Columns(), ANCHOR_ALL);
	editor->SetBGColor(BGColor());
	editor->SetFGColor(FGColor());
	editor->SetScrollBars(HorizScrollBar(), VertScrollBar());


	// Add the tools pane
	
	toolContainer = new Container(splitPane);
	toolContainer->SetCapturesFocus(true);

	Label* findLabel = new Label(toolContainer, "Find:", 0, 1, 1, 8);
	findLabel->SetAlignment(ALIGN_RIGHT);

	int e = findLabel->Column() + findLabel->Columns() + 1;
	int r = findLabel->Row();
	patternEditor = new Editor(toolContainer, false, r, e, 1,
			toolContainer->ClientColumns() - e - 1 - 3 * (3 + 1),
			ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT);
	patternEditor->RegisterEventHandler(this);

	/*ignoreCaseCheckBox = new CheckBox(toolContainer, "Ignore Case", false,1,1);
	regexCheckBox = new CheckBox(toolContainer, "Regular Expression", false, 1,
			ignoreCaseCheckBox->Column() + ignoreCaseCheckBox->Columns() + 2);*/

	e = patternEditor->Column() + patternEditor->Columns() + 1;

	searchPrevButton = new Button(toolContainer, "<", r, e, -1,
			ANCHOR_TOP | ANCHOR_RIGHT);
	searchPrevButton->RegisterEventHandler(this);
	e += searchPrevButton->Columns() + 1;

	searchNextButton = new Button(toolContainer, ">", r, e, -1,
			ANCHOR_TOP | ANCHOR_RIGHT);
	searchNextButton->RegisterEventHandler(this);
	e += searchNextButton->Columns() + 1;

	searchCloseButton = new Button(toolContainer, "X", r, e, -1,
			ANCHOR_TOP | ANCHOR_RIGHT);
	searchCloseButton->RegisterEventHandler(this);
	e += searchCloseButton->Columns() + 1;

	toolContainer->SetMinSize(2, 60);
}


/**
 * Destroy the object
 */
EditorWindow::~EditorWindow(void)
{
}


/**
 * Load from file, and set the associated document file name
 *
 * @param file the file name
 * @param a ReturnExt
 */
ReturnExt EditorWindow::LoadFromFile(const char* file)
{
	ReturnExt r = editor->LoadFromFile(file);
	if (!r) return r;

	char* s = strdup(file);
	char* b = basename(s);
	SetTitle(b);
	free(s);

	Paint();
	
	return ReturnExt(true);
}


/**
 * Paint the window status
 */
void EditorWindow::PaintEditorStatus(void)
{
	char buf[256];
	int r = Rows() - 1;
	
	if (editor == NULL) return;

	UseFrameStyle();

	
	// Clear the space
	
	tcw->OutHorizontalLine(r, 1, 14);
	
	
	// Print the cursor location
	
	snprintf(buf, sizeof(buf), " %d:%d ", editor->DocumentCursorRow(),
			 editor->DocumentCursorColumn());
	tcw->OutText(r, 9 - digits(editor->DocumentCursorRow()), buf);
	
	
	// Print the flags
	
	if (editor->OverwriteMode()) tcw->OutChar(r, 2, 'O');
	if (editor->Document()->Modified()) tcw->OutChar(r, 3, '*');
}


/**
 * An event handler for pressing a key
 *
 * @param key the key code
 */
void EditorWindow::OnKeyPressed(int key)
{
	if (key == KEY_ESC) {

		if (splitPane->OneComponentMode() == SPLITPANE_COMPONENT_NONE) {

			// Close the find dialog

			splitPane->SetOneComponentMode(SPLITPANE_COMPONENT_FIRST);
			editor->SetHighlightPattern(NULL);
		}
	}

	else if (key == KEY_CTRL('f')) {

		if (splitPane->OneComponentMode() == SPLITPANE_COMPONENT_NONE) {

			// Focus the find dialog

			patternEditor->Focus();
			patternEditor->SelectAll();
		}
		else {

			// Open the find dialog

			splitPane->SetOneComponentMode(SPLITPANE_COMPONENT_NONE);
			editor->SetHighlightPattern(patternEditor->Document()->Line(0));
			patternEditor->Focus();
		}
	}

	else if (key == KEY_CTRL('d')) {

		if (editor->HighlightPattern() != NULL) {
			editor->FindNext(false, false);
		}
	}

	else if (key == KEY_CTRL('g')) {

		if (editor->HighlightPattern() != NULL) {
			editor->FindNext(true, false);
		}
	}

	else if (key == KEY_CTRL('s')) {

		if (editor->Document()->FileName() == NULL) {
			// XXX
			ReturnExt r = editor->Document()->Save();
			Dialogs::Error(this, r.Message());
		}
		else {
			ReturnExt r = editor->Document()->Save();
			if (!r) Dialogs::Error(this, r.Message());
		}
	}

	else {

		Window::OnKeyPressed(key);
	}
}


/**
 * An event handler for an action
 *
 * @param sender the sender
 */
void EditorWindow::OnAction(Component* sender)
{
	if (sender == patternEditor) {
		if (editor->HighlightPattern() != NULL) {
			editor->FindNext(true, true);
		}
		editor->Focus();
	}

	else if (sender == searchPrevButton) {
		if (editor->HighlightPattern() != NULL) {
			editor->FindNext(false, false);
		}
	}

	else if (sender == searchNextButton) {
		if (editor->HighlightPattern() != NULL) {
			editor->FindNext(false, false);
		}
	}

	else if (sender == searchCloseButton) {
		if (splitPane->OneComponentMode() == SPLITPANE_COMPONENT_NONE) {
			splitPane->SetOneComponentMode(SPLITPANE_COMPONENT_FIRST);
			editor->SetHighlightPattern(NULL);
		}
	}
}


/**
 * An event handler for value changed
 *
 * @param sender the sender
 */
void EditorWindow::OnValueChanged(Component* sender)
{
	if (sender == patternEditor) {
		editor->SetHighlightPattern(patternEditor->Document()->Line(0));
		editor->FindNext(true, true);
	}
}


/**
 * Paint the contents of the window
 */
void EditorWindow::Paint(void)
{
	if (!Visible()) return;

	Container::Paint();
	
	
	// Paint the editor status
	
	PaintEditorStatus();
}


/**
 * Refresh the component
 */
void EditorWindow::Refresh(void)
{
	// Is this a good idea???

	PaintEditorStatus();
	
	Component::Refresh();
}


