/*
 * flstd.h - v1.0 - MIT Licenced - https://github.com/ManidakisM/Flair
 * In time I would like it to have the functionality needed for common tasks. 
 * File loading/reading/writing, string manipulation, dynamic arrays,
 *		core math functionality including vectors and matrices
 *		and other stuff like that
 * Something like a my custom C Standard Library
 *
 * Do this:
 *      #define FLSTD_IMPLEMENTATION
 * before you include this file in *one* of your C or C++ files to create the implementation
 *
 * //i.e it should look like this
 *
 * #include ...
 * #include ...
 * #include ...
 * #define FLSTD_IMPLEMENTATION
 * #include "flstd.h"
 */

#ifndef __FLSTD_H__
#define __FLSTD_H__

#ifdef FL_STATIC
#define FLAPI static
#else
#define FLAPI extern
#endif

#ifdef __cplusplus
#define FL_BEGIN_DECLS extern "C" {
#define FL_END_DECLS }
#else
#define FL_BEGIN_DECLS
#define FL_END_DECLS
#endif

#ifndef __cplusplus
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;
#endif 
typedef char * cstr_t;

#define FL_TRUE  1
#define FL_FALSE 0

/* Needed for MSVC compiler */
#if defined(_WIN32) && !defined(__MINGW32__)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif
#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif
#endif

#ifndef NDEBUG
#define FL_ASSERT(condition) (condition) ? 0 : printf("Assertion Failed! %s >> %s:%d \n", #condition, __FILE__, __LINE__)
#define FL_LOG(format, ...) printf("[LOG]: " format " \n", __VA_ARGS__)
#else
#define FL_ASSERT(condition)
#define FL_LOG(format, ...)
#endif

FL_BEGIN_DECLS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
/////////////////////////////////////////////////////////////////
//	Sean T. Barrett's implementation of stretchy buffer
//	https://github.com/nothings/stb/blob/master/stretchy_buffer.h
*/

#define flstd_array_free(a)			((a) ? free(flstd__arrayraw(a)),0 : 0)
#define flstd_array_push(a,v)		(flstd__arraymaybegrow(a,1), (a)[flstd__arraysize(a)++] = (v))
#define flstd_array_count(a)		((a) ? flstd__arraysize(a) : 0)
#define flstd_array_add(a,n)		(flstd__arraymaybegrow(a,n), flstd__arraysize(a)+=(n), &(a)[flstd__arraysize(a)-(n)])
#define flstd_array_last(a)			((a)[flstd__arraysize(a)-1])

#define flstd__arrayraw(a)			((int *) (a) - 2)
#define flstd__arraycapacity(a)		flstd__arrayraw(a)[0]
#define flstd__arraysize(a)			flstd__arrayraw(a)[1]

#define flstd__arrayneedgrow(a,n)  ((a)==0 || flstd__arraysize(a)+(n) >= flstd__arraycapacity(a)) /* condition for expansion of the array */
#define flstd__arraymaybegrow(a,n) (flstd__arrayneedgrow(a,(n)) ? flstd__arraygrow(a,n) : 0)
#define flstd__arraygrow(a,n)      ((a) = flstd__arraygrowfunc((a), (n), sizeof(*(a))))

/*
 * Expand the array.
 * First we check wether the array has already been expanded at least once. 
 * If it has we calculate the doubled current capacity and calculate the minimum
 * required to hold the data we want.
 * We then proceed to allocate the memory needed and return a pointer to the third element
 * The first element holds the current capacity
 * The second element holds the number of elements in the array
 */
static void *(flstd__arraygrowfunc)(void *__arr, int __inc, int __sz)
{
	int double_capacity = __arr ? (flstd__arraycapacity(__arr) << 1) : 0;
	int min_needed = flstd_array_count(__arr) + __inc;
	int capacity = double_capacity > min_needed ? double_capacity : min_needed;
	int *p = (int *)realloc(__arr ? flstd__arrayraw(__arr) : 0, __sz * capacity + (sizeof(int) << 1));
	if (p) {
		if (!__arr)
			p[1] = 0;
		p[0] = capacity;
		return p + 2;
	}
	else {
		return (void *)(sizeof(int) << 2);
	}
}

/*
 * Reads the file contents and returns the memory address of the allocated string
 * Remember to free the memory after done using the buffer
 * Usage Example: 
 *		cstr_t buffer = flstd_file_read("myfile.txt");
 *		// do stuff with the contents
 *		flstd_file_free(buffer);
 */
FLAPI cstr_t flstd_file_read(const cstr_t __path);

/*
 * Free's the memory allocated from flstd_file_read function
 */
FLAPI void flstd_file_free(void *__return_from_flstd_file_read);

FL_END_DECLS
#endif /* __FLSTD_H__ */

#ifdef FLSTD_IMPLEMENTATION

FLAPI cstr_t flstd_file_read(const cstr_t __path) {
	long flstd__sz;
	cstr_t flstd__buffer;
	FILE *flstd__fp;

	flstd__fp = fopen(__path, "rb");
	fseek(flstd__fp, 0, SEEK_END);
	flstd__sz = ftell(flstd__fp);
	fseek(flstd__fp, 0, SEEK_SET);
	
	flstd__buffer = (cstr_t) malloc(sizeof(char) * flstd__sz + 1);
	flstd__buffer[flstd__sz] = '\0';
	fread(flstd__buffer, 1, flstd__sz, flstd__fp);
	
	fclose(flstd__fp);
	return flstd__buffer;
}

FLAPI  void flstd_file_free(void *__return_from_flstd_file_read) {
	free(__return_from_flstd_file_read);
}

#endif

/*
--------------------------------------------------------------------------------
MIT License
Copyright (c) 2017 Manidakis Marinos
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------------------------------------------------------------------
*/
