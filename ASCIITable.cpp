/*
 * ASCIITable.cpp
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
#include "ASCIITable.h"

const char* ASCII_CTRL_CHAR[] = {
	"NUL","SOH","STX","ETX","EOT","ENQ","ACK","BEL","BS" ,"TAB","LF",
	"VT" ,"NP" ,"CR" ,"SO" ,"SI" ,"DLE","DC1","DC2","DC3","DC4","NAK",
	"SYN","ETB","CAN","EM" ,"EOF","ESC","FS" ,"GS" ,"RS" ,"US" };


/**
 * Create an instance of class ASCIITable
 */
ASCIITable::ASCIITable(void)
	: Window("ASCII Table", 0, 0, 11 + 12, 4 * 17 + 4, 6, 0)
{
	allowMaximize = false;
	allowResize = false;
	
	SetMinSize(Rows(), Columns());
}


/**
 * Destroy the object
 */
ASCIITable::~ASCIITable(void)
{
	// Nothing to do
}


/**
 * Paint the contents of the window
 */
void ASCIITable::PaintContents(void)
{
	int alt_r = 11;


	// Regular ASCII table

	// Print headers
	
	tcw->SetAttribute(A_BOLD, true);
	
	for (int i = 0; i < 8; i++) {
		tcw->OutFormattedText(i + 2, 2, "0x%X", i);
	}
	
	for (int i = 0; i < 16; i++) {
		tcw->OutFormattedText(1, 7 + i * 4, "0x%X", i);
	}
	
	tcw->SetAttribute(A_BOLD, false);
	
	
	// Print contents
	
	tcw->SetAttribute(A_DIM, true);

	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 16; c++) {
			int C = (r << 4) | c;
			if (C >= 0 && C < 32) {
				tcw->OutText(r + 2, 7 + c * 4, ASCII_CTRL_CHAR[C]);
			}
			else {
				tcw->OutChar(r + 2, 8 + c * 4, C);
			}
		}
	}

	tcw->SetAttribute(A_DIM, false);


	// Alternative ASCII table

	// Print headers
	
	tcw->SetAttribute(A_BOLD, true);
	tcw->OutText(alt_r, 2, "Alternative Character Set");
	
	for (int i = 0; i < 8; i++) {
		tcw->OutFormattedText(alt_r + i + 3, 2, "0x%X", i);
	}
	
	for (int i = 0; i < 16; i++) {
		tcw->OutFormattedText(alt_r + 2, 7 + i * 4, "0x%X", i);
	}
	
	tcw->SetAttribute(A_BOLD, false);
	
	
	// Print contents
	
	tcw->SetAttribute(A_DIM, true);

	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 16; c++) {
			int C = (r << 4) | c;
			if (r > 0) {
				tcw->OutChar(alt_r + r + 3, 8 + c * 4, C | A_ALTCHARSET);
			}
		}
	}

	tcw->SetAttribute(A_DIM, false);
}

