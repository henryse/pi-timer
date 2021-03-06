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

#include <stdbool.h>
#include <time.h>
#include "pi_timer_settings.h"
#include "version_config.h"
#include "pi_utils.h"
#include "stdlib.h"
#include "string.h"
#include "pi_string.h"

unsigned int run_minutes = 5;
unsigned char pin = 4;
bool run_as_daemon = false;
pid_t process_id = 0;
unsigned short server_port = 9080;
bool service_running = false;
const char* default_up_time = "9-18";
pi_string_ptr current_up_time = NULL;
unsigned int sleep_minutes = 55;
unsigned int duration_minutes = 0;
struct timespec start_time;
bool use_random = false;
unsigned int start_hour = 9;
unsigned int end_hour = 18;


bool is_operation_enabled() {
    time_t t = time(NULL);
    struct tm *local = localtime(&t); // getting local time
    int hour = local->tm_hour; // getting the current hour

    check_hours(start_hour, end_hour);
    if (hour >= start_hour && hour < end_hour) { // if time is between 9:00am and 5:00pm return true
        return true;
    }

    return false; // if not return false
}

void set_up_time_string(char *value){
    if (current_up_time == NULL){
        current_up_time = pi_string_new(16);
    }

    pi_string_append_str(current_up_time, value);

    start_hour = (unsigned int) atol(strtok(pi_string_c_string(current_up_time), "-"));
    end_hour = (unsigned int) atol(strtok(NULL, "-"));
}

void check_hours(unsigned int start, unsigned int end){
    if(start == 0)
    {
        start_hour = 9;
    }
    if(end == 0)
    {
        end_hour = 16;
    }
}


const char *get_pi_timer_version() {
    return PI_TIMER_VERSION;
}

unsigned int get_run_minutes() {
    return run_minutes;
}

void set_run_minutes(unsigned int value) {
    run_minutes = value;
}

unsigned int get_sleep_minutes() {
    return sleep_minutes;
}

unsigned int get_random_sleep_minutes() {
    return random_time();
}

void set_sleep_minutes(unsigned int value) {
    sleep_minutes = value;
}

unsigned char get_pin() {
    return pin;
}

void set_pin(unsigned char value) {
    pin = value;
}

bool get_run_as_daemon() {
    return run_as_daemon;
}

void set_run_as_daemon(bool value) {
    run_as_daemon = value;
}

pid_t get_process_id() {
    return process_id;
}

void set_process_id(pid_t value) {
    process_id = value;
}

bool get_log_mode() {
    return true;
}

unsigned short get_server_port() {
    return server_port;
}

void set_server_port(unsigned short value) {
    server_port = value;
}

bool get_service_running() {
    return service_running;
}

void set_service_running(bool value) {
    service_running = value;
}

int get_minutes_remaining() {
    long long minutes_so_far = timer_diff_minutes(start_time);

    return duration_minutes - (int) minutes_so_far;
}

void set_timer_start(unsigned int minutes) {
    duration_minutes = minutes;
    start_time = timer_start();
}

unsigned int random_time(){
    srand((unsigned int) time(NULL));
    unsigned int i = (unsigned int) (rand() % 60);
    return i + sleep_minutes;
}

const char *get_up_time_string() {
    if(current_up_time == NULL){
        return default_up_time;
    }

    return pi_string_c_string(current_up_time);
}

bool get_use_random(){
    return use_random;
}

void set_use_random(bool value){
    use_random = value;
}


