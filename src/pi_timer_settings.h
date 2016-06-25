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

#ifndef PI_TIMER_SETTINGS_H
#define PI_TIMER_SETTINGS_H

#include <unistd.h>
#include <stdbool.h>

const char *get_pi_timer_version();

unsigned int get_run_minutes();

void set_run_minutes(unsigned int value);

unsigned int get_sleep_minutes();

unsigned int get_random_sleep_minutes();

void set_sleep_minutes(unsigned int value);

unsigned char get_pin();

void set_pin(unsigned char value);

void set_uptime_string(char *value);

bool get_run_as_daemon();

bool is_operation_enabled();

void set_run_as_daemon(bool value);

unsigned int random_time();

pid_t get_process_id();

void set_process_id(pid_t value);

bool get_log_mode();

unsigned short get_server_port();

void set_server_port(unsigned short value);

bool get_service_running();

void set_service_running(bool value);

void set_timer_start(unsigned int minutes);

int get_minutes_remaining();

const char *get_up_time_string();

bool get_use_random();

void set_use_random(bool value);

void check_hours(unsigned int start, unsigned int end);


#endif //PI_TIMER_SETTINGS_H
