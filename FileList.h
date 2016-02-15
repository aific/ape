/*
 * FileList.h
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


#ifndef __FILE_LIST_H
#define __FILE_LIST_H

#include <dirent.h>

#include "List.h"


/**
 * An element in file list
 */
class FileListItem
{
	struct dirent entry;


public:

	/**
	 * Create an instance of FileListItem
	 *
	 * @param e the directory entry
	 */
	FileListItem(struct dirent* e)
	{
		memcpy(&entry, e, sizeof(entry));
	}

	/**
	 * Destroy the element
	 */
	~FileListItem(void) {}

	/**
	 * Get the directory entry
	 *
	 * @return the directory entry
	 */
	inline const struct dirent& Entry(void) const { return entry; }

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	inline bool operator== (const FileListItem& other) const
	{
		return strcmp(entry.d_name, other.entry.d_name) == 0;
	}

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	inline bool operator!= (const FileListItem& other) const
	{
		return strcmp(entry.d_name, other.entry.d_name) != 0;
	}

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	bool operator< (const FileListItem& other) const
	{
		// . comes first

		if (strcmp(entry.d_name, ".") == 0) {
			return strcmp(other.entry.d_name, ".") != 0;
		}
		if (strcmp(other.entry.d_name, ".") == 0) return false;


		// .. comes second

		if (strcmp(entry.d_name, "..") == 0) {
			return strcmp(other.entry.d_name, "..") != 0;
		}
		if (strcmp(other.entry.d_name, "..") == 0) return false;


		// Then directories

		if (entry.d_type == DT_DIR) {
			if (other.entry.d_type != DT_DIR) return true;
		}
		else if (other.entry.d_type == DT_DIR) return false;


		// Finally, compare by the file name

		return strcmp(entry.d_name, other.entry.d_name) < 0;
	}

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	inline bool operator<= (const FileListItem& other) const
	{
		return *this < other || *this == other;
	}

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	inline bool operator> (const FileListItem& other) const
	{
		return !(*this <= other);
	}

	/**
	 * Compare two elements
	 *
	 * @param other the other element
	 * @return true if the condition is satisfied
	 */
	inline bool operator>= (const FileListItem& other) const
	{
		return !(*this < other);
	}
};


/**
 * The << operator for FileListItem
 *
 * @param out the output stream
 * @param item the list item
 * @return the stream
 */
std::ostream& operator<< (std::ostream& out, const FileListItem& item);


/**
 * The item renderer
 */
class FileListItemRenderer : public ListItemRenderer<FileListItem>
{

public:

	/**
	 * Create an instance of class FileListItemRenderer
	 */
	inline FileListItemRenderer() {}

	/**
	 * Destroy an instance of class FileListItemRenderer
	 */
	virtual ~FileListItemRenderer() {}

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
	virtual void Paint(AbstractList* list, TerminalControlWindow* tcw,
			const FileListItem& item, bool active, bool selected,
			const char* highlightPattern);
};


/**
 * The file list
 *
 * @author Peter Macko
 */
class FileList : public List<FileListItem>
{
	std::string directory;
	std::string initialDirectory;


protected:
	
	/**
	 * An event handler for pressing a key
	 * 
	 * @param key the key code
	 */
	virtual void OnKeyPressed(int key);


public:
	
	/**
	 * Create an instance of class FileList
	 *
	 * @param parent the parent container
	 * @param row the initial row
	 * @param col the initial column
	 * @param rows the number of rows
	 * @param cols the number of columns
	 * @param anchor set the anchor
	 */
	FileList(Container* parent, int row = 0, int col = 0,
			int rows = 10, int cols = 16,
			int anchor = ANCHOR_LEFT | ANCHOR_TOP)
		: List<FileListItem>(parent, true, row, col, rows, cols, anchor)
	{
		SetRenderer(new FileListItemRenderer(), true);
		SetPath(".", true);
		initialDirectory = directory;
	}

	/**
	 * Destroy the object
	 */
	virtual ~FileList(void) {}

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
	ReturnExt SetPath(const char* path, bool inclusive);

	/**
	 * Get the path under the cursor
	 *
	 * @return the path under the cursor (or "" on error)
	 */
	std::string Path();
};

#endif
