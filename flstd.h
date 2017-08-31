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
typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
#endif

#define FL_TRUE 1
#define FL_FALSE 0

/* Need for MSVC compiler */
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
#define FL_ASSERT(condition) (condition) ? ((void)0) : printf("Assertion Failed! %s >> %s:%d \n", #condition, __FILE__, __LINE__)
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

#define flstd_array_free(a)         ((a) ? free(flstd__arrayraw(a)),0 : 0)
#define flstd_array_push(a,v)       (flstd__arraymaybegrow(a,1), (a)[flstd__arrayn(a)++] = (v))
#define flstd_array_count(a)        ((a) ? flstd__arrayn(a) : 0)
#define flstd_array_add(a,n)        (flstd__arraymaybegrow(a,n), flstd__arrayn(a)+=(n), &(a)[flstd__arrayn(a)-(n)])
#define flstd_array_last(a)         ((a)[flstd__arrayn(a)-1])

#define flstd__arrayraw(a) ((int *) (a) - 2)
#define flstd__arraym(a)   flstd__arrayraw(a)[0]
#define flstd__arrayn(a)   flstd__arrayraw(a)[1]

#define flstd__arrayneedgrow(a,n)  ((a)==0 || flstd__arrayn(a)+(n) >= flstd__arraym(a))
#define flstd__arraymaybegrow(a,n) (flstd__arrayneedgrow(a,(n)) ? flstd__arraygrow(a,n) : 0)
#define flstd__arraygrow(a,n)      ((a) = flstd__arraygrowf((a), (n), sizeof(*(a))))

static void *(flstd__arraygrowf)(void *arr, int increment, int itemsize)
{
	int dbl_cur = arr ? 2 * flstd__arraym(arr) : 0;
	int min_needed = flstd_array_count(arr) + increment;
	int m = dbl_cur > min_needed ? dbl_cur : min_needed;
	int *p = (int *)realloc(arr ? flstd__arrayraw(arr) : 0, itemsize * m + sizeof(int) * 2);
	if (p) {
		if (!arr)
			p[1] = 0;
		p[0] = m;
		return p + 2;
	}
	else {
		return (void *)(2 * sizeof(int));
	}
}

/**
 * Reads the file contents and returns the memory address of the allocated string
 * Remember to free the memory after done using the buffer
 * Usage Example: 
 *		char *buffer = flstd_file_read("myfile.txt");
 *		// do stuff with the contents
 *		flstd_file_free(buffer);
 *
 * @param const char *__path : The path to the file to read
 * @return char * : the memory adress of the allocated string
 */
FLAPI char *(flstd_file_read)(const char *__path);

/**
 * Free's the memory allocated from flstd_file_read function
 * @param void *__return_from_flstd_file_read : the pointer to the return value of flstd_file_read function
 * @return void
 */
FLAPI void flstd_file_free(void *__return_from_flstd_file_read);

FL_END_DECLS
#endif /* __FLSTD_H__ */

#ifdef FL_IMPLEMENTATION


FLAPI char *(flstd_file_read)(const char *__path) {
	long flstd__sz;
	char *flstd__buffer;
	FILE *flstd__fp;

	flstd__fp = fopen(__path, "rb");
	fseek(flstd__fp, 0, SEEK_END);
	flstd__sz = ftell(flstd__fp);
	fseek(flstd__fp, 0, SEEK_SET);
	
	flstd__buffer = (char *) malloc(sizeof(char) * flstd__sz + 1);
	flstd__buffer[flstd__sz] = '\0';
	fread(flstd__buffer, 1, flstd__sz, flstd__fp);
	
	fclose(flstd__fp);
	return flstd__buffer;
}

FLAPI void flstd_file_free(void *__return_from_flstd_file_read) {
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
