/*
 * util.h
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


#ifndef __UTIL_H
#define __UTIL_H

struct timeval;


/**
 * Logging levels
 */
enum log_level_t {
	LL_NONE = 0,
	LL_DEBUG = 1,
	LL_INFO = 2,
	LL_WARNING = 3,
	LL_ERROR = 4,
	LL_FATAL = 5
};


/**
 * Start a timer
 */
void StartTimer(void);

/**
 * End the timer
 * 
 * @param the number of elapsed seconds
 */
double EndTimer(void);

/**
 * Convert timeval to seconds
 * 
 * @param tim the time value
 * @return the number of seconds
 */
double seconds(struct timeval* tim);

/**
 * Compute the number of digits of a number
 * 
 * @param num the number
 * @param base the base
 * @return the number of digits
 */
int digits(int num, int base = 10);

/**
 * Add a message to a log
 *
 * @param level the log level
 * @param format the printf format
 * @param ... the format arguments
 */
void log(enum log_level_t level, const char* format, ...);


#endif
