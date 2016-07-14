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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "pi_string.h"
#include "pi_utils.h"

pi_string_ptr pi_string_new(size_t size) {
    pi_string_ptr pi_string = (pi_string_ptr) malloc(sizeof(pi_string_t));
    if (pi_string) {
        memory_clear(pi_string, sizeof(pi_string_t));
        size =  max(size, 4);

        pi_string->size = size;
        pi_string->c_string = (char *) malloc(size);
        memory_clear(pi_string->c_string, size);
        pi_string->position = 0;
    }

    return pi_string;
}

void pi_string_reset(pi_string_ptr pi_string) {
    if (NULL != pi_string) {
        pi_string->position = 0;
        memory_clear(pi_string->c_string, pi_string->size);
    }
}

void pi_string_delete(pi_string_ptr pi_string, bool free_string) {

    if (pi_string) {
        if (free_string) {
            memory_clear(pi_string->c_string, pi_string->size);
            free(pi_string->c_string);
        }

        memory_clear(pi_string, sizeof(pi_string_t));
        free(pi_string);
    }
}

bool string_buffer_resize(pi_string_ptr pi_string, const size_t new_size) {

    if (NULL == pi_string) {
        return false;
    }

    char *old_c_string = pi_string->c_string;

    pi_string->c_string = (char *) realloc(pi_string->c_string, new_size);
    if (pi_string->c_string == NULL) {
        pi_string->c_string = old_c_string;
        return false;
    }
    memory_clear(pi_string->c_string + pi_string->position, new_size - pi_string->position);

    pi_string->size = new_size;
    return true;
}

int string_buffer_double_size(pi_string_ptr pi_string) {
    return string_buffer_resize(pi_string, pi_string->size * 2);
}

void pi_string_append_char(pi_string_ptr pi_string, const char ch) {
    if (NULL == pi_string) {
        return;
    }

    if (pi_string->position == pi_string->size - 1) {
        string_buffer_double_size(pi_string);
    }

    pi_string->c_string[pi_string->position++] = ch;
}

void pi_string_append_str_length(pi_string_ptr pi_string, const char *src, size_t length) {

    if (NULL == pi_string || NULL == src) {
        return;
    }

    size_t chars_remaining;
    size_t chars_required;
    size_t new_size;

    // <buffer size> - <zero based index of next char to write> - <space for null terminator>
    chars_remaining = pi_string->size - pi_string->position - 1;
    if (chars_remaining < length) {
        chars_required = length - chars_remaining;
        new_size = pi_string->size;
        do {
            new_size = new_size * 2;
        } while (new_size < (pi_string->size + chars_required));
        string_buffer_resize(pi_string, new_size);
    }

    memcpy(pi_string->c_string + pi_string->position, src, length);
    pi_string->position += length;
}

void pi_string_append_str(pi_string_ptr pi_string, const char *src) {
    pi_string_append_str_length(pi_string, src, strlen(src));
}

void pi_string_sprintf(pi_string_ptr pi_string, const char *template, ...) {

    if (NULL == pi_string) {
        return;
    }

    char *str;
    va_list arg_list;

    va_start(arg_list, template);
    vasprintf(&str, template, arg_list);
    va_end(arg_list);

    if (!str) {
        return;
    }

    pi_string_append_str(pi_string, str);
    free(str);
}

#pragma clang diagnostic pop