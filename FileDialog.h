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

#include <dirent.h>

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
	/**
	 * An element in the file dialog
	 */
	class Element
	{
		struct dirent entry;
	
	public:

		/**
		 * Create an instance of Element
		 *
		 * @param e the directory entry
		 */
		Element(struct dirent* e)
		{
			memcpy(&entry, e, sizeof(entry));
		}

		/**
		 * Destroy the element
		 */
		~Element(void) {}

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
		inline bool operator== (const Element& other) const
		{
			return strcmp(entry.d_name, other.entry.d_name) == 0;
		}

		/**
		 * Compare two elements
		 *
		 * @param other the other element
		 * @return true if the condition is satisfied
		 */
		inline bool operator!= (const Element& other) const
		{
			return strcmp(entry.d_name, other.entry.d_name) != 0;
		}

		/**
		 * Compare two elements
		 *
		 * @param other the other element
		 * @return true if the condition is satisfied
		 */
		bool operator< (const Element& other) const
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
		inline bool operator<= (const Element& other) const
		{
			return *this < other || *this == other;
		}

		/**
		 * Compare two elements
		 *
		 * @param other the other element
		 * @return true if the condition is satisfied
		 */
		inline bool operator> (const Element& other) const
		{
			return !(*this <= other);
		}

		/**
		 * Compare two elements
		 *
		 * @param other the other element
		 * @return true if the condition is satisfied
		 */
		inline bool operator>= (const Element& other) const
		{
			return !(*this < other);
		}
	};


	/**
	 * The item renderer
	 */
	class ItemRenderer : ListItemRenderer<Element>
	{

	public:

		/**
		 * Create an instance of class ItemRenderer
		 */
		inline ItemRenderer() {}

		/**
		 * Destroy an instance of class ItemRenderer
		 */
		virtual ~ItemRenderer() {}

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
				const Element& item, bool active, bool selected,
				const char* highlightPattern);
	};


	FileDialogType type;

	List<Element, ItemRenderer>* fileList;

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
