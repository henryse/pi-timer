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
#include <string.h>
#include <stdarg.h>

#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#ifdef __MACH__

#include <mach/clock.h>
#include <mach/mach.h>

#endif

#include "pi_utils.h"
#include "pi_string.h"
#include "pi_timer_settings.h"

void *memory_clear(void *p, size_t n) {
    if (NULL != p) {
        memset(p, 0, n);
    }

    return p;
}

void *memory_alloc(size_t n) {
    void *p = NULL;

    p = malloc(n);

    return memory_clear(p, n);
}

void *memory_realloc(void *p, size_t n) {
    return realloc(p, n);
}

void memory_free(void *p) {
    if (p) {
        free(p);

    }
}

char *trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    //
    while (isspace(*str)) {
        str++;
    }

    // All spaces?
    //
    if (*str == 0) {
        return str;
    }

    // Trim trailing space
    //
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;

    // Write new null terminator
    //
    *(end + 1) = 0;

    return str;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"

void create_output_header(pi_string_t *output,
                          const char *status,
                          const char *function,
                          const char *file,
                          int line) {

    // Output the common header
    //
    pi_string_sprintf(output,
                      "service=pi_timer;status=%s;location=%s:%d;function=%s;message=",
                      status,
                      file,
                      line,
                      function);
}

#pragma clang diagnostic pop

void log_message(int log_level,
                 const char *function,
                 const char *file,
                 int line,
                 const char *template, ...) {

    char *message_type = NULL;
    bool log_message;

    switch (log_level) {
        case LOG_EMERG:
        case LOG_ALERT:
        case LOG_CRIT:
        case LOG_ERR:
        case LOG_WARNING:
            message_type = "ERROR";
            log_message = true;
            break;
        case LOG_NOTICE:
        case LOG_INFO:
            message_type = "INFO";
            log_message = get_log_mode();
            break;

        case LOG_DEBUG:
        default:
            message_type = "DEBUG";
            log_message = get_log_mode();
            break;
    }

    if (log_message) {
        pi_string_t *output = pi_string_new(1024);

        create_output_header(output, message_type, function, file, line);

        char *str;
        va_list arg_list;

        va_start(arg_list, template);
        vasprintf(&str, template, arg_list);
        va_end(arg_list);

        if (str) {
            pi_string_append_str(output, str);
            free(str);
        }

        if (output && get_process_id() == 0) {
            switch (log_level) {
                case LOG_EMERG:
                case LOG_ALERT:
                case LOG_CRIT:
                case LOG_ERR:
                case LOG_WARNING:
                    fprintf(stderr, "%s\n", pi_string_c_string(output));
                    fflush(stderr);
                    break;

                case LOG_NOTICE:
                case LOG_INFO:
                case LOG_DEBUG:
                default:
                    fprintf(stdout, "%s\n", pi_string_c_string(output));
                    fflush(stdout);
                    break;
            }
        }

        syslog(log_level, "%s", pi_string_c_string(output));

        pi_string_delete(output, true);
    }
}

void create_logs() {
    setlogmask(LOG_UPTO (LOG_DEBUG));
    openlog("pi_timer", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
}

void close_logs() {
    closelog();
}

void current_utc_time(struct timespec *ts) {
#ifdef __MACH__
    // OS X does not have clock_gettime, use clock_get_time
    //
    clock_serv_t c_clock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &c_clock);
    clock_get_time(c_clock, &mts);
    mach_port_deallocate(mach_task_self(), c_clock);
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
#else
    // How everyone else does it.
    //
    clock_gettime(CLOCK_REALTIME, ts);
#endif
}

// call this function to start a nanosecond-resolution timer
//
struct timespec timer_start() {
    struct timespec start_time;
    current_utc_time(&start_time);
    return start_time;
}

static const long long kNsPerSec = 1000000000;

// Convert timespec to nanoseconds
//
long long timespec_to_ns(const struct timespec *ts) {
    long long base_ns = (long long) (ts->tv_sec) * kNsPerSec;
    return base_ns + (long long) (ts->tv_nsec);
}


long long timer_diff_minutes(struct timespec start_time) {
    struct timespec end_time;
    current_utc_time(&end_time);

    long long nanoseconds = timespec_to_ns(&end_time) - timespec_to_ns(&start_time);

    return (nanoseconds / kNsPerSec) / 60;
}


#pragma clang diagnostic pop