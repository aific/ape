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

	DIR *dir = opendir(path);
	if (!dir) {
		return ReturnExt(false, "Cannot open the directory");
	}

	for (;;) {
		struct dirent entry;
		struct dirent *result;

		int error = readdir_r(dir, &entry, &result);
		if (error != 0) {
			return ReturnExt(false, "Cannot list the directory");
		}

		if (result == NULL) break;
		if (strcmp(result->d_name, ".") == 0) continue;
		
		Add(FileListItem(result));
	}
	closedir(dir);

	this->path = normalizedPath;

	return ReturnExt(true);
}

