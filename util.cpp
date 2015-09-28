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
	
	return std::fabs(seconds(&tend) - seconds(&tstart));
}


/**
 * Convert timeval to seconds
 * 
 * @param tim the time value
 * @return the number of seconds
 */
double seconds(struct timeval* tim)
{
	return (double)tim->tv_sec + (double)tim->tv_usec / 1000000.0;
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
