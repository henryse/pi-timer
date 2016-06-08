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

#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <memory.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include "pi_timer_settings.h"
#include "pi_utils.h"
#include "pi_timer_server.h"
#include "pi_timer_gpio.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void usage(const char *program) {
    fprintf(stdout, "Version: %s\n", get_pi_timer_version());
    fprintf(stdout, "Usage:     %s --run=RUN_MINUTES --sleep=SLEEP_MINUTES \n", program);
    fprintf(stdout, "Example:   %s --run=5 --sleep=50 \n\n", program);
    fprintf(stdout, "Simple Raspberry Timer for our compost bin.\n\n");
    fprintf(stdout, "     run        number of minutes to run, default: %u\n", get_run_minutes());
    fprintf(stdout, "     sleep      number of minutes to sleep, default: %u\n", get_sleep_minutes());
    fprintf(stdout, "     gpio       gpio pin to signal fan, default: %u\n", (unsigned int) get_pin());
    fprintf(stdout, "     daemon     run as daemon, default: %s\n", get_run_as_daemon() ? "true" : "false");
    fprintf(stdout, "     port       port to listen to, default: %d\n", (int) get_server_port());
    fprintf(stdout, "     help       get this help message\n");
}

bool parse_arguments(int argc, char *argv[]) {

    static struct option long_options[] =
            {
                    {"run",    optional_argument, 0, 'r'},
                    {"sleep",  optional_argument, 0, 's'},
                    {"gpio",   optional_argument, 0, 'g'},
                    {"daemon", optional_argument, 0, 'd'},
                    {"port",   optional_argument, 0, 'p'},
                    {"help",   optional_argument, 0, '?'},
                    {0, 0,                        0, 0}
            };

    int option_index = 0;
    int c = 0;

    do {
        c = getopt_long(argc, argv, "?r:s:p:d:g:", long_options, &option_index);

        switch (c) {
            case -1:
                // Ignore this one.
                break;

            case 'r':
                set_run_minutes((unsigned int) max(atol(optarg), 1));
                fprintf(stdout, "\nRunning for %u minutes\n", get_run_minutes());
                break;

            case 's':
                set_sleep_minutes((unsigned int) max(atol(optarg), 1));
                fprintf(stdout, "\nSleeping for %u minutes\n", get_sleep_minutes());
                break;

            case 'g':
                set_pin((unsigned char) atol(optarg));
                fprintf(stdout, "\nGPIO Pin to use for the pi timer %u\n", get_pin());
                break;

            case 'p':
                set_server_port((unsigned short) atol(optarg));
                fprintf(stdout, "\nPort to listen on %u\n", get_server_port());
                break;

            case 'd':
                set_run_as_daemon(strcmp(optarg, "true") == 0);
                fprintf(stdout, "\nRun as daemon %s\n", get_run_as_daemon() ? "true" : "false");
                break;

            case '?':
            default:
                usage("pi-timer");
                return false;
        }
    } while (c != -1);

    return true;
}

void pi_timer_start() {
    INFO_LOG("Start pi timer on pin %u", get_pin());

    set_gpio(get_pin(), HIGH_SIGNAL);
}

void pi_timer_stop() {
    INFO_LOG("Stop pi timer on pin %u", get_pin());

    set_gpio(get_pin(), LOW_SIGNAL);
}

void service_stop() {
    pi_timer_stop();

    set_service_running(false);

    close_logs();
}

void signal_shutdown(int value) {
    INFO_LOG("Shutting down the service, signal: %d", value);
    service_stop();
    exit(value);
}

void signal_SIGPIPE(int value) {
    INFO_LOG("SIGPIPE failure: %d", value);
    service_stop();
    exit(value);
}

void pi_timer_process_setup() {
    signal(SIGABRT, signal_shutdown);
    signal(SIGFPE, signal_shutdown);
    signal(SIGILL, signal_shutdown);
    signal(SIGINT, signal_shutdown);
    signal(SIGSEGV, signal_shutdown);
    signal(SIGTERM, signal_shutdown);
    signal(SIGPIPE, signal_SIGPIPE);
}

void pause_minutes(unsigned minutes) {
    set_timer_start(minutes);
    sleep(minutes * 60);
}

void fork_process() {
    if (get_run_as_daemon()) {
        // Create child process
        //
        pid_t process_id = fork();

        // Indication of fork() failure
        //
        if (process_id < 0) {

            ERROR_LOG("Forking the process failed.");
            // Return failure in exit status
            exit(1);
        }

        // PARENT PROCESS. Need to kill it.
        //
        if (process_id > 0) {
            ERROR_LOG("Process ID of child process %d", process_id);

            // return success in exit status
            //
            exit(0);
        }

        // Unmask the file mode
        //
        umask(0);

        //set new session
        //
        pid_t sid = setsid();
        if (sid < 0) {
            ERROR_LOG("Failed to obtain process id");
            // Return failure
            exit(1);
        }

        set_process_id(sid);

        // Daemon Process ID
        //
        INFO_LOG("Starting process %d", get_process_id());

        // Change the current working directory to root.
        //
        chdir("/");

        // Close stdin. stdout and stderr
        //
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    // Setup base service
    //
    pi_timer_process_setup();
}

int main(int argc, const char *argv[]) {

#ifdef BCMHOST
    printf("Hello From BCMHOST\n\r");
#endif

    if (parse_arguments(argc, (char **) argv)) {

        fork_process();

        setup_wring_pi();

        create_logs();

        set_service_running(true);

        pi_timer_service_start();

        pi_timer_stop();

        while (true) {
            pi_timer_start();
            pause_minutes(get_run_minutes());

            pi_timer_stop();
            pause_minutes(get_sleep_minutes());
        }
    }

    return 0;
}

#pragma clang diagnostic pop