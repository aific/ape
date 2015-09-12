/*
 * Histogram.cpp
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
#include "Histogram.h"


/**
 * Create an instance of class Histogram
 */
Histogram::Histogram(void)
{
	// Nothing to do
}


/**
 * Destroy the object
 */
Histogram::~Histogram(void)
{
	// Nothing to do
}


/**
 * Look up an exact value in the histogram
 * 
 * @param key the key value
 * @return the associated value
 */
int Histogram::Get(int key)
{
	std::map<int, int>::iterator i = m.find(key);
	return i == m.end() ? 0 : i->second;
}


/**
 * Set the value
 * 
 * @param key the key value
 * @param value the new value
 */
void Histogram::Set(int key, int value)
{
	if (value == 0) {
		std::map<int, int>::iterator i = m.find(key);
		if (i != m.end()) m.erase(i);
	}
	else {
		m[key] = value;
	}
}


/**
 * Increment a value
 * 
 * @param key the key value
 */
void Histogram::Increment(int key)
{
	std::map<int, int>::iterator i = m.find(key);
	if (i == m.end()) {
		m[key] = 1;
	}
	else {
		i->second++;
	}
}


/**
 * Decrement a value
 * 
 * @param key the key value
 */
void Histogram::Decrement(int key)
{
	std::map<int, int>::iterator i = m.find(key);
	if (i == m.end()) {
		m[key] = -1;
	}
	else {
		int v = i->second;
		if (v == 1) {
			m.erase(i);
		}
		else {
			i->second = v + 1;
		}
	}
}


/**
 * Clear the histogram
 */
void Histogram::Clear(void)
{
	m.clear();
}


/**
 * Get the smallest key
 * 
 * @return the smallest key
 */
int Histogram::MinKey(void)
{
	std::map<int, int>::iterator i = m.begin();
	return i == m.end() ? 0 : i->first;
}


/**
 * Get the largest key
 * 
 * @return the largest key
 */
int Histogram::MaxKey(void)
{
	std::map<int, int>::reverse_iterator i = m.rbegin();
	return i == m.rend() ? 0 : i->first;
}
