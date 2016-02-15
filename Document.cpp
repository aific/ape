/*
 * Document.cpp
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
#include "Document.h"

#include <unistd.h>


/**
 * Create an instance of class EditorDocument
 */
EditorDocument::EditorDocument(void)
{
	currentUndo = NULL;
	tabSize = 4;
	
	Clear();
}


/**
 * Destroy the object
 */
EditorDocument::~EditorDocument(void)
{
	if (currentUndo != NULL) delete currentUndo;
	while (!undo.empty()) { delete undo.back(); undo.pop_back(); }
	while (!redo.empty()) { delete redo.back(); redo.pop_back(); }
}


/**
 * Clear the text document
 */
void EditorDocument::Clear(void)
{
	lines.clear();
	displayLengths.Clear();

	DocumentLine l;
	lines.push_back(l);
	UpdateLineMetadata(l);
	displayLengths.Increment(l.displayLength);

	fileName = "";
	
	pageStart = 0;
	modified = false;
	
	cursorRow = 0;
	cursorColumn = 0;
	
	while (!undo.empty()) { delete undo.back(); undo.pop_back(); }
	while (!redo.empty()) { delete redo.back(); redo.pop_back(); }
	
	if (currentUndo != NULL) delete currentUndo;
	currentUndo = NULL;
}


/**
 * Load from file, and set the associated document file name
 *
 * @param file the file name
 * @return a ReturnExt
 */
ReturnExt EditorDocument::LoadFromFile(const char* file)
{
	// Open the file

	FILE* f = fopen(file, "rt");
	if (f == NULL) {
		return ReturnExt(false, "Cannot open the file", errno);
	}


	// Load the lines

	Clear();
	lines.clear();

	ssize_t buffer_length = 1024;
	ssize_t buffer_pos = 0;
	char* buffer = (char*) malloc(buffer_length + 4);
	char c;

	while ((c = fgetc(f)) != EOF) {
	
		if (c == '\r') continue;
		
		if (c == '\n') {
			buffer[buffer_pos++] = '\0';
			buffer_pos = 0;

			DocumentLine l;
			l.str = std::string(buffer);
			UpdateLineMetadata(l);
			lines.push_back(l);
			displayLengths.Increment(l.displayLength);

			continue;
		}

		if (buffer_pos >= buffer_length) {
			char* new_buffer = (char*) malloc(2 * buffer_length + 4);
			memcpy(new_buffer, (const char*) buffer, buffer_length);
			buffer_length *= 2;
			
			free(buffer);
			buffer = new_buffer;
		}
		
		buffer[buffer_pos++] = c;
	}

	buffer[buffer_pos++] = '\0';

	DocumentLine l;
	l.str = std::string(buffer);
	UpdateLineMetadata(l);
	lines.push_back(l);
	displayLengths.Increment(l.displayLength);


	// Finish

	fclose(f);

	modified = false;
	fileName = file;

	return ReturnExt(true);
}


/**
 * Save to file
 *
 * @param file the file name
 * @param switchFile whether to set the associated file name and clear
 *                   the modified flag
 * @return a ReturnExt
 */
ReturnExt EditorDocument::SaveToFile(const char* file, bool switchFile)
{
	// Open the temporary file
	
	char tmp[L_tmpnam];
	if (tmpnam(tmp) == NULL) {
		return ReturnExt(false, "Cannot generate a name for a new "
				"temporary file", errno);
	}

	FILE* f = fopen(tmp, "wt");
	if (f == NULL) {
		return ReturnExt(false, "Error while saving", errno);
	}

	int numLines = NumLines();
	for (int i = 0; i < numLines; i++) {
		const char* l = lines[i].str.c_str();
		if (fputs(l, f) == EOF) {
			fclose(f);
			unlink(tmp);
			return ReturnExt(false, "Error while writing", errno);
		}
		fputc('\n', f);
	}

	fclose(f);

	if (rename(tmp, file) != 0) {
		unlink(tmp);
		return ReturnExt(false, "Error while saving", errno);
	}


	// Switch the associated file and clear the modified flag
	
	if (switchFile) {
		fileName = file;

		if (modified) {
			modified = false;

			// TODO Fix the undo and redo logs instead of clearing them
			
			while (!undo.empty()) { delete undo.back(); undo.pop_back(); }
			while (!redo.empty()) { delete redo.back(); redo.pop_back(); }
			
			if (currentUndo != NULL) delete currentUndo;
			currentUndo = NULL;
		}
	}

	return ReturnExt(true);
}


/**
 * Save to the current file
 *
 * @return a ReturnExt
 */
ReturnExt EditorDocument::Save(void)
{
	if (FileName() == NULL) {
		return ReturnExt(false, "There is no associated file name");
	}

	SaveToFile(fileName.c_str(), true);
	return ReturnExt(true);
}


/**
 * Set the start of the page
 * 
 * @param start the page start line
 */
void EditorDocument::SetPageStart(int start)
{
	if (start >= NumLines()) start = NumLines() - 1;
	if (start < 0) start = 0;
	
	pageStart = start;
}


/**
 * Get the display length of the line
 * 
 * @param line the line string
 * @return the display length
 */
int EditorDocument::DisplayLength(const char* line)
{
	int pos = 0;
	const char* p = line;
	
	while (*p != '\0' && *p != '\n' && *p != '\r') {
		char c = *p;
		
		if (c == '\t') {
			pos = (pos / tabSize) * tabSize + tabSize;
			p++;
		}
		else {
			pos++;
			p++;
		}
	}
	
	return pos;
}


/**
 * Return the maximum display length
 * 
 * @return the maximum display length
 */
int EditorDocument::MaxDisplayLength(void)
{
	return displayLengths.MaxKey();
}


/**
 * Return the string position corresponding to the given cursor position
 * 
 * @param line the line number
 * @param cursor the cursor position
 * @return the string position index
 */
int EditorDocument::StringPosition(int line, int cursor)
{
	int pos = 0, index = 0;
	const char* p = Line(line);
	
	while (*p != '\0' && *p != '\n' && *p != '\r' && pos < cursor) {
		char c = *p;
		
		if (c == '\t') {
			pos = (pos / tabSize) * tabSize + tabSize;
		}
		else {
			pos++;
		}
		
		p++;
		index++;
	}
	
	return index;
}


/**
 * Return the cursor position corresponding to the given string offset
 * 
 * @param line the line number
 * @param cursor the cursor position
 * @return the cursor position, or the maximum position if out of bounds
 */
int EditorDocument::CursorPosition(int line, size_t offset)
{
	int pos = 0;
	size_t index = 0;
	const char* p = Line(line);
	
	while (*p != '\0' && *p != '\n' && *p != '\r' && index < offset) {
		char c = *p;
		
		if (c == '\t') {
			pos = (pos / tabSize) * tabSize + tabSize;
		}
		else {
			pos++;
		}
		
		p++;
		index++;
	}
	
	return pos;
}


/**
 * Update the line metadata
 * 
 * @param l the line object
 */
void EditorDocument::UpdateLineMetadata(DocumentLine& l)
{
	l.displayLength = DisplayLength(l.str.c_str());
}


/**
 * Set the cursor location (used for undo logging)
 * 
 * @param row the row
 * @param column the column
 */
void EditorDocument::SetCursorLocation(int row, int column)
{
	cursorRow = row;
	cursorColumn = column;
}


/**
 * Prepare the document for an edit
 */
void EditorDocument::PrepareEdit(void)
{
	while (!redo.empty()) { delete redo.back(); redo.pop_back(); }
	
	if (currentUndo == NULL) {
		currentUndo = new UndoEntry(this, cursorRow, cursorColumn, modified);
	}
}


/**
 * Append a line
 * 
 * @param line the line
 */
void EditorDocument::Append(const char* line)
{
	PrepareEdit();
	DocumentLine l;
	
	l.str = std::string(line);
	
	UpdateLineMetadata(l);
	lines.push_back(l);
	displayLengths.Increment(l.displayLength);
	
	modified = true;
}


/**
 * Insert a new line
 * 
 * @param pos the line before which to insert
 * @param line the new line
 */
void EditorDocument::Insert(int pos, const char* line)
{
	PrepareEdit();
	DocumentLine l;
	
	l.str = std::string(line);
	
	UpdateLineMetadata(l);
	lines.insert(lines.begin() + pos, 1, l);
	displayLengths.Increment(l.displayLength);
	
	modified = true;
	
	currentUndo->Add(new EA_InsertLine(pos, line));
}


/**
 * Replace a line
 * 
 * @param pos the line to replace
 * @param line the new contents of the line
 */
void EditorDocument::Replace(int pos, const char* line)
{
	PrepareEdit();
	DocumentLine& l = lines[pos];
	displayLengths.Decrement(l.displayLength);
	
	std::string org = l.str;
	l.str = std::string(line);
	
	UpdateLineMetadata(l);
	displayLengths.Increment(l.displayLength);
	
	modified = true;
	
	currentUndo->Add(new EA_ReplaceLine(pos, org.c_str(), line));
}


/**
 * Insert a character to a line
 * 
 * @param line the line
 * @param ch the character to insert
 * @param pos the string position
 */
void EditorDocument::InsertCharToLine(int line, char ch, int pos)
{
	PrepareEdit();
	DocumentLine& l = lines[line];
	displayLengths.Decrement(l.displayLength);
	
	if (pos < 0) pos = 0;
	if (pos > l.str.length()) pos = l.str.length();
	
	l.str.insert(pos, 1, ch);
	UpdateLineMetadata(l);
	displayLengths.Increment(l.displayLength);
	
	modified = true;
	
	currentUndo->Add(new EA_InsertChar(line, pos, ch));
}


/**
 * Delete a character from line
 * 
 * @param line the line
 * @param pos the string position
 */
void EditorDocument::DeleteCharFromLine(int line, int pos)
{
	PrepareEdit();
	DocumentLine& l = lines[line];
	if (l.str.length() == 0) return;
	displayLengths.Decrement(l.displayLength);

	if (pos >= l.str.length()) pos = l.str.length() - 1;
	if (pos < 0) pos = 0;
	
	char ch = l.str[pos];
	
	l.str.erase(pos, 1);
	UpdateLineMetadata(l);
	displayLengths.Increment(l.displayLength);
	
	modified = true;
	
	currentUndo->Add(new EA_DeleteChar(line, pos, ch));
}


/**
 * Join two subsequent lines
 * 
 * @param line the first line to join
 */
void EditorDocument::JoinTwoLines(int line)
{
	PrepareEdit();
	DocumentLine& l = lines[line];
	DocumentLine& l2 = lines[line + 1];
	displayLengths.Decrement(l.displayLength);
	displayLengths.Decrement(l2.displayLength);
	
	std::string org1 = l.str;
	std::string org2 = l2.str;
	
	l.str += l2.str;
	UpdateLineMetadata(l);
	
	lines.erase(lines.begin() + line + 1);
	displayLengths.Increment(l.displayLength);
	
	modified = true;

	currentUndo->Add(new EA_ReplaceLine(line, org1.c_str(), l.str.c_str()));
	currentUndo->Add(new EA_DeleteLine(line + 1, org2.c_str()));
}


/**
 * Just insert a string
 * 
 * @param line the first line
 * @param pos the string position
 * @param str the string to insert
 */
void EditorDocument::InsertStringEx(int line, int pos, const char* str)
{
	DocumentLine& l = lines[line];
	displayLengths.Decrement(l.displayLength);
	
	if (pos < 0) pos = 0;
	if (pos > l.str.length()) pos = l.str.length();
	
	const char* linebreak = std::strchr(str, '\n');
	if (linebreak == NULL) {
		
		l.str = l.str.substr(0, pos) + std::string(str) + l.str.substr(pos);
		UpdateLineMetadata(l);
		displayLengths.Increment(l.displayLength);
	}
	else {
		
		char* buf = (char*) alloca(std::strlen(str) + 1);
		int bi = 0, li = -1;
		
		const char* start = str;
		const char* end = start;
		
		std::string rest = l.str.substr(pos);
		
		while (true) {
			
			if (*end == '\n' || *end == '\0') {
				
				buf[bi++] = '\0';
				bi = 0; li++;
				start = end + 1;
				
				if (li == 0) {
					l.str = l.str.substr(0, pos) + std::string(buf);
					UpdateLineMetadata(l);
					displayLengths.Increment(l.displayLength);
				}
				else if (*end == '\0') {
					DocumentLine nl;
					nl.str = std::string(buf) + rest;
					UpdateLineMetadata(nl);
					lines.insert(lines.begin() + line + li, 1, nl);
					displayLengths.Increment(nl.displayLength);
					break;
				}
				else {
					DocumentLine nl;
					nl.str = std::string(buf);
					UpdateLineMetadata(nl);
					lines.insert(lines.begin() + line + li, 1, nl);
					displayLengths.Increment(nl.displayLength);
				}
			}
			else {
				buf[bi++] = *end;
			}
			
			end++;
		}
	}
}


/**
 * Insert a string
 * 
 * @param line the first line
 * @param pos the string position
 * @param str the string to insert
 */
void EditorDocument::InsertString(int line, int pos, const char* str)
{
	PrepareEdit();
	
	InsertStringEx(line, pos, str);
	
	modified = true;
	currentUndo->Add(new EA_InsertString(line, pos, str));
}


/**
 * Just delete a string
 * 
 * @param line the first line
 * @param pos the string position
 * @param toline the last line
 * @param topos the position within the last line
 */
void EditorDocument::DeleteStringEx(int line, int pos, int toline, int topos)
{
	if (toline < line) {
		int t = toline; toline = line; line = t;
		t = topos; topos = pos; pos = t;
	}
	
	if (toline == line) {
		
		if (topos < pos) {
			int t = topos; topos = pos; pos = t;
		}
		
		if (topos == pos) return;
		
		DocumentLine& l = lines[line];
		displayLengths.Decrement(l.displayLength);
		
		if (pos >= l.str.length()) pos = l.str.length();
		if (pos < 0) pos = 0;
		if (topos >= l.str.length()) topos = l.str.length();
		if (topos < 0) topos = 0;
		
		l.str.erase(pos, topos - pos);
		
		UpdateLineMetadata(l);
		displayLengths.Increment(l.displayLength);
	}
	
	if (toline > line) {
		
		// Get the last line
		
		DocumentLine& ll = lines[toline];
		displayLengths.Decrement(ll.displayLength);
		
		if (topos >= ll.str.length()) topos = ll.str.length();
		if (topos < 0) topos = 0;
		
		
		// Update the first line
		
		DocumentLine& l = lines[line];
		displayLengths.Decrement(l.displayLength);
		
		if (pos >= l.str.length()) pos = l.str.length();
		if (pos < 0) pos = 0;
		
		l.str = l.str.substr(0, pos) + ll.str.substr(topos);
		
		UpdateLineMetadata(l);
		displayLengths.Increment(l.displayLength);
		
		
		// Delete the other lines
		
		for (int i = line; i < toline; i++) {
			DocumentLine& nl = lines[line];
			displayLengths.Decrement(nl.displayLength);
			lines.erase(lines.begin() + line + 1);
		}
	}
}


/**
 * Delete a string
 * 
 * @param line the first line
 * @param pos the string position
 * @param toline the last line
 * @param topos the position within the last line
 */
void EditorDocument::DeleteString(int line, int pos, int toline, int topos)
{
	if (toline < line) {
		int t = toline; toline = line; line = t;
		t = topos; topos = pos; pos = t;
	}
	
	if (toline == line && topos < pos) {
		int t = topos; topos = pos; pos = t;
	}
	
	
	PrepareEdit();
	
	std::string str = Get(line, pos, toline, topos);
	
	DeleteStringEx(line, pos, toline, topos);
	
	modified = true;
	currentUndo->Add(new EA_DeleteString(line, pos, str.c_str()));
}


/**
 * Get a substring
 * 
 * @param line the first line
 * @param pos the string position
 * @param toline the last line
 * @param topos the position within the last line
 */
std::string EditorDocument::Get(int line, int pos, int toline, int topos)
{
	if (toline < line) {
		int t = toline; toline = line; line = t;
		t = topos; topos = pos; pos = t;
	}
	
	if (toline == line) {
		
		if (topos < pos) {
			int t = topos; topos = pos; pos = t;
		}
		
		if (topos == pos) return "";
		
		DocumentLine& l = lines[line];
		
		if (pos >= l.str.length()) pos = l.str.length();
		if (pos < 0) pos = 0;
		
		if (topos >= l.str.length()) topos = l.str.length();
		if (topos < 0) topos = 0;
		
		return l.str.substr(pos, topos - pos);
	}
	
	if (toline > line) {
		
		// Get the first line
		
		DocumentLine& l = lines[line];
		
		if (pos >= l.str.length()) pos = l.str.length();
		if (pos < 0) pos = 0;
		
		std::string s = l.str.substr(pos);
		
		
		// Get the other lines
		
		for (int i = line + 1; i < toline; i++) {
			DocumentLine& nl = lines[i];
			s += "\n" + nl.str;
		}
		
		
		// Get the last line
		
		DocumentLine& ll = lines[toline];
		
		if (topos >= ll.str.length()) topos = ll.str.length();
		if (topos < 0) topos = 0;
		
		return s + "\n" + ll.str.substr(0, topos);
	}
	
	assert(0);
}


/**
 * Create an instance of class UndoEntry
 * 
 * @param _document the document
 * @param _cursorRow the cursor row
 * @param _cursorColumn the cursor column
 * @param _modified the modification status of a document
 */
UndoEntry::UndoEntry(EditorDocument* _document, int _cursorRow, int _cursorColumn, bool _modified)
{
	action = new CompoundEditAction();
	
	document = _document;
	cursorRow = _cursorRow;
	cursorColumn = _cursorColumn;
	modified = _modified;
}


/**
 * Destroy the object
 */
UndoEntry::~UndoEntry(void)
{
	delete action;
}


/**
 * Add an edit action
 * 
 * @param a the action (will be destroyed together with the object)
 */
void UndoEntry::Add(EditAction* a)
{
	action->Add(a);
}


/**
 * Perform an undo
 */
void UndoEntry::Undo(void)
{
	action->Undo(document);
	
	document->cursorRow = cursorRow;
	document->cursorColumn = cursorColumn;
	document->modified = modified;
}


/**
 * Perform a redo
 */
void UndoEntry::Redo(void)
{
	action->Redo(document);
	
	document->cursorRow = redo_cursorRow;
	document->cursorColumn = redo_cursorColumn;
	document->modified = redo_modified;
}


/**
 * Perform an undo
 */
void EditorDocument::Undo(void)
{
	if (currentUndo == NULL) {
		if (undo.empty()) return;
		currentUndo = undo.back();
		undo.pop_back();
	}
	
	currentUndo->Undo();
	
	redo.push_back(currentUndo);
	currentUndo = NULL;
}


/**
 * Perform a redo
 */
void EditorDocument::Redo(void)
{
	if (redo.empty()) return;
	if (currentUndo != NULL) FinalizeEditAction();
	
	currentUndo = redo.back();
	redo.pop_back();
	
	currentUndo->Redo();
	
	undo.push_back(currentUndo);
	currentUndo = NULL;
}


/**
 * Finalize an edit action (for undo purposes)
 */
void EditorDocument::FinalizeEditAction(void)
{
	if (currentUndo == NULL) return;
	
	currentUndo->redo_cursorRow = cursorRow;
	currentUndo->redo_cursorColumn = cursorColumn;
	currentUndo->redo_modified = modified;
	
	undo.push_back(currentUndo);
	currentUndo = NULL;
}
