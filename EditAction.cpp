/*
 * EditAction.cpp
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
#include "EditAction.h"

#include "Document.h"
#include "Histogram.h"


/**
 * Create an instance of class EditAction
 * 
 * @param actionType the action type
 */
EditAction::EditAction(EditActionType actionType)
{
	type = actionType;
}


/**
 * Destroy the object
 */
EditAction::~EditAction(void)
{
	// Nothing to do
}


/**
 * Return a line from a document
 * 
 * @param doc the document
 * @param row the row
 * @return the line
 */
DocumentLine& EditAction::Line(EditorDocument* doc, int row)
{
	return doc->lines[row];
}


/**
 * Return the line display lengths
 * 
 * @param doc the document
 * @return the line length histogram
 */
Histogram& EditAction::DisplayLengths(EditorDocument* doc)
{
	return doc->displayLengths;
}


/**
 * Insert a line and update the appropriate meta-data
 * 
 * @param doc the document
 * @param row the row before which to insert
 * @param contents the line contents
 */
void EditAction::InsertLine(EditorDocument* doc, int row, const char* contents)
{
	DocumentLine l;
	l.SetText(contents);
	
	doc->displayLengths.Increment(l.DisplayLength());
	
	doc->lines.insert(doc->lines.begin() + row, std::move(l));
}


/**
 * Delete a line and update the appropriate meta-data
 * 
 * @param doc the document
 * @param row the row
 */
void EditAction::DeleteLine(EditorDocument* doc, int row)
{
	DocumentLine& l = Line(doc, row);
	doc->displayLengths.Decrement(l.DisplayLength());
	
	doc->lines.erase(doc->lines.begin() + row);
}


/**
 * Insert a string
 * 
 * @param doc the document
 * @param line the first line
 * @param pos the string position
 * @param str the string to insert
 */
void EditAction::InsertString(EditorDocument* doc, int line, int pos, const char* str)
{
	doc->InsertStringEx(line, pos, str);
}


/**
 * Delete a string
 * 
 * @param doc the document
 * @param line the first line
 * @param pos the string position
 * @param toline the last line
 * @param topos the position within the last line
 */
void EditAction::DeleteString(EditorDocument* doc, int line, int pos, int toline, int topos)
{
	doc->DeleteStringEx(line, pos, toline, topos);
}


/**
 * Create an instance of class CharacterEditAction
 * 
 * @param actionType the action type
 * @param _row the row
 * @param _pos the string position
 * @param _ch the character
 */
CharacterEditAction::CharacterEditAction(EditActionType actionType, int _row, int _pos, char _ch) : EditAction(actionType)
{
	row = _row;
	pos = _pos;
	ch = _ch;
}


/**
 * Destroy the object
 */
CharacterEditAction::~CharacterEditAction(void)
{
	// Nothing to do
}


/**
 * Create an instance of class LineEditAction
 * 
 * @param actionType the action type
 * @param _row the row
 * @param _contents the line contents
 */
LineEditAction::LineEditAction(EditActionType actionType, int _row, const char* _contents) : EditAction(actionType)
{
	row = _row;
	contents = strdup(_contents);
}


/**
 * Destroy the object
 */
LineEditAction::~LineEditAction(void)
{
	std::free(contents);
}


/**
 * Create an instance of class StringEditAction
 * 
 * @param actionType the action type
 * @param _row the row
 * @param _pos the position
 * @param _contents the line contents
 */
StringEditAction::StringEditAction(EditActionType actionType, int _row, int _pos, const char* _contents) : EditAction(actionType)
{
	row = _row;
	pos = _pos;
	contents = strdup(_contents);
	
	newlines = 0;
	for (char* c = contents; *c != '\0'; c++) if (*c == '\n') newlines++;
	
	last = std::strrchr(contents, '\n');
	if (last == NULL) last = contents; else last++;
	lastlength = std::strlen(last);
}


/**
 * Destroy the object
 */
StringEditAction::~StringEditAction(void)
{
	std::free(contents);
}


/**
 * Create an instance of class EA_InsertChar
 * 
 * @param actionType the action type
 * @param row the row
 * @param pos the string position
 * @param ch the character
 */
EA_InsertChar::EA_InsertChar(int row, int pos, char ch) : CharacterEditAction(EAT_InsertChar, row, pos, ch)
{
	// Nothing to do
}


/**
 * Destroy the object
 */
EA_InsertChar::~EA_InsertChar(void)
{
	// Nothing to do
}


/**
 * Undo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_InsertChar::Undo(EditorDocument* doc)
{
	DocumentLine& l = Line(doc, row);
	DisplayLengths(doc).Decrement(l.DisplayLength());
	
	std::string s = l.Text();
	s.erase(pos, 1);
	l.SetText(s);
	
	DisplayLengths(doc).Increment(l.DisplayLength());
}


/**
 * Redo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_InsertChar::Redo(EditorDocument* doc)
{
	DocumentLine& l = Line(doc, row);
	DisplayLengths(doc).Decrement(l.DisplayLength());
	
	std::string s = l.Text();
	s.insert(pos, 1, ch);
	l.SetText(s);
	
	DisplayLengths(doc).Increment(l.DisplayLength());
}


/**
 * Create an instance of class EA_DeleteChar
 * 
 * @param actionType the action type
 * @param row the row
 * @param pos the string position
 * @param ch the character
 */
EA_DeleteChar::EA_DeleteChar(int row, int pos, char ch) : CharacterEditAction(EAT_DeleteChar, row, pos, ch)
{
	// Nothing to do
}


/**
 * Destroy the object
 */
EA_DeleteChar::~EA_DeleteChar(void)
{
	// Nothing to do
}


/**
 * Undo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_DeleteChar::Undo(EditorDocument* doc)
{
	DocumentLine& l = Line(doc, row);
	DisplayLengths(doc).Decrement(l.DisplayLength());
	
	std::string s = l.Text();
	s.insert(pos, 1, ch);
	l.SetText(s);
	
	DisplayLengths(doc).Increment(l.DisplayLength());
}


/**
 * Redo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_DeleteChar::Redo(EditorDocument* doc)
{
	DocumentLine& l = Line(doc, row);
	DisplayLengths(doc).Decrement(l.DisplayLength());
	
	std::string s = l.Text();
	s.erase(pos, 1);
	l.SetText(s);
	
	DisplayLengths(doc).Increment(l.DisplayLength());
}


/**
 * Create an instance of class EA_InsertLine
 * 
 * @param row the row
 * @param contents the line contents
 */
EA_InsertLine::EA_InsertLine(int row, const char* contents) : LineEditAction(EAT_InsertLine, row, contents)
{
	// Nothing to do
}


/**
 * Destroy the object
 */
EA_InsertLine::~EA_InsertLine(void)
{
	// Nothing to do
}


/**
 * Undo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_InsertLine::Undo(EditorDocument* doc)
{
	DeleteLine(doc, row);
}


/**
 * Redo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_InsertLine::Redo(EditorDocument* doc)
{
	InsertLine(doc, row, contents);
}


/**
 * Create an instance of class EA_DeleteLine
 * 
 * @param row the row
 * @param contents the line contents
 */
EA_DeleteLine::EA_DeleteLine(int row, const char* contents) : LineEditAction(EAT_DeleteLine, row, contents)
{
	// Nothing to do
}


/**
 * Destroy the object
 */
EA_DeleteLine::~EA_DeleteLine(void)
{
	// Nothing to do
}


/**
 * Undo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_DeleteLine::Undo(EditorDocument* doc)
{
	InsertLine(doc, row, contents);
}


/**
 * Redo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_DeleteLine::Redo(EditorDocument* doc)
{
	DeleteLine(doc, row);
}


/**
 * Create an instance of class EA_ReplaceLine
 * 
 * @param row the row
 * @param _original the original line contents
 * @param contents the new line contents
 */
EA_ReplaceLine::EA_ReplaceLine(int row, const char* _original, const char* contents) : LineEditAction(EAT_ReplaceLine, row, contents)
{
	original = strdup(_original);
}


/**
 * Destroy the object
 */
EA_ReplaceLine::~EA_ReplaceLine(void)
{
	std::free(original);
}


/**
 * Undo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_ReplaceLine::Undo(EditorDocument* doc)
{
	DocumentLine& l = Line(doc, row);
	DisplayLengths(doc).Decrement(l.DisplayLength());
	
	l.SetText(original);
	
	DisplayLengths(doc).Increment(l.DisplayLength());
}


/**
 * Redo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_ReplaceLine::Redo(EditorDocument* doc)
{
	DocumentLine& l = Line(doc, row);
	DisplayLengths(doc).Decrement(l.DisplayLength());
	
	l.SetText(contents);
	
	DisplayLengths(doc).Increment(l.DisplayLength());
}


/**
 * Create an instance of class EA_InsertString
 * 
 * @param actionType the action type
 * @param row the row
 * @param pos the string position
 * @param contents the contents
 */
EA_InsertString::EA_InsertString(int row, int pos, const char* contents) : StringEditAction(EAT_InsertString, row, pos, contents)
{
	// Nothing to do
}


/**
 * Destroy the object
 */
EA_InsertString::~EA_InsertString(void)
{
	// Nothing to do
}


/**
 * Undo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_InsertString::Undo(EditorDocument* doc)
{
	DeleteString(doc, row, pos, row + newlines, newlines == 0 ? pos + lastlength : lastlength);
}


/**
 * Redo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_InsertString::Redo(EditorDocument* doc)
{
	InsertString(doc, row, pos, contents);
}


/**
 * Create an instance of class EA_DeleteString
 * 
 * @param actionType the action type
 * @param row the row
 * @param pos the string position
 * @param contents the contents
 */
EA_DeleteString::EA_DeleteString(int row, int pos, const char* contents) : StringEditAction(EAT_DeleteString, row, pos, contents)
{
	// Nothing to do
}


/**
 * Destroy the object
 */
EA_DeleteString::~EA_DeleteString(void)
{
	// Nothing to do
}


/**
 * Undo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_DeleteString::Undo(EditorDocument* doc)
{
	InsertString(doc, row, pos, contents);
}


/**
 * Redo the action
 * 
 * @param doc the document to which this operation applies to
 */
void EA_DeleteString::Redo(EditorDocument* doc)
{
	DeleteString(doc, row, pos, row + newlines, newlines == 0 ? pos + lastlength : lastlength);
}


/**
 * Create an instance of class CompoundEditAction
 */
CompoundEditAction::CompoundEditAction(void) : EditAction(EAT_Compound)
{
	// Nothing to do
}


/**
 * Destroy the object
 */
CompoundEditAction::~CompoundEditAction(void)
{
	for (int i = 0; i < actions.size(); i++) delete actions[i];
}


/**
 * Add an action
 * 
 * @param a the action (will be destroyed together with this object)
 */
void CompoundEditAction::Add(EditAction* a)
{
	actions.push_back(a);
}


/**
 * Undo the action
 * 
 * @param doc the document to which this operation applies to
 */
void CompoundEditAction::Undo(EditorDocument* doc)
{
	if (actions.empty()) return;
	for (int i = (int) actions.size() - 1; i >= 0; i--) actions[i]->Undo(doc);
}


/**
 * Redo the action
 * 
 * @param doc the document to which this operation applies to
 */
void CompoundEditAction::Redo(EditorDocument* doc)
{
	if (actions.empty()) return;
	for (int i = 0; i < actions.size(); i++) actions[i]->Redo(doc);
}



