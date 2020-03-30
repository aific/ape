/*
 * util.cpp
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
#include "util.h"

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <cstring>

static struct rusage rstart;
static struct timeval tstart;
static struct rusage rend;
static struct timeval tend;


/**
 * Start a timer
 */
void StartTimer(void)
{
	gettimeofday(&tstart, NULL);
	getrusage(RUSAGE_SELF, &rstart);
}


/**
 * End the timer
 * 
 * @param the number of elapsed seconds
 */
double EndTimer(void)
{
	gettimeofday(&tend, NULL);
	getrusage(RUSAGE_SELF, &rend);
	
	return std::fabs(seconds(tend) - seconds(tstart));
}


/**
 * Get the current time in seconds
 * 
 * @return the number of seconds
 */
double Time(void)
{
	static struct timeval t;
	gettimeofday(&t, NULL);
	return seconds(t);
}


/**
 * Convert timeval to seconds
 * 
 * @param tim the time value
 * @return the number of seconds
 */
double seconds(struct timeval& tim)
{
	return (double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
}


/**
 * Compute the number of digits of a number
 * 
 * @param num the number
 * @param base the base
 * @return the number of digits
 */
int digits(int num, int base)
{
	int d = 1;
	for (int n = num; n >= base; n /= base) d++;
	return d;
}


/**
 * Add a message to a log
 *
 * @param level the log level
 * @param format the printf format
 * @param ... the format arguments
 */
void log(enum log_level_t level, const char* format, ...)
{
	// ### This should be cached

	FILE* f = fopen("log", "a");
	assert(f != NULL);

	switch (level) {
		case LL_NONE   : fprintf(f, "[-----] "); break;
		case LL_DEBUG  : fprintf(f, "[DEBUG] "); break;
		case LL_INFO   : fprintf(f, "[INFO ] "); break;
		case LL_WARNING: fprintf(f, "[WARN ] "); break;
		case LL_ERROR  : fprintf(f, "[ERROR] "); break;
		case LL_FATAL  : fprintf(f, "[FATAL] "); break;
	}
	
	va_list args;
	va_start(args, format);
	vfprintf(f, format, args);
	fprintf(f, "\n");
	va_end(args);

	if (level >= LL_ERROR) fflush(f);

	fclose(f);
}


/**
 * Normalize a path
 *
 * @param path the path
 * @param wd the working directory (use NULL for the CWD)
 * @return the normalized path, or "" on error
 */
std::string NormalizePath(const char* path, const char* wd)
{
	if (path == NULL) return "";


	// Prefix the path with the working directory

	std::string absPath = path;
	if (*path != '/') {
		std::string s;
		if (wd == NULL || *wd == '\0') {
			char cwd[PATH_MAX];
			if (getcwd(cwd, sizeof(cwd)) == NULL) {
				log(LL_ERROR, "getcwd() failed");
				return "";
			}
			if (*cwd != '/') {
				return "";
			}
			s = cwd;
		}
		else {
			if (*wd != '/') {
				return "";
			}
			s = wd;
		}
		s += "/";
		absPath = s + absPath;
	}

	if (absPath[0] != '/') return "";


	// Strip the path

	char* s = strdup(absPath.c_str());

	for (char* p = s + 1; *p != '\0'; ) {
		
		char* t = strchr(p, '/');
		if (t == NULL) t = p + strlen(p);

		size_t len = ((size_t) t) - ((size_t) p);
		if (len == 2 && strncmp(p, "..", 2) == 0) {
			if (p == s + 1) {
				if (*t == '\0') {
					*p = '\0';
				}
				else {
					memmove(p, p + 3, strlen(p) - 2);
				}
			}
			else {
				bool ok = false;
				for (char* x = p - 2; x != s - 1; x--) {
					if (*x == '/') {
						ok = true;
						if (*t == '\0') {
							p = x + 1;
							*p = '\0';
						}
						else {
							memmove(x + 1, p + 3, strlen(p) - 2);
							p = x + 1;
						}
						break;
					}
				}
			}
		}
		else if (len == 1 && *p == '.') {
			if (*t == '\0') {
				*p = '\0';
			}
			else {
				memmove(p, p + 2, strlen(p) - 1);
			}
		}
		else if (len == 0) {
			if (*t == '\0') {
				*p = '\0';
			}
			else {
				memmove(p, p + 1, strlen(p));
			}
		}
		else if (*t == '\0') {
			break;
		}
		else {
			p = t + 1;
		}
	}


	// Remove the trailing /
	
	if (s[1] != '\0' && s[strlen(s)-1] == '/') {
		s[strlen(s)-1] = '\0';
	}

	std::string r = s;
	free(s);
	return r;
}

