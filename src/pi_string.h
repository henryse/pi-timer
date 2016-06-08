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

#ifndef PI_STRING_H
#define PI_STRING_H

#include <stdbool.h>
#include <unistd.h>

typedef struct pi_string_struct {
    char *c_string;
    size_t position;
    size_t size;
} pi_string_t;

typedef pi_string_t *pi_string_ptr;

// Creates a new pi_string_t with the specified size
//
pi_string_ptr pi_string_new(size_t size);

// Destroys the given pi_string_t.  Pass 1 to free_string if the underlying c string should also be freed
//
void pi_string_delete(pi_string_ptr pi_string, bool free_string);

// Resets the pi_string_t to empty
//
void pi_string_reset(pi_string_ptr pi_string);

// Appends the given character to the string builder
//
void pi_string_append_char(pi_string_ptr pi_string, const char ch);

// Appends at most length of the given src string to the string buffer
//
void pi_string_append_str_length(pi_string_ptr pi_string, const char *src, size_t length);

// Appends the given src string to the string builder
//
void pi_string_append_str(pi_string_ptr pi_string, const char *src);

// Appends the formatted string to the given string builder
//
void pi_string_sprintf(pi_string_ptr pi_string, const char *fmt, ...);

// Returns the pi_string_t as a regular C String
//
#define pi_string_c_string(pi_string) ((pi_string)->c_string)

// Returns the size of the string
//
#define pi_string_c_string_length(pi_string) ((pi_string)->position)

#endif // PI_STRING_H