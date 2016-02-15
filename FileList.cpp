/*
 * FileList.cpp
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


#include "stdafx.h"
#include "FileList.h"

#include <sys/stat.h>
#include <libgen.h>
#include "DialogWindow.h"


/**
 * The << operator for FileListItem
 *
 * @param out the output stream
 * @param item the list item
 * @return the stream
 */
std::ostream& operator<< (std::ostream& out, const FileListItem& item)
{
	std::ostringstream ss;
	out << item.Entry().d_name;

	switch (item.Entry().d_type) {
		case DT_DIR:
			out << "/";
			break;
		case DT_LNK:
			out << "@";
			break;
	}

	return out;
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
void FileListItemRenderer::Paint(AbstractList* list,
		TerminalControlWindow* tcw, const FileListItem& item,
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
 * Set the path
 *
 * @param path the path
 * @param inclusive if the path is a directory, true will expand the
 *                  directory, while false will open the parent directory
 *                  and move the cursor to the appropriate entry (the
 *                  argument is ignored if the path is not a directory)
 * @return a ReturnExt
 */
ReturnExt FileList::SetPath(const char* path, bool inclusive)
{
	std::string normalizedPath = NormalizePath(path);
	std::string directory = normalizedPath;

	if (normalizedPath == "") {
		return ReturnExt(false, "Illegal path");
	}


	// Determine if the path is a directory

	struct stat st;
	bool pathIsDirectory = false;
	if (stat(directory.c_str(), &st) == 0) {
		pathIsDirectory = S_ISDIR(st.st_mode);
	}

	std::string select = "";
	if (!pathIsDirectory || (pathIsDirectory && !inclusive)) {
		std::string d = directory + "/..";
		directory = NormalizePath(d.c_str());

		// Get the basename to search for in the directory listing
		const char* p = normalizedPath.c_str() + normalizedPath.length() - 1;
		while (p != normalizedPath.c_str()) {
			if (*p == '/') break;
			p--;
		}
		if (*p == '/') p++;
		select = p;
	}


	// List the directory

	std::vector<FileListItem> contents;

	DIR *dir = opendir(directory.c_str());
	if (!dir) {
		return ReturnExt(false, "Cannot open the directory", errno);
	}

	ssize_t selectIndex = -1;
	for (;;) {
		struct dirent entry;
		struct dirent *result;

		int error = readdir_r(dir, &entry, &result);
		if (error != 0) {
			return ReturnExt(false, "Cannot list the directory", errno);
		}

		if (result == NULL) break;
		if (strcmp(result->d_name, "." ) == 0) continue;
		if (strcmp(result->d_name, "..") == 0 && directory == "/") continue;
		
		contents.push_back(FileListItem(result));

		if (select != "" && select == result->d_name) {
			selectIndex = contents.size() - 1;
		}
	}
	closedir(dir);


	// Set the List contents

	this->directory = directory;
	SetContents(contents);
	if (selectIndex >= 0) SetCursor(Find(contents[selectIndex]));

	return ReturnExt(true);
}


/**
 * An event handler for pressing a key
 * 
 * @param key the key code
 */
void FileList::OnKeyPressed(int key)
{
	if (key == KEY_ENTER || key == KEY_RETURN) {
		
		if (Cursor() >= 0 && Cursor() < Size()) {
			FileListItem& item = (*this)[Cursor()];
			if (item.Entry().d_type == DT_DIR) {
				ReturnExt r;
				if (strcmp(item.Entry().d_name, "..") == 0) {
					r = SetPath(directory.c_str(), false);
				}
				else {
					std::string p = directory + "/";
					p += item.Entry().d_name;
					r = SetPath(p.c_str(), true);
				}
				if (!r) {
					Dialogs::Error(ParentWindow(), r);
				}
			}
			else {
				FireOnAction();
			}
		}

		return;
	}

	List<FileListItem>::OnKeyPressed(key);
}


/**
 * Get the path under the cursor
 *
 * @return the path under the cursor (or "" on error)
 */
std::string FileList::Path()
{
	if (Cursor() >= 0 && Cursor() < Size()) {
		FileListItem& item = (*this)[Cursor()];
		std::string p = directory + "/";
		p += item.Entry().d_name;
		p = NormalizePath(p.c_str());
		return p;
	}
	else {
		return "";
	}
}

