/**********************************************************************
//    Copyright (c) 2016 Henry Seurer & Samuel Kelly
//
//    Permission is hereby granted, free of charge, to any person
//    obtaining a copy of this software and associated documentation
//    files (the "Software"), to deal in the Software without
//    restriction, including without limitation the rights to use,
//    copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the
//    Software is furnished to do so, subject to the following
//    conditions:
//
//    The above copyright notice and this permission notice shall be
//    included in all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//    OTHER DEALINGS IN THE SOFTWARE.
//
**********************************************************************/

#ifndef PI_UTILS_H
#define PI_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/syslog.h>

void *memory_clear(void *p, size_t n);

void *memory_alloc(size_t n);

void *memory_realloc(void *p, size_t n);

void memory_free(void *p);

char *trim_whitespace(char *str);

struct timespec timer_start();

long long timer_diff_minutes(struct timespec start_time);

#if !defined(NDEBUG)
#define ASSERT(x)  {if (!(x)){log_message(LOG_ALERT, __FUNCTION__, __FILE__, __LINE__, "Assert Fired" );}}
#else
#define ASSERT(context, x) ((void)0)
#endif

#define ERROR_LOG(...) log_message(LOG_CRIT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ )
#define DEBUG_LOG(...) log_message(LOG_DEBUG, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ )
#define INFO_LOG(...) log_message(LOG_INFO, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ )

void log_message(int log_level, const char *function, const char *file, int line,
                 const char *template, ...);

void create_logs();

void close_logs();

#define max(a, b) \
       ({ \
            __typeof__ (a) _a = (a); \
            __typeof__ (b) _b = (b); \
            _a > _b ? _a : _b; \
        })

#endif //PI_UTILS_H
