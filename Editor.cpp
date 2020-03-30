/*
 * Editor.cpp
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
#include "Editor.h"

#include "Container.h"
#include "Manager.h"
#include "Window.h"


/**
 * Create an instance of class Editor
 *
 * @param parent the parent container
 * @param _multiline true for a multiline editor
 * @param row the initial row
 * @param col the initial column
 * @param rows the number of rows
 * @param cols the number of columns
 * @param anchor set the anchor
 */
Editor::Editor(Container* parent, bool _multiline, int _row, int _col, int rows,
		int cols, int anchor)
	: Component(parent, true, _row, _col, rows, cols, anchor)
{

	multiline = _multiline;


	// Create an empty document
	
	doc = new EditorDocument();

	
	// TODO Move the parser creation somewhere else
	
	Parser* parser = new Parser();
	ParserRule* r;
	
	ParserEnvironment* global = new ParserEnvironment("global", 7);
	parser->AddEnvironment(global);
	
	ParserEnvironment* preprocessor = new ParserEnvironment("preprocessor", 1);
	parser->AddEnvironment(preprocessor);
	
	r = new ParserRule("#", false, preprocessor);
	r->SetMustStartLine(true);
	global->AddRule(r);
	
	r = new ParserRule("", true, NULL);
	r->SetMustEndLine(true);
	preprocessor->AddRule(r);
	
	ParserEnvironment* singleLineComment = new ParserEnvironment("comment-sl", 2);
	parser->AddEnvironment(singleLineComment);
	
	r = new ParserRule("//", false, singleLineComment);
	global->AddRule(r);
	preprocessor->AddRule(r);
	
	r = new ParserRule("", true, NULL);
	r->SetMustEndLine(true);
	singleLineComment->AddRule(r);
	
	ParserEnvironment* multiLineComment = new ParserEnvironment("comment-ml", 2);
	parser->AddEnvironment(multiLineComment);
	
	r = new ParserRule("/*", false, multiLineComment);
	global->AddRule(r);
	preprocessor->AddRule(r);
	
	r = new ParserRule("*/", true, NULL);
	multiLineComment->AddRule(r);
	
	ParserEnvironment* stringLiteral = new ParserEnvironment("string", 5);
	parser->AddEnvironment(stringLiteral);
	
	r = new ParserRule("\"", false, stringLiteral);
	global->AddRule(r);
	
	r = new ParserRule("", true, NULL);	// Handle unterminated literals
	r->SetMustEndLine(true);
	stringLiteral->AddRule(r);
	
	r = new ParserRule("\"", true, NULL);
	stringLiteral->AddRule(r);
	
	ParserEnvironment* characterLiteral = new ParserEnvironment("character", 5);
	parser->AddEnvironment(characterLiteral);
	
	r = new ParserRule("\'", false, characterLiteral);
	global->AddRule(r);
	
	r = new ParserRule("", true, NULL);	// Handle unterminated literals
	r->SetMustEndLine(true);
	characterLiteral->AddRule(r);
	
	r = new ParserRule("\'", true, NULL);
	characterLiteral->AddRule(r);
	
	ParserEnvironment* reservedWord = new ParserEnvironment("reserved", 6);
	parser->AddEnvironment(reservedWord);
	
	static const char* RESERVED_WORDS[] = { "alignas",
	"alignof",
	"and",
	"and_eq",
	"asm",
	"atomic_cancel",
	"atomic_commit",
	"atomic_noexcept",
	"auto",
	"bitand",
	"bitor",
	"bool",
	"break",
	"case",
	"catch",
	"char",
	"char16_t",
	"char32_t",
	"class",
	"compl",
	"concept",
	"const",
	"constexpr",
	"const_cast",
	"continue",
	"co_await",
	"co_return",
	"co_yield",
	"decltype",
	"default",
	"delete",
	"do",
	"double",
	"dynamic_cast",
	"else",
	"enum",
	"explicit",
	"export",
	"extern",
	"false",
	"float",
	"for",
	"friend",
	"goto",
	"if",
	"import",
	"inline",
	"int",
	"long",
	"module",
	"mutable",
	"namespace",
	"new",
	"noexcept",
	"not",
	"not_eq",
	"nullptr",
	"operator",
	"or",
	"or_eq",
	"private",
	"protected",
	"public",
	"reflexpr",
	"register",
	"reinterpret_cast",
	"requires",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"static_assert",
	"static_cast",
	"struct",
	"switch",
	"synchronized",
	"template",
	"this",
	"thread_local",
	"throw",
	"true",
	"try",
	"typedef",
	"typeid",
	"typename",
	"union",
	"unsigned",
	"using",
	"virtual",
	"void",
	"volatile",
	"wchar_t",
	"while",
	"xor",
	"xor_eq",
	"override",
	"final",
	"audit",
	"axiom",
	"transaction_safe",
	"transaction_safe_dynamic",
	NULL };
	
	for (int i = 0; RESERVED_WORDS[i] != NULL; i++) {
		r = new ParserRule(RESERVED_WORDS[i], true, reservedWord);
		r->SetWholeWord(true);
		global->AddRule(r);
	}
	
	if (_multiline) {
		doc->SetParser(parser);
	}
	else {
		delete parser;
	}

	
	// Configure the editor

	bg = 6;
	fg = 0;
	
	tabSize = 4;
	displayTabs = true;
	
	overwriteMode = false;
	scrollBarsReflectCursor = false;
	
	
	// Set window properties
	
	if (multiline) {
		SetMinSize(2, 10);
	}
	else {
		SetMinSize(1, 5);
	}
	
	
	// Set the cursor position

	col = 0;
	row = 0;
	colStart = 0;
	
	selection = false;
	highlightPattern = "";
	
	
	// Update the scrollbars
	
	horizScroll = NULL;
	vertScroll = NULL;
	
	
	// Finalize
	
	lastAction = EEAT_None;
	UpdateCursor();
}


/**
 * Destroy the object
 */
Editor::~Editor(void)
{
	if (doc != NULL) delete doc;
}


/**
 * Set the scroll bars
 *
 * @param horiz the horizontal scroll bar
 * @param vert the vertical scroll bar
 */
void Editor::SetScrollBars(ScrollBar* horiz, ScrollBar* vert)
{
	horizScroll = horiz;
	vertScroll = vert;

	if (horizScroll != NULL) {
		horizScroll->SetRange(0, doc->MaxDisplayLength());
	}
	if (vertScroll != NULL) {
		vertScroll->SetRange(0, doc->NumLines() - 1);
	}
	
	if (scrollBarsReflectCursor) {
		if (horizScroll != NULL) {
			horizScroll->SetPosition(actualCol);
		}
		if (vertScroll != NULL) {
			vertScroll->SetPosition(row);
		}
	}
	else {
		if (horizScroll != NULL) {
			horizScroll->SetPosition(colStart, Columns()); // was: - 2
		}
		if (vertScroll != NULL) {
			vertScroll->SetPosition(doc->PageStart(), Rows()); // was: - 2
		}
	}
}


/**
 * Load from file, and set the associated document file name
 *
 * @param file the file name
 * @param a ReturnExt
 */
ReturnExt Editor::LoadFromFile(const char* file)
{
	ReturnExt r = doc->LoadFromFile(file);
	if (!r) return r;

	col = 0;
	row = 0;
	actualCol = 0;
	colStart = 0;
	
	
	// Update the scrollbars
	
	if (horizScroll != NULL) {
		horizScroll->SetRange(0, doc->MaxDisplayLength());
	}
	if (vertScroll != NULL) {
		vertScroll->SetRange(0, doc->NumLines() - 1);
	}
	
	if (scrollBarsReflectCursor) {
		if (horizScroll != NULL) {
			horizScroll->SetPosition(actualCol);
		}
		if (vertScroll != NULL) {
			vertScroll->SetPosition(row);
		}
	}
	else {
		if (horizScroll != NULL) {
			horizScroll->SetPosition(colStart, Columns() - 2);
		}
		if (vertScroll != NULL) {
			vertScroll->SetPosition(doc->PageStart(), Rows() - 2);
		}
	}


	// Paint

	UpdateCursor();
	Paint();

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
ReturnExt Editor::SaveToFile(const char* file, bool switchFile)
{
	return doc->SaveToFile(file);
}


/**
 * Save to the current file
 *
 * @return a ReturnExt
 */
ReturnExt Editor::Save()
{
	return doc->Save();
}


/**
 * Paint a line
 * 
 * @param line the line number
 */
void Editor::PaintLine(int line)
{
	int left = 0;
	int top = 0;
	int ncols = Columns();
	
	assert(line >= doc->PageStart() && line < doc->NumLines()
			&& line < doc->PageStart() + Rows());
	
	//assert(colStart >= 0);
	if (colStart < 0) colStart = 0;

	DocumentLine* objLine = doc->LineObject(line);
	const char* strLine = doc->Line(line);
	size_t lineLength = strlen(strLine);
	tcw->SetCursor(top + line - doc->PageStart(), left);
	
	
	// Initialize
	
	int pos = 0;
	int offset = 0;
	int length = ncols;
	int start = colStart;
	const char* p = strLine;
	int bg = BGColor();
	int fg = FGColor();

	int scrollIndicatorBG = fg;
	int scrollIndicatorFG = bg;


	// TODO If we are in the single-line mode, draw scrolling indicators

	/* if (!multiline && start > 0) {

		wattrset(Handle(),
				COLOR_PAIR(scrollIndicatorBG * 8 + 7 - scrollIndicatorFG));
		waddch(Handle(), ACS_LARROW);

		start++;
		length--;
	}*/
	
	
	// Make sure that the line is parsed, so that we can do syntax highlighting

	Parser* parser = doc->DocumentParser();
	if (parser != NULL && objLine != NULL) {
	
		// Make sure all of the previous lines are also parsed
		// TODO Make more efficient
		
		for (int l = 0; l <= line; l++) {
			if (!doc->LineObject(l)->ValidParse()
				|| (l > 0 && !doc->LineObject(l)->ParserStateFollows(doc->LineObject(l-1)))) {
				for (int i = l; i <= line; i++) {
					parser->Parse(*doc->LineObject(i),
						i == 0 ? NULL : doc->LineObject(i-1));
				}
				break;
			}
		}
	}
	else {
		if (objLine != NULL) {
			objLine->ClearParsing();
		}
	}


	// Figure out the highlighting
	
	std::vector< std::pair<int, int> > charColors(lineLength);
	for (size_t i = 0; i < lineLength; i++) {
		charColors[i].first = BGColor();
		charColors[i].second = FGColor();
	}
	
	if (parser != NULL && objLine != NULL && !objLine->ParserStates().empty()) {
		size_t stateIndex = 0;
		for (size_t i = 0; i < lineLength; i++) {
			while (stateIndex + 1 < objLine->ParserStates().size()
			    && objLine->ParserStates()[stateIndex + 1].first <= i) stateIndex++;
			ParserEnvironment* env = objLine->ParserStates()[stateIndex].second.Environment();
			if (env != NULL) {
				charColors[i].second = env->Color();
			}
		}
	}

	if (highlightPattern != "") {
		size_t l = highlightPattern.length();
		for (const char* s = strstr(p, highlightPattern.c_str());
		     s != NULL;
		     s = strstr(s + l, highlightPattern.c_str())) {
			int offset = s - p;
			bool active = offsetWithinLine >= offset
			           && offsetWithinLine <  offset + l
			           && row    == line;
			for (int i = 0; i < l; i++) {
				size_t x = i + offset;
				charColors[x].first = active ? 5 : 6;
				charColors[x].second = 4;
			}
		}
	}


	// Find the place in the string to start displaying
	
	while (*p != '\0' && *p != '\n' && *p != '\r' && pos < start) {
		
		// Determine whether we are inside a selection
		
		bool insel = false;
		
		if (selection) {
			
			if (selRow == row && selCol < actualCol && row == line) {
				if (pos >= selCol && pos < actualCol) insel = true;
			}
			
			if (selRow == row && selCol >= actualCol && row == line) {
				if (pos < selCol && pos >= actualCol) insel = true;
			}
			
			if (selRow < row && line >= selRow && line <= row) {
				if (line == selRow) insel = pos >= selCol;
				else if (line == row) insel = pos < actualCol;
				else insel = true;
			}
			
			if (selRow > row && line <= selRow && line >= row) {
				if (line == selRow) insel = pos < selCol;
				else if (line == row) insel = pos >= actualCol;
				else insel = true;
			}
		}
		
		
		// Set the appropriate colors
		
		if (insel) {
			bg = 7;
			fg = 4;
		}
		else {
			bg = charColors[offset].first;
			fg = charColors[offset].second;
		}
		
		
		// Paint
		
		char c = *p;
		tcw->SetColor(bg, fg);
		
		if (c == '\t') {
			pos = (pos / tabSize) * tabSize + tabSize;
			if (pos > start) {
				int d = pos - start;
				length -= d;
				while (d --> 0) {
					tcw->PutChar(' ');
				}
			}
		}
		else {
			pos++;
		}

		p++;
		offset++;
	}
	
	
	// Display from that position on
	
	int bytes = 0;
	int pd = pos - start;
	
	while (*p != '\0' && *p != '\n' && *p != '\r' && bytes < length) {
		unsigned char c = *p;
		
		
		// Determine whether we are inside a selection
		
		bool insel = false;
		
		if (selection) {
			
			if (selRow == row && selCol < actualCol && row == line) {
				if (pos >= selCol && pos < actualCol) insel = true;
			}
			
			if (selRow == row && selCol >= actualCol && row == line) {
				if (pos < selCol && pos >= actualCol) insel = true;
			}
			
			if (selRow < row && line >= selRow && line <= row) {
				if (line == selRow) insel = pos >= selCol;
				else if (line == row) insel = pos < actualCol;
				else insel = true;
			}
			
			if (selRow > row && line <= selRow && line >= row) {
				if (line == selRow) insel = pos < selCol;
				else if (line == row) insel = pos >= actualCol;
				else insel = true;
			}
		}
			
			
		// Set the appropriate colors
		
		if (insel) {
			bg = 7;
			fg = 4;
		}
		else {
			bg = charColors[offset].first;
			fg = charColors[offset].second;
		}
		
		
		// Paint the character(s)
		
		if (c == '\t') {
			int k = ((pos) / tabSize) * tabSize + tabSize;
			if (k > length + start) k = length + start;
			if (displayTabs) {
				tcw->SetColor(bg, 6);
				tcw->PutChar(ACS_BULLET);
				//tcw->PutChar(ACS_VLINE);
				//tcw->PutChar(187 | A_ALTCHARSET);
				pos++;
				bytes++;
			}
			tcw->SetColor(bg, fg);
			for (int i = pos; i < k; i++) {
				tcw->PutChar(' ');
				bytes++;
			}
			pos = k;
		}
		else if (c < ' ' || c >= 127) {
			tcw->SetColor(bg, 1);
			tcw->PutChar('?');
			bytes++;
			pos++;
		}
		else {
			tcw->SetColor(bg, fg);
			tcw->PutChar(c);
			bytes++;
			pos++;
		}

		p++;
		offset++;
	}
	
	
	// Fill the rest of the line
	
	bg = BGColor();
	if (selection && selRow < row && line >= selRow && line <  row) bg = 7;
	if (selection && selRow > row && line <  selRow && line >= row) bg = 7;
	
	tcw->SetColor(bg, FGColor());
	for ( ; bytes < length; bytes++) {
		tcw->PutChar(' ');
	}
}


/**
 * Paint the contents of the window
 */
void Editor::Paint(void)
{
	Clear();
	

	// Compute the bounds
	
	int nlines = Rows();
	if (nlines + doc->PageStart() > doc->NumLines())
		nlines = doc->NumLines() - doc->PageStart();
	
	
	// Paint the lines
	
	for (int l = 0; l < nlines; l++) {
		PaintLine(l + doc->PageStart());
	}
}


/**
 * Update the actual cursor position
 */
void Editor::UpdateActualCursorPosition(void)
{
	// Calculate the actual cursor position
	
	int pos = 0;
	int length = Columns();
	const char* p = doc->Line(row);
	bool lastWasTab = false;
	int preTabPos = 0;
	
	while (*p != '\0' && *p != '\n' && *p != '\r' && pos < col) {
		char c = *p;
		
		if (c == '\t') {
			lastWasTab = true;
			preTabPos = pos;
			pos = (pos / tabSize) * tabSize + tabSize;
			p++;
		}
		else {
			lastWasTab = false;
			pos++;
			p++;
		}
	}
	
	if (lastWasTab && pos > col) pos = preTabPos;
	
	actualCol = pos;
	
	
	// Deselect, if necessary
	
	if (selection && selRow == row && selCol == actualCol) selection = false;
}


/**
 * Update the cursor location and refresh
 *
 * @param scroll whether to scroll to make the cursor visible
 */
void Editor::UpdateCursor(bool scroll)
{
	// Scroll, if necessary

	if (scroll) {
	
		if (row < doc->PageStart()) doc->SetPageStart(row);
	
		if (row - doc->PageStart() >= Rows() - 1) {
			doc->SetPageStart(row - Rows() + 1);
		}
	}
	
	
	// Calculate the actual cursor position
	
	int nr = row - doc->PageStart();
	int nc = col - colStart;
	
	if (scroll) {
		assert(nr >= 0 && nr <= Rows());
		assert(row >= doc->PageStart());
		assert(row < doc->NumLines() || doc->NumLines() == 0);
		assert(row < doc->PageStart() + Rows());
	}
	
	UpdateActualCursorPosition();
	
	
	// Scroll to the left, if necessary
	
	if (scroll) {
		if (actualCol < colStart) {
			colStart = actualCol;
			Paint();
		}
	
		if (1 + actualCol - colStart > Columns() - 1) {
			colStart = 1 + actualCol - Columns();
			Paint();
		}
	}
	
	
	// Compute the new column
	
	nc = actualCol - colStart;
	assert(!scroll || (nc >= 0 && nc <= Columns()));
	
	
	// Update the scroll bars
	
	if (scrollBarsReflectCursor) {
		if (horizScroll != NULL) {
			horizScroll->SetPosition(actualCol);
		}
		if (vertScroll != NULL) {
			vertScroll->SetPosition(row);
		}
	}
	else {
		if (horizScroll != NULL) {
			horizScroll->SetPosition(colStart, Columns() - 2);
		}
		if (vertScroll != NULL) {
			vertScroll->SetPosition(doc->PageStart(), Rows() - 2);
		}
	}
	
	
	// Update the screen
	
	if (nr >= 0 && nr < Rows() && nc >= 0 && nc < Columns()) {
		MoveCursor(nr, nc);
	}
	else {
		HideCursor();
	}
	
	doc->SetCursorLocation(row, actualCol);
	
	// If we are inside EditorWindow, PaintEditorStatus() needs to be called
	ParentWindow()->Refresh();
}


/**
 * Move the cursor in the document
 *
 * @param newRow the new row
 * @param newCol the new column
 * @param shift whether the Shift button was held
 */
void Editor::MoveDocumentCursor(int newRow, int newCol, bool shift)
{
	bool needsPaint = false;

	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
		needsPaint = true;
	}
	
	if (selection) needsPaint = true;
	
	
	// Cursor movement and scrolling logic
	
	row = newRow;
	if (row >= doc->NumLines()) row = doc->NumLines() - 1;
	if (row < 0) row = 0;

	col = newCol;
	int len = doc->DisplayLength(row);
	if (col >= len) col = len;
	if (col < 0) col = 0;
	
	
	if (EnsureValidScroll()) needsPaint = true;

	UpdateActualCursorPosition();
	if (needsPaint) Paint();
	UpdateCursor();
}


/**
 * Move the cursor up
 * 
 * @param shift whether the Shift button was held
 */
void Editor::MoveCursorUp(bool shift)
{
	if (row <= 0) return;
	
	bool needsPaint = EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
		needsPaint = true;
	}
	
	if (selection) needsPaint = true;
	
	
	// Cursor movement and scrolling logic
	
	if (doc->PageStart() >= row) {
		row--;
		doc->SetPageStart(row);
		needsPaint = true;
	}
	else if (row - doc->PageStart() >= Rows()) {
		// Do we need this?
		row--;
		//doc->SetPageStart(row - (Rows() - 2) + 1);
		doc->SetPageStart(row - Rows() + 1);
		needsPaint = true;
	}
	else {
		row--;
	}
	
	UpdateActualCursorPosition();
	if (needsPaint) Paint();
	UpdateCursor();
}


/**
 * Move the cursor down
 * 
 * @param shift whether the Shift button was held
 */
void Editor::MoveCursorDown(bool shift)
{
	if (row >= doc->NumLines() - 1) return;
	
	bool needsPaint = EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
		needsPaint = true;
	}
	
	if (selection) needsPaint = true;
	
	
	// Cursor movement and scrolling logic
	
	if (row - doc->PageStart() >= Rows() - 1) {
		row++;
		doc->SetPageStart(row - Rows() + 1);
		needsPaint = true;
	}
	else if (doc->PageStart() >= row + 1) {
		row++;
		doc->SetPageStart(row);
		needsPaint = true;
	}
	else {
		row++;
	}
	
	UpdateActualCursorPosition();
	if (needsPaint) Paint();
	UpdateCursor();
}


/**
 * Move the cursor left
 * 
 * @param shift whether the Shift button was held
 */
void Editor::MoveCursorLeft(bool shift)
{
	if (col <= 0 && row <= 0) return;
	
	bool needsPaint = EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
		needsPaint = true;
	}
	
	if (selection) needsPaint = true;
	
	
	// Cursor movement and scrolling logic
	
	if (col == 0) {
		row--;
		col = doc->DisplayLength(row);
		needsPaint = needsPaint || EnsureValidScroll();
	}
	else {
		col = actualCol - 1;
	}
	
	int idx = doc->StringPosition(row, actualCol);
	const char* line = doc->Line(row);
	if (idx > 0) {
		if (line[idx - 1] == '\t') {
			while (doc->StringPosition(row, col) >= idx) col--;
		}
	}
	
	if (col < colStart) {
		colStart--;
		needsPaint = true;
	}
	
	UpdateActualCursorPosition();
	
	
	// Finalize
	
	if (needsPaint) Paint();
	UpdateCursor();
}


/**
 * Move the cursor right
 * 
 * @param shift whether the Shift button was held
 */
void Editor::MoveCursorRight(bool shift)
{
	bool needsPaint = EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
		needsPaint = true;
	}
	
	if (selection) needsPaint = true;
	
	
	// Cursor movement and scrolling logic
	
	col = actualCol + 1;
	int len = doc->DisplayLength(row);
	//if (col > len) col = len;
	
	if (col > len && row < doc->NumLines() - 1) {
		row++;
		col = 0;
		needsPaint = needsPaint || EnsureValidScroll();
	}
	
	int idx = doc->StringPosition(row, actualCol);
	const char* line = doc->Line(row);
	if (line[idx] == '\t') {
		if (line[idx + 1] == '\0' || line[idx + 1] == '\n' || line[idx + 1] == '\r') {
			col = len;
		}
		else {
			while (doc->StringPosition(row, col) <= idx + 1) col++;
			col--;
		}
	}
	
	if (col - colStart > Columns() - 1) {
		colStart++;
		needsPaint = true;
	}
	
	UpdateActualCursorPosition();
	if (needsPaint) Paint();
	UpdateCursor();
}


/**
 * Determine if the character is a part of a word
 *
 * @param c the character
 * @return true if it is a part of a word
 */
inline bool isWordCharacter(const char c)
{
	return isalnum(c) || c == '_';
}


/**
 * Move the cursor to the beginning of the word or to the previous word
 * 
 * @param shift whether the Shift button was held
 */
void Editor::MoveCursorBeginningWord(bool shift)
{
	if (col <= 0 && row <= 0) return;
	
	bool needsPaint = EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
		needsPaint = true;
	}
	
	if (selection) needsPaint = true;
	
	
	// Cursor movement and scrolling logic
	
	if (col == 0) {
		row--;
		col = doc->DisplayLength(row);
		needsPaint = needsPaint || EnsureValidScroll();
	}
	else {
	
		const char* line = doc->Line(row);
		col = actualCol - 1;
		int idx = doc->StringPosition(row, col);
		
		if (isspace(line[idx])) {
			while (col > 0 && isspace(line[idx])) {
				col--;
				idx = doc->StringPosition(row, col);
			}
		}
		
		if (!isWordCharacter(line[idx])) {
			while (col > 0 && !isWordCharacter(line[idx-1])
					&& !isspace(line[idx-1])) {
				col--;
				idx = doc->StringPosition(row, col);
			}
		}
		else {
			while (col > 0 && isWordCharacter(line[idx-1])) {
				col--;
				idx = doc->StringPosition(row, col);
			}
		}
	}
	
	if (col < colStart) {
		colStart = col;
		needsPaint = true;
	}
	
	UpdateActualCursorPosition();
	
	
	// Finalize
	
	if (needsPaint) Paint();
	UpdateCursor();
}


/**
 * Move the cursor to the end of the word or to the next word
 * 
 * @param shift whether the Shift button was held
 */
void Editor::MoveCursorEndWord(bool shift)
{
	bool needsPaint = EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
		needsPaint = true;
	}
	
	if (selection) needsPaint = true;
	
	
	// Cursor movement and scrolling logic
	
	int len = doc->DisplayLength(row);
	if (actualCol + 1 > len && row < doc->NumLines() - 1) {
		row++;
		col = 0;
		needsPaint = needsPaint || EnsureValidScroll();
	}
	else {
	
		const char* line = doc->Line(row);
		col = actualCol + 1;
		int idx = doc->StringPosition(row, col);
		
		if (isspace(line[idx])) {
			while (isspace(line[idx])) {
				col++;
				idx = doc->StringPosition(row, col);
			}
		}
		
		if (!isWordCharacter(line[idx])) {
			while (line[idx+1] != '\0' && !isWordCharacter(line[idx+1])
					&& !isspace(line[idx+1])) {
				col++;
				idx = doc->StringPosition(row, col);
			}
		}
		else {
			while (line[idx+1] != '\0' && isWordCharacter(line[idx+1])) {
				col++;
				idx = doc->StringPosition(row, col);
			}
		}
		
		col++;
	}
	
	if (col - colStart > Columns() - 1) {
		colStart = col - Columns() + 1;;
		needsPaint = true;
	}
	
	UpdateActualCursorPosition();
	if (needsPaint) Paint();
	UpdateCursor();
}


/**
 * Move the cursor to the very left
 */
void Editor::MoveCursorVeryLeft(void)
{
	bool needsPaint = EnsureValidScroll();
	col = 0;
	
	if (colStart > 0) {
		colStart = 0;
		needsPaint = true;
	}
	
	if (needsPaint) Paint();
	UpdateCursor();
}


/**
 * Move the cursor to the beginning of the line
 * 
 * @param shift whether the Shift button was held
 */
void Editor::MoveCursorToTheBeginningOfTheLine(bool shift)
{
	bool needsPaint = EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
		needsPaint = true;
	}
	
	if (selection) needsPaint = true;
	
	
	// Determine the position of the first non-blank character of the line
	
	int firstNonBlank = 0;
	const char* line = doc->Line(row);
	for (const char* p = line; *p != '\0'; p++) {
		if (*p == ' ') {
			firstNonBlank++;
		}
		else if (*p == '\t') {
			firstNonBlank = (firstNonBlank / tabSize) * tabSize + tabSize;
		}
		else {
			break;
		}
	}
	
	
	// Move the cursor appropriately
	
	if (col == firstNonBlank || actualCol == firstNonBlank) {
		col = 0;
	}
	else {
		col = firstNonBlank;
	}
	
	
	// Take care of scrolling
	
	if (colStart > col) {
		colStart = col;
		needsPaint = true;
	}
	
	if (needsPaint) Paint();
	UpdateCursor();
}


/**
 * Move the cursor to the very right
 * 
 * @param shift whether the Shift button was held
 */
void Editor::MoveCursorVeryRight(bool shift)
{
	bool needsPaint = EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
		needsPaint = true;
	}
	
	if (selection) needsPaint = true;


	// Move the cursor
	
	col = doc->DisplayLength(row);
	
	if (needsPaint) Paint();
	UpdateCursor();
}


/**
 * Move the cursor one page up
 * 
 * @param shift whether the Shift button was held
 */
void Editor::MoveCursorPageUp(bool shift)
{
	EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
	}
	
	
	// Set the new page start
	
	int p = doc->PageStart();
	int d = Rows();

	if (p == 0 && row == 0) {
		MoveCursorVeryLeft();
		return;
	}
	
	if (p - d < 0) d = p;
	
	doc->SetPageStart(p - d);
	
	
	// Move the cursor
	
	if (d < Rows()) {
		row -= Rows() - 1;
		if (doc->PageStart() == 0) row = 0;
		if (row < 0) row = 0;
	}
	else {
		row -= d;
	}
	
	
	// Finish
	
	Paint();
	UpdateCursor();
}


/**
 * Move the cursor one page down
 * 
 * @param shift whether the Shift button was held
 */
void Editor::MoveCursorPageDown(bool shift)
{
	EnsureValidScroll();
	
	
	// Selection logic
	
	if (shift && !selection) {
		selRow = row;
		selCol = actualCol;
		selection = true;
	}
	
	if (!shift && selection) {
		selection = false;
	}
	
	
	// Set the new page start
	
	int p = doc->PageStart();
	int d = Rows();
	
	int m = doc->NumLines() - Rows();
	if (p + d > m) d = m - p;

	if (d == 0 && row == doc->NumLines() - 1) {
		MoveCursorVeryRight();
		return;
	}
	
	doc->SetPageStart(p + d);
	
	
	// Move the cursor
	
	if (d < Rows()) {
		row += Rows() - 2;
		if (d == 0) row++;
		if (doc->PageStart() + Rows() == doc->NumLines()) {
			row = doc->NumLines() - 1;
		}
		if (row > doc->NumLines() - 1) row = doc->NumLines() - 1;
	}
	else {
		row += d;
	}
	
	
	// Finish
	
	Paint();
	UpdateCursor();
}


/**
 * Insert a character at the cursor position
 * 
 * @param c the character
 */
void Editor::InsertChar(char c)
{
	bool white = isspace(c);
	if (white ? (lastAction != EEAT_TypeWhitespace && lastAction != EEAT_Type)
			: lastAction != EEAT_Type) doc->FinalizeEditAction();
	
	bool needsPaint = EnsureValidScroll();
	
	if (selection) {
		doc->FinalizeEditAction();
		needsPaint = true;
		DeleteSelection();
		EnsureValidScroll();
	}
	
	col = actualCol;
	doc->InsertCharToLine(row, c, doc->StringPosition(row, col));
	
	PaintLine(row);
	MoveCursorRight();
	
	lastAction = white ? EEAT_TypeWhitespace : EEAT_Type;
	
	if (needsPaint) {
		UpdateActualCursorPosition();
		Paint();
	}
	
	AfterEdit();
}


/**
 * Insert a new line at the cursor position
 */
void Editor::NewLine(void)
{
	if (!multiline) return;

	if (lastAction != EEAT_Enter) doc->FinalizeEditAction();
	if (selection) DeleteSelection();
	
	std::string lineStr = doc->Line(row);
	const char* line = lineStr.c_str();
	int idx = doc->StringPosition(row, actualCol);
	
	EnsureValidScroll();
	
	
	// Create the new line
	
	char* newline = (char*) alloca(strlen(line) + 4);
	*newline = '\0';
	
	int w = 0;
	for (int i = 0; i < idx; i++) {
		if (isspace(line[i])) {
			newline[i] = line[i];
			newline[i + 1] = '\0';
			
			if (line[i] == ' ') w++;
			if (line[i] == '\t') w = w % tabSize == 0 ? w + tabSize : tabSize * (w / tabSize + 1);
		}
		else break;
	}
	
	strcat(newline, line + idx);
	doc->Insert(row + 1, newline);
	
	
	// Replace the old line
	
	char* nl = (char*) alloca(idx + 4);
	memcpy(nl, line, idx);
	nl[idx] = '\0';
	
	doc->Replace(row, nl);
	
	
	// Update the cursor
	
	row++;
	col = w;
	
	if (row >= doc->PageStart() + Rows() - 2) doc->SetPageStart(doc->PageStart() + 1);
	
	
	// Finalize
	
	lastAction = EEAT_Enter;
	UpdateActualCursorPosition();
	Paint();
	AfterEdit();
}


/**
 * Delete a character at the cursor position
 */
void Editor::DeleteChar(void)
{
	if (lastAction != EEAT_Delete) doc->FinalizeEditAction();
	
	bool needsPaint = EnsureValidScroll();
	
	if (selection) {
		DeleteSelection();
		lastAction = EEAT_Delete;
		UpdateActualCursorPosition();
		EnsureValidScroll();
		Paint();
		lastAction = EEAT_Delete;
		AfterEdit();
		return;
	}
	
	col = actualCol;
	
	if (col == doc->DisplayLength(row)) {
		if (row < doc->NumLines() - 1) {
			doc->JoinTwoLines(row);
			
			EnsureValidScroll();
			Paint();
		}
		else {
			if (needsPaint) Paint();
			return;
		}
	}
	else {
		doc->DeleteCharFromLine(row, doc->StringPosition(row, col));
		
		if (needsPaint) Paint(); else PaintLine(row);
	}
	
	lastAction = EEAT_Delete;
	AfterEdit();
}


/**
 * Delete a character before the cursor position
 */
void Editor::Backspace(void)
{
	if (lastAction != EEAT_Backspace) doc->FinalizeEditAction();
		
	bool needsPaint = EnsureValidScroll();

	if (selection) {
		DeleteSelection();
		UpdateActualCursorPosition();
		EnsureValidScroll();
		Paint();
		lastAction = EEAT_Backspace;
		AfterEdit();
		return;
	}
	
	col = actualCol;
	
	if (col == 0) {
		if (row > 0) {
			
			row--;
			col = doc->DisplayLength(row);
			
			if (row < doc->PageStart()) doc->SetPageStart(row);
			
			doc->JoinTwoLines(row);
			
			EnsureValidScroll();
			Paint();
		}
		else {
			if (needsPaint) Paint();
			return;
		}
	}
	else {
		
		col = col - 1;
		
		int idx = doc->StringPosition(row, actualCol);
		const char* line = doc->Line(row);
		if (idx > 0) {
			if (line[idx - 1] == '\t') {
				while (doc->StringPosition(row, col) >= idx) col--;
			}
		}
		
		doc->DeleteCharFromLine(row, doc->StringPosition(row, col));
		
		if (needsPaint) Paint(); else PaintLine(row);
	}
	
	lastAction = EEAT_Backspace;
	AfterEdit();
}


/**
 * Copy a string to the clipboard
 */
void Editor::Copy(void)
{
	if (!selection) return;
	
	int selIdx = doc->StringPosition(selRow, selCol);
	int idx = doc->StringPosition(row, actualCol);
	
	std::string s = doc->Get(row, idx, selRow, selIdx).c_str();
	wm.SetClipboard(s);
}


/**
 * Cut a string and copy it to the clipboard
 */
void Editor::Cut(void)
{
	if (!selection) return;
	
	EnsureValidScroll();
	
	Copy();
	
	doc->FinalizeEditAction();
	DeleteSelection();
	UpdateActualCursorPosition();
	EnsureValidScroll();
	Paint();
	lastAction = EEAT_Cut;
	AfterEdit();
}


/**
 * Paste a string from the clipboard
 */
void Editor::Paste(void)
{
	if (wm.Clipboard()[0] == '\0') return;
	
	doc->FinalizeEditAction();
	
	
	// Remove the selected string
	
	DeleteSelection();
	
	
	// Paste
	
	const char* str = wm.Clipboard();
	int pos = doc->StringPosition(row, actualCol);
	doc->InsertString(row, pos, str);
	
	
	// Update the cursor location
	
	int newlines = 0;
	for (const char* c = str; *c != '\0'; c++) if (*c == '\n') newlines++;
	row += newlines;
	
	const char* last = std::strrchr(str, '\n');
	if (last == NULL) {
		int idx = doc->StringPosition(row, actualCol);
		const char* line = doc->Line(row);
		std::string s = line;
		DocumentLine l; l.SetText(s.substr(0, idx) + str);
		actualCol = col = l.DisplayLength();
	}
	else {
		last++;
		DocumentLine l; l.SetText(last);
		actualCol = col = l.DisplayLength();
	}
	
	
	// Scroll, if necessary
	
	if (row < doc->PageStart()) doc->SetPageStart(row);
	
	if (row - doc->PageStart() >= Rows() - 3) {
		doc->SetPageStart(row - Rows() + 3);
	}
	
	
	// Finalize the update
	
	lastAction = EEAT_None;
	doc->FinalizeEditAction();
	
	UpdateActualCursorPosition();
	EnsureValidScroll();
	Paint();
	AfterEdit();
}


/**
 * Perform an undo
 */
void Editor::Undo(void)
{
	// Undo
	
	doc->FinalizeEditAction();
	doc->Undo();
	
	
	// Get the updated cursor location
	
	actualCol = col = doc->CursorColumn();
	row = doc->CursorRow();
	selection = false;
	
	
	// Scroll, if necessary
	
	EnsureValidScroll();
	
	
	// Finalize the update
	
	lastAction = EEAT_None;
	Paint();
	AfterEdit();
}


/**
 * Perform a redo
 */
void Editor::Redo(void)
{
	// Redo
	
	doc->Redo();
	
	
	// Get the updated cursor location
	
	actualCol = col = doc->CursorColumn();
	row = doc->CursorRow();
	selection = false;
	
	
	// Scroll, if necessary
	
	EnsureValidScroll();
	
	
	// Finalize the update
	
	lastAction = EEAT_None;
	Paint();
	AfterEdit();
}


/**
 * Select all
 */
void Editor::SelectAll(void)
{
	selection = true;
	selRow = 0;
	selCol = 0;

	row = doc->NumLines() - 1;
	col = doc->DisplayLength(row);
	
	EnsureValidScroll();
	Paint();
	UpdateCursor();
}


/**
 * Perform the necessary operations after an edit
 */
void Editor::AfterEdit(void)
{
	if (horizScroll != NULL) {
		horizScroll->SetRange(0, doc->MaxDisplayLength());
	}
	if (vertScroll != NULL) {
		vertScroll->SetRange(0, doc->NumLines() - 1);
	}
	
	UpdateCursor();
	FireOnValueChanged();
}


/**
 * Delete the selection, if applicable
 */
void Editor::DeleteSelection(void)
{
	if (!selection) return;
	
	int selIdx = doc->StringPosition(selRow, selCol);
	int idx = doc->StringPosition(row, actualCol);
	
	doc->DeleteString(row, idx, selRow, selIdx);
	
	if (selRow < row) { row = selRow; col = actualCol = selCol; }
	if (selRow == row && (actualCol > selCol || col > selCol))
		col = actualCol = selCol;
	
	selection = false;
}


/**
 * Scroll the window such that the cursor is visible
 * 
 * @return true if the scrolling actually happened
 */
bool Editor::EnsureValidScroll(void)
{
	if (!multiline) {
		if (doc->PageStart() == 0) {
			return false;
		}
		else {
			doc->SetPageStart(0);
			return true;
		}
	}

	if (row - doc->PageStart() >= Rows() - 1) {
		doc->SetPageStart(row - Rows() + 1);
		return true;
	}
	
	if (doc->PageStart() >= row + 1) {
		doc->SetPageStart(row);
		return true;
	}

	if (doc->PageStart() > 0 && doc->NumLines() - doc->PageStart() < Rows()) {
		int p = doc->NumLines() - Rows();
		if (p < 0) p = 0;
		doc->SetPageStart(p);
		return true;
	}
	
	return false;
}


/**
 * An event handler for pressing a key
 * 
 * @param key the key code
 */
void Editor::OnKeyPressed(int key)
{

	// Text editing
	
	if ((key >= 0x20 && key <= 0x7E) || (key == 9 && multiline)) {
		InsertChar((char) key);
		return;
	}

	if (key == KEY_ENTER || key == KEY_RETURN) {
		if (multiline) {
			NewLine();
			return;
		}
		else {
			FireOnAction();
			return;
		}
	}
	
	if (key == KEY_DC) {
		DeleteChar();
		return;
	}
	
	if (key == KEY_BACKSPACE || key == 127 || key == 8) {
		Backspace();
		return;
	}
	
	
	// Cursor movement
	
	if (multiline) {

		if (key == KEY_UP) {
			doc->FinalizeEditAction();
			MoveCursorUp();
			return;
		}
		
		if (key == KEY_DOWN) {
			doc->FinalizeEditAction();
			MoveCursorDown();
			return;
		}
	}

	if (key == KEY_LEFT) {
		doc->FinalizeEditAction();
		MoveCursorLeft();
		return;
	}
	
	if (key == KEY_RIGHT) {
		doc->FinalizeEditAction();
		MoveCursorRight();
		return;
	}

	if (key == KEY_ALT_LEFT) {
		doc->FinalizeEditAction();
		MoveCursorBeginningWord();
		return;
	}
	
	if (key == KEY_ALT_RIGHT) {
		doc->FinalizeEditAction();
		MoveCursorEndWord();
		return;
	}
	
	if (key == KEY_HOME) {
		doc->FinalizeEditAction();
		MoveCursorToTheBeginningOfTheLine();
		return;
	}
	
	if (key == KEY_END) {
		doc->FinalizeEditAction();
		MoveCursorVeryRight();
		return;
	}
	
	if (multiline) {
		if (key == KEY_PPAGE || key == KEY_ALT_UP) {
			doc->FinalizeEditAction();
			MoveCursorPageUp();
			return;
		}
		
		if (key == KEY_NPAGE || key == KEY_ALT_DOWN) {
			doc->FinalizeEditAction();
			MoveCursorPageDown();
			return;
		}
		
		if (key == KEY_SHIFT_ALT_UP) {
			doc->FinalizeEditAction();
			MoveCursorPageUp(true);
			return;
		}
		
		if (key == KEY_SHIFT_ALT_DOWN) {
			doc->FinalizeEditAction();
			MoveCursorPageDown(true);
			return;
		}
	}
	
	
	// Cursor movement with selection
	
	if (multiline) {
		if (key == KEY_SHIFT_UP) {
			doc->FinalizeEditAction();
			MoveCursorUp(true);
			return;
		}
		
		if (key == KEY_SHIFT_DOWN) {
			doc->FinalizeEditAction();
			MoveCursorDown(true);
			return;
		}
	}

	if (key == KEY_SHIFT_LEFT) {
		doc->FinalizeEditAction();
		MoveCursorLeft(true);
		return;
	}
	
	if (key == KEY_SHIFT_RIGHT) {
		doc->FinalizeEditAction();
		MoveCursorRight(true);
		return;
	}

	if (key == KEY_SHIFT_ALT_LEFT) {
		doc->FinalizeEditAction();
		MoveCursorBeginningWord(true);
		return;
	}
	
	if (key == KEY_SHIFT_ALT_RIGHT) {
		doc->FinalizeEditAction();
		MoveCursorEndWord(true);
		return;
	}
	
	if (key == KEY_SHIFT_HOME) {
		doc->FinalizeEditAction();
		MoveCursorToTheBeginningOfTheLine(true);
		return;
	}
	
	if (key == KEY_SHIFT_END) {
		doc->FinalizeEditAction();
		MoveCursorVeryRight(true);
		return;
	}
	
	
	// Select all
	
	if (key == KEY_CTRL('a')) {
		doc->FinalizeEditAction();
		SelectAll();
		return;
	}
	
	
	// Copy and paste
	
	if (key == KEY_CTRL('c')) {
		Copy();
		return;
	}
	
	if (key == KEY_CTRL('v')) {
		Paste();
		return;
	}
	
	if (key == KEY_CTRL('x')) {
		Cut();
		return;
	}
	
	
	// Commands
	
	if (key == KEY_CTRL('y')) {
		Redo();
		return;
	}
	
	if (key == KEY_CTRL('z')) {
		Undo();
		return;
	}


	// The default action
	
	Component::OnKeyPressed(key);
}


/**
 * An event handler for a mouse event
 *
 * @param row the row
 * @param column the column
 * @param buttonState the button state bits
 */
void Editor::OnMouseEvent(int row, int column, mmask_t buttonState)
{
	if (doc == NULL) return;
	
	if ((buttonState & BUTTON1_PRESSED) != 0) {
		doc->FinalizeEditAction();
		MoveDocumentCursor(doc->PageStart() + row, colStart + column,
			(buttonState & BUTTON_SHIFT) != 0);
	}
}


/**
 * An event handler for mouse wheel
 *
 * @param row the row
 * @param column the column
 * @param wheel the wheel direction
 */
void Editor::OnMouseWheel(int row, int column, int wheel)
{
	if (doc == NULL) return;
	bool needsPaint = false;
	
	if (wheel < 0) {
		if (doc->PageStart() > 0) {
			doc->SetPageStart(doc->PageStart() - 1);
			needsPaint = true;
		}
	}
	
	if (wheel > 0) {
		if (doc->NumLines() - doc->PageStart() > Rows()) {
			doc->SetPageStart(doc->PageStart() + 1);
			needsPaint = true;
		}
	}
	
	if (needsPaint) Paint();
	UpdateCursor(false /* scroll */);
}


/**
 * An event handler for resizing the window
 *
 * @param oldRows the old number of rows
 * @param oldCols the old number of columns
 * @param newRows the new number of rows
 * @param newCols the new number of columns
 */
void Editor::OnResize(int oldRows, int oldCols, int newRows, int newCols)
{
	Component::OnResize(oldRows, oldCols, newRows, newCols);
	EnsureValidScroll();
	UpdateCursor();
}


/**
 * Set the highlight pattern
 *
 * @param pattern the pattern (use an empty string or NULL to clear)
 */
void Editor::SetHighlightPattern(const char* pattern)
{
	highlightPattern = pattern == NULL ? "" : pattern;
	Paint();
}


/**
 * Move the cursor to the next pattern match
 *
 * @param forward true to search forward, false to search backwards
 * @param keepIfOnMatch if the cursor is already at a match, keep it there
 * @param wrap true to wrap around
 * @return true if there is at least a single match
 */
bool Editor::FindNext(bool forward, bool keepIfOnMatch, bool wrap)
{
	if (highlightPattern == "") return false;

	int r = row;
	const char* line = doc->Line(r);
	int offset = offsetWithinLine;

	while (true) {
		const char* s = NULL;
		
		if (forward) {
			s = strstr(line + offset, highlightPattern.c_str());
		}
		else {
			const char* x = line;
			while (true) {
				x = strstr(x, highlightPattern.c_str());
				if (x == NULL || x - line >= offset) break;
				s = x;
				x += highlightPattern.length();
			}
		}

		if (s != NULL) {
			offset = s - line;
			int c = doc->CursorPosition(r, offset);
			bool currentMatch = r == row && offsetWithinLine >= offset
				&& offsetWithinLine < offset + highlightPattern.length();

			if (currentMatch) {
				if (keepIfOnMatch) return true;

				if (forward) {
					offset = s + highlightPattern.length() - line;
				}
				else {
					if (s == line) {
						s = NULL;
					}
					else {
						offset = s - line - 1;
					}
				}
			}
			else {
				MoveDocumentCursor(r, c);
				return true;
			}
		}

		if (s == NULL) {
			if (forward) {
				r++;
				if (r >= doc->NumLines()) {
					if (!wrap) return false;
					r = 0;
				}
			}
			else {
				if (r > 0) {
					r--;
				}
				else {
					if (!wrap) return false;
					r = doc->NumLines() - 1;
				}
			}

			if (row == r) return false;

			line = doc->Line(r);

			if (forward) {
				offset = 0;
			}
			else {
				offset = strlen(line);
			}
		}
	}
}
