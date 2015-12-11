/*
 * FileDialog.cpp
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
#include "FileDialog.h"

#include "Manager.h"


/**
 * Create an instance of class FileDialog
 *
 * @param parent the parent window
 * @param type the dialog type
 * @param title the title
 */
FileDialog::FileDialog(Window* parent, FileDialogType type, const char* title)
	: DialogWindow(parent, title)
{
	fileList = new List<Element, ItemRenderer>(this, true /* sorted */, 1, 1,
			ClientRows() - 3, ClientColumns() - 2,
			ANCHOR_ALL);
	fileList->RegisterEventHandler(this);

	okButton = new Button(this, "OK", ClientRows() - 1,
			ClientColumns() - 20, 8, ANCHOR_RIGHT | ANCHOR_BOTTOM);
	okButton->RegisterEventHandler(this);

	cancelButton = new Button(this, "Cancel", ClientRows() - 1,
			ClientColumns() - 10, 8, ANCHOR_RIGHT | ANCHOR_BOTTOM);
	cancelButton->RegisterEventHandler(this);

	DIR *dir = opendir(".");
	if (!dir) {
		perror("opendir");
		exit(EXIT_FAILURE);
	}

	for (;;) {
		struct dirent entry;
		struct dirent *result;

		int error = readdir_r(dir, &entry, &result);
		if (error != 0) {
			perror("readdir");
			exit(EXIT_FAILURE);
		}

		if (result == NULL) break;
		if (strcmp(result->d_name, ".") == 0) continue;
		
		fileList->Add(Element(result));
	}
	closedir(dir);

	Center();
	returnValue = false;
}


/**
 * Destroy the object
 */
FileDialog::~FileDialog(void)
{
}


/**
 * Paint a list item
 *
 * @param list the list component
 * @param tcw the output buffer
 * @param item the item
 * @param active true if the item is active (under cursor)
 * @param selected true if the item is selected
 * @param highlightPattern the highlight pattern
 */
void FileDialog::ItemRenderer::Paint(AbstractList* list,
		TerminalControlWindow* tcw, const FileDialog::Element& item,
		bool active, bool selected, const char* highlightPattern)
{
	std::ostringstream ss;
	ss << item.Entry().d_name;

	switch (item.Entry().d_type) {
		case DT_DIR:
			ss << "/";
			break;
		case DT_LNK:
			ss << "@";
			break;
	}

	::PaintStringListItem(list, tcw, ss.str().c_str(), active, selected,
			highlightPattern);
}


/**
 * Display and run the dialog window
 *
 * @return true on okay, false on cancel
 */
bool FileDialog::Run(void)
{
	returnValue = false;
	wm.Add(this);

	while (Mode() != WM_CLOSED) {
		usleep(20 * 1000);
		wm.ProcessMessages();
	}

	return returnValue;
}


/**
 * An event handler for an action
 *
 * @param sender the sender
 */
void FileDialog::OnAction(Component* sender)
{
	if (sender == okButton) {
		returnValue = true;
		Close();
		return;
	}

	if (sender == cancelButton) {
		returnValue = false;
		Close();
		return;
	}
}

