/*
 * main.cpp
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

#include <csignal>
#include <getopt.h>
#include <libgen.h>
#include <unistd.h>

#include "Manager.h"
#include "ColorTable.h"
#include "ASCIITable.h"
#include "MenuWindow.h"
#include "EditorWindow.h"


/**
 * Handle the SIGINT signal
 */
static void sigint(int sig)
{
    exit(1);
}


/**
 * Short command-line arguments
 */
static const char* SHORT_OPTIONS = "h";


/**
 * Long command-line arguments
 */
static struct option LONG_OPTIONS[] =
{
	{"help"         , no_argument,       0, 'h'},
	{0, 0, 0, 0}
};


/**
 * Print the usage information
 *
 * @param arg0 the first element in the argv array
 */
static void usage(const char* arg0) {

	char* s = strdup(arg0);
	char* p = basename(s);
	fprintf(stderr, "Usage: %s [OPTIONS] [FILE [FILE...]]\n\n", p);
	free(s);
	
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -h, --help            Show this usage information and exit\n");
}


/**
 * The entry point to the application
 *
 * @param argc the number of command-line arguments
 * @param argv the command-line arguments
 * @return the exit code
 */
int main(int argc, char * const argv[])
{
	// Parse the command-line arguments

	while (true) {
		int option_index = 0;
		int c = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS,
				&option_index);
		if (c == -1) break;

		switch (c) {

			case 'h':
				usage(argv[0]);
				return 0;

			case '?':
			case ':':
				return 1;

			default:
				abort();
		}
	}


	// Set up the signal handlers and initialize

	signal(SIGINT, sigint);
	signal(SIGSEGV, sigint);
	signal(SIGABRT, sigint);

	wm.Initialize();


	// Open the files

	if (optind >= argc) {

		EditorWindow* w = new EditorWindow(2, 1, wm.Rows()-4, wm.Columns()-2);
		w->Maximize();
		wm.Add(w);
	}
	else {
		int n = argc - optind;

		for (int i = optind; i < argc; i++) {

			int r = i - optind + 1;
			int c = i - optind;
			int rows = wm.Rows() - 1 - n;
			int cols = wm.Columns() + 1 - n;
			
			if (n == 1) {
				r++; c++;
				rows -= 2;
				cols -= 2;
			}

			EditorWindow* w = new EditorWindow(r, c, rows, cols);
			w->LoadFromFile(argv[i]);

			if (n == 1) {
				w->Maximize();
			}

			wm.Add(w);
		}
	}


	// Main loop

	wm.Paint();
	wm.Refresh();

	for (;;) {
		usleep(20 * 1000);
		wm.ProcessMessages();
	}

	sigint(0);
	return 0;
}
