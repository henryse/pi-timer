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

#ifndef __MACH__
#define _POSIX_C_SOURCE 200809L
#include <strings.h>
#endif

#ifndef __unused
#define __unused
#endif

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "pi_utils.h"
#include "pi_timer_settings.h"
#include "pi_string.h"
#include "pi_timer_gpio.h"
#include "pi_strmap.h"

size_t http_read_line(int socket, pi_string_t *buffer) {
    if (NULL == buffer) {
        return 0;
    }

    // Free the buffer because the expectation is that we are reading in a
    // whole new line.
    //
    pi_string_reset(buffer);

    // Now we read in the data:
    //
    char c = '\0';

    while (c != '\n') {
        ssize_t n = recv(socket, &c, 1, 0);

        if (n > 0) {
            if (c == '\r') {
                n = recv(socket, &c, 1, MSG_PEEK);

                if ((n > 0) && (c == '\n')) {
                    recv(socket, &c, 1, 0);
                }
                else {
                    c = '\n';
                }
            }
            pi_string_append_char(buffer, c);
        }
        else {
            c = '\n';
        }
    }

    return pi_string_c_string_length(buffer);
}

void http_output_pins(pi_string_t *response_body) {

    pi_string_append_str(response_body, "<style type=\"text/css\">\n"
            ".tg  {border-collapse:collapse;border-spacing:0;}\n"
            ".tg td{font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;}\n"
            ".tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;}\n"
            ".tg .tg-baqh{text-align:center;vertical-align:top}\n"
            ".tg .tg-ud39{font-weight:bold;color:#036400;text-align:center;vertical-align:top}\n"
            ".tg .tg-w8l0{font-weight:bold;background-color:#fffc9e;text-align:center;vertical-align:top}\n"
            ".tg .tg-amwm{font-weight:bold;text-align:center;vertical-align:top}\n"
            ".tg .tg-mxrt{font-weight:bold;color:#fe0000;text-align:center;vertical-align:top}\n"
            ".tg .tg-q0p8{font-weight:bold;color:#329a9d;text-align:center;vertical-align:top}\n"
            ".tg .tg-g5x8{font-weight:bold;color:#cd9934;text-align:center;vertical-align:top}\n"
            ".tg .tg-scrz{font-weight:bold;background-color:#ffffff;color:#000000;text-align:center;vertical-align:top}\n"
            ".tg .tg-rgq4{font-weight:bold;color:#34696d;text-align:center;vertical-align:top}\n"
            "</style>");

    pi_string_sprintf(response_body, "<P><B>Main GPIO Connector</B><P/>\r\n");

    pi_string_append_str(response_body, "<table class=\"tg\">");

    pi_string_append_str(response_body, "<tr>\n"
            "    <th class=\"tg-w8l0\">WiringPI Pin</th>\n"
            "    <th class=\"tg-w8l0\">Setting</th>\n"
            "    <th class=\"tg-w8l0\">Name</th>\n"
            "    <th class=\"tg-w8l0\">Header</th>\n"
            "    <th class=\"tg-w8l0\">Header</th>\n"
            "    <th class=\"tg-w8l0\">Name</th>\n"
            "    <th class=\"tg-w8l0\">Setting</th>\n"
            "    <th class=\"tg-w8l0\">Wiring PI Pin</th>\n"
            "  </tr>");

    pi_string_append_str(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-mxrt\">3.3v</td>\n"
            "    <td class=\"tg-w8l0\">1</td>\n"
            "    <td class=\"tg-w8l0\">2</td>\n"
            "    <td class=\"tg-mxrt\">5v</td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "  </tr>");

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">8</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-q0p8\">SDA</td>\n"
            "    <td class=\"tg-w8l0\">3</td>\n"
            "    <td class=\"tg-w8l0\">4</td>\n"
            "    <td class=\"tg-mxrt\">5v</td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "  </tr>", get_gpio_str(8));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">9</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-q0p8\">SCL</td>\n"
            "    <td class=\"tg-w8l0\">5</td>\n"
            "    <td class=\"tg-w8l0\">6</td>\n"
            "    <td class=\"tg-amwm\">GND</td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "  </tr>", get_gpio_str(9));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">7</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-ud39\">GPIO 7</td>\n"
            "    <td class=\"tg-w8l0\">7</td>\n"
            "    <td class=\"tg-w8l0\">8</td>\n"
            "    <td class=\"tg-g5x8\">TxD</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-amwm\">15</td>\n"
            "  </tr>", get_gpio_str(7), get_gpio_str(15));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-scrz\">GND</td>\n"
            "    <td class=\"tg-w8l0\">9</td>\n"
            "    <td class=\"tg-w8l0\">10</td>\n"
            "    <td class=\"tg-g5x8\">RxD</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-amwm\">16</td>\n"
            "  </tr>", get_gpio_str(16));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">0</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-ud39\">GPIO 0</td>\n"
            "    <td class=\"tg-w8l0\">11</td>\n"
            "    <td class=\"tg-w8l0\">12</td>\n"
            "    <td class=\"tg-ud39\">GPIO 1</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-amwm\">1</td>\n"
            "  </tr>", get_gpio_str(0), get_gpio_str(1));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">2</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-ud39\">GPIO 2</td>\n"
            "    <td class=\"tg-w8l0\">13</td>\n"
            "    <td class=\"tg-w8l0\">14</td>\n"
            "    <td class=\"tg-amwm\">GND</td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "  </tr>", get_gpio_str(2));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">3</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-ud39\">GPIO 3</td>\n"
            "    <td class=\"tg-w8l0\">15</td>\n"
            "    <td class=\"tg-w8l0\">16</td>\n"
            "    <td class=\"tg-ud39\">GPIO 4</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-amwm\">4</td>\n"
            "  </tr>", get_gpio_str(3), get_gpio_str(4));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-mxrt\">3.3v</td>\n"
            "    <td class=\"tg-w8l0\">17</td>\n"
            "    <td class=\"tg-w8l0\">18</td>\n"
            "    <td class=\"tg-ud39\">GPIO 5</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-amwm\">5</td>\n"
            "  </tr>", get_gpio_str(5));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">12</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-rgq4\">MOSI</td>\n"
            "    <td class=\"tg-w8l0\">19</td>\n"
            "    <td class=\"tg-w8l0\">20</td>\n"
            "    <td class=\"tg-amwm\">GND</td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "  </tr>", get_gpio_str(12));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">13</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-rgq4\">MISO</td>\n"
            "    <td class=\"tg-w8l0\">21</td>\n"
            "    <td class=\"tg-w8l0\">22</td>\n"
            "    <td class=\"tg-ud39\">GPIO 6</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-amwm\">6</td>\n"
            "  </tr>", get_gpio_str(13), get_gpio_str(6));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">14</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-rgq4\">SCLK</td>\n"
            "    <td class=\"tg-w8l0\">23</td>\n"
            "    <td class=\"tg-w8l0\">24</td>\n"
            "    <td class=\"tg-rgq4\">CE 0</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-amwm\">10</td>\n"
            "  </tr>", get_gpio_str(14), get_gpio_str(10));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-amwm\">GND</td>\n"
            "    <td class=\"tg-w8l0\">25</td>\n"
            "    <td class=\"tg-w8l0\">26</td>\n"
            "    <td class=\"tg-rgq4\">CE 1</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-amwm\">11</td>\n"
            "  </tr>", get_gpio_str(11));

    pi_string_append_str(response_body, "</table>");

    pi_string_sprintf(response_body, "<P><B>Secondary GPIO Connector Rev. 2 Pi Only</B><P/>\r\n");

    pi_string_append_str(response_body, "<table class=\"tg\">");

    pi_string_append_str(response_body, "<tr>\n"
            "    <th class=\"tg-w8l0\">WiringPI Pin</th>\n"
            "    <th class=\"tg-w8l0\">Setting</th>\n"
            "    <th class=\"tg-w8l0\">Name</th>\n"
            "    <th class=\"tg-w8l0\">Header</th>\n"
            "    <th class=\"tg-w8l0\">Header</th>\n"
            "    <th class=\"tg-w8l0\">Name</th>\n"
            "    <th class=\"tg-w8l0\">Setting</th>\n"
            "    <th class=\"tg-w8l0\">Wiring PI Pin</th>\n"
            "  </tr>");

    pi_string_append_str(response_body, "  <tr>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-mxrt\">5v</td>\n"
            "    <td class=\"tg-w8l0\">1</td>\n"
            "    <td class=\"tg-w8l0\">2</td>\n"
            "    <td class=\"tg-mxrt\">3.3v</td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "  </tr>");

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">17</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-ud39\">GPIO 8</td>\n"
            "    <td class=\"tg-w8l0\">3</td>\n"
            "    <td class=\"tg-w8l0\">4</td>\n"
            "    <td class=\"tg-ud39\">GPIO 9</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-amwm\">18</td>\n"
            "  </tr>", get_gpio_str(17), get_gpio_str(18));

    pi_string_sprintf(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\">19</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-ud39\">GPIO 10</td>\n"
            "    <td class=\"tg-w8l0\">5</td>\n"
            "    <td class=\"tg-w8l0\">6</td>\n"
            "    <td class=\"tg-ud39\">GPIO 10</td>\n"
            "    <td class=\"tg-baqh\">%s</td>\n"
            "    <td class=\"tg-amwm\">20</td>\n"
            "  </tr>", get_gpio_str(19), get_gpio_str(20));

    pi_string_append_str(response_body, "<tr>\n"
            "    <td class=\"tg-amwm\"></td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-5ncq\">GND</td>\n"
            "    <td class=\"tg-w8l0\">7</td>\n"
            "    <td class=\"tg-w8l0\">8</td>\n"
            "    <td class=\"tg-5ncq\">GND</td>\n"
            "    <td class=\"tg-baqh\"></td>\n"
            "    <td class=\"tg-amwm\">11</td>\n"
            "  </tr>");
}

void http_output_config(pi_string_t *response_body) {
    pi_string_sprintf(response_body, "<P><B>Configuration Settings:</B><P/>\r\n");
    pi_string_sprintf(response_body, "<ul><li>Run Minutes: %u</li>", get_run_minutes());
    pi_string_sprintf(response_body, "<li>Sleep Minutes: %u</li>", get_sleep_minutes());
    pi_string_sprintf(response_body, "<li>GPIO Pin: %u</li>", (unsigned int) get_pin());
    pi_string_sprintf(response_body, "<li>daemon: %s</li>", get_run_as_daemon() ? "true" : "false");
    pi_string_sprintf(response_body, "<li>Port: %d</li>", (int) get_server_port());
    pi_string_sprintf(response_body, "<li>Time Remaining in Minutes: %d</li></ul>", get_minutes_remaining());
}

void http_html_monitor_page(pi_string_t *response) {

    pi_string_t *response_body = pi_string_new(1024);

    pi_string_sprintf(response_body, "<HTML><TITLE>Timer</TITLE>");
    pi_string_sprintf(response_body, "<BODY>");

    http_output_config(response_body);
    http_output_pins(response_body);

    pi_string_sprintf(response_body, "</BODY></HTML>");

    pi_string_sprintf(response, "HTTP/1.0 200 OK\r\n");
    pi_string_sprintf(response, "Server: %s\r\n", get_pi_timer_version());
    pi_string_sprintf(response, "Content-Type: text/html\r\n");
    pi_string_sprintf(response, "Connection: close\r\n");
    pi_string_sprintf(response, "Content-Length: %d\r\n", pi_string_c_string_length(response_body));
    pi_string_sprintf(response, "\r\n%s", pi_string_c_string(response_body));

    pi_string_delete(response_body, true);
}


void http_output_health_check(pi_string_t *response) {

    pi_string_t *response_body = pi_string_new(256);

    pi_string_sprintf(response_body, "{\"status\":\"UP\"");

    pi_string_sprintf(response, "HTTP/1.0 200 OK\r\n");
    pi_string_sprintf(response, "Server: %s\r\n", get_pi_timer_version());
    pi_string_sprintf(response, "Transfer-Encoding: Identity\r\n");
    pi_string_sprintf(response, "Content-Type: application/json;charset=UTF-8\r\n");
    pi_string_sprintf(response, "Connection: close\r\n");
    pi_string_sprintf(response, "Content-Length: %d\r\n", pi_string_c_string_length(response_body));
    pi_string_sprintf(response, "\r\n%s", pi_string_c_string(response_body));

    pi_string_delete(response_body, true);
}

void http_output_build_info(pi_string_t *response) {

    pi_string_t *response_body = pi_string_new(256);

    pi_string_sprintf(response_body, "{\"version\":\"%s\", \"name\":\"pi-timer\"}", get_pi_timer_version());

    pi_string_sprintf(response, "HTTP/1.0 200 OK\r\n");
    pi_string_sprintf(response, "Server: %s\r\n", get_pi_timer_version());
    pi_string_sprintf(response, "Content-Type: application/json;charset=UTF-8\r\n");
    pi_string_sprintf(response, "Connection: close\r\n");
    pi_string_sprintf(response, "Content-Length: %d\r\n", pi_string_c_string_length(response_body));
    pi_string_sprintf(response, "\r\n%s", pi_string_c_string(response_body));

    pi_string_delete(response_body, true);
}

void http_not_found(pi_string_t *response) {

    pi_string_t *response_body = pi_string_new(256);

    pi_string_sprintf(response_body, "<HTML><TITLE>Not Found</TITLE>\r\n");
    pi_string_sprintf(response_body, "<BODY><P>The server could not fulfill\r\n");
    pi_string_sprintf(response_body, "your request because the resource specified\r\n");
    pi_string_sprintf(response_body, "is unavailable or nonexistent.</P>\r\n");
    pi_string_sprintf(response_body, "</BODY></HTML>\r\n");
    pi_string_sprintf(response_body, "\r\n");

    pi_string_sprintf(response, "HTTP/1.0 404 NOT FOUND\r\n");
    pi_string_sprintf(response, "Server: %s\r\n", get_pi_timer_version());
    pi_string_sprintf(response, "Content-Type: text/html\r\n");
    pi_string_sprintf(response, "Connection: close\r\n");
    pi_string_sprintf(response, "Content-Length: %d\r\n", pi_string_c_string_length(response_body));
    pi_string_sprintf(response, "\r\n%s", pi_string_c_string(response_body));

    pi_string_delete(response_body, true);
}

bool pi_timer_service_json_response(str_map_t __unused *headers) {
    return false;
}

void http_output_response(pi_string_t *request_path, str_map_t *headers, pi_string_t *response) {

    bool json_response = pi_timer_service_json_response(headers);

    if (request_path && 0 == strncmp(pi_string_c_string(request_path), "/health", strlen("/health"))) {
        // Do Health Checks
        //
        http_output_health_check(response);
    }
    else if (request_path && 0 == strncmp(pi_string_c_string(request_path), "/buildInfo", strlen("/buildInfo"))) {
        // Output Build information
        //
        http_output_build_info(response);
    }
    else if (request_path && 0 == strncmp(pi_string_c_string(request_path), "/", strlen("/"))) {
        // Just output stats.
        //
        if (!json_response) {
            http_html_monitor_page(response);
        }
    }
    else {
        http_not_found(response);
    }
}

void http_send_response(int socket, pi_string_t *response) {
    if (NULL != response) {
        if (pi_string_c_string_length(response) > 0) {
            send(socket,
                 pi_string_c_string(response),
                 pi_string_c_string_length(response), 0);
        }
    }
}

int debug_startup_connection() {

    struct addrinfo hints;
    memory_clear(&hints, sizeof hints);

    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    char port_string[16];   // needs to fit a short
    memory_clear(port_string, sizeof port_string);
    snprintf(port_string, sizeof port_string, "%d", get_server_port());

    struct addrinfo *response = NULL;
    getaddrinfo(NULL, port_string, &hints, &response);

    int socket_fd = socket(response->ai_family, response->ai_socktype, response->ai_protocol);

    if (socket_fd == -1) {
        ERROR_LOG("Unable to create socket, this is either a network issue where the port %"
                          " is already in use or a bug in the service.", get_server_port());
    }
    else {
        // The setsockopt() function is used to allow the local address to
        // be reused when the server is restarted before the required wait
        // time expires.
        //
        int option_one = 1;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
                       (char *) &option_one, sizeof(option_one)) < 0) {
            ERROR_LOG("Setsockopt(SO_REUSEADDR) failed, this is either"
                              " a network issue or a bug in the service.");
        }

        if (bind(socket_fd, response->ai_addr, response->ai_addrlen) < 0) {
            ERROR_LOG("Bind failed on socket %d, this is either a network "
                              "issue or a bug in the service", socket_fd);
            close(socket_fd);
            socket_fd = -1;
        }

        // The backlog argument (5) defines the maximum length to which the queue of pending connections for
        // sockfd may grow. If a connection request arrives when the queue is full, the client may receive an
        // error with an indication of ECONNREFUSED or, if the underlying protocol supports
        // retransmission, the request may be ignored so that a later reattempt at connection succeeds.
        //

        if (listen(socket_fd, 5) < 0) {
            ERROR_LOG("listen debug failed");
            close(socket_fd);
            socket_fd = -1;
        }
    }

    return socket_fd;
}

// List of methods
//
typedef enum {
    http_invalid,
    http_get,
    http_post,
    http_delete,
    http_put,
    http_options,
    http_head,
    http_trace,
    http_connect
} http_method_t;

http_method_t http_map_string_to_method(pi_string_t *request_buffer) {
    http_method_t result = http_invalid;
    const char *method = pi_string_c_string(request_buffer);

    if (0 == strncasecmp(method, "GET", 3)) {
        result = http_get;
    }
    else if (0 == strncasecmp(method, "POST", 4)) {
        result = http_post;
    }
    else if (0 == strncasecmp(method, "PUT", 3)) {
        result = http_put;
    }
    else if (0 == strncasecmp(method, "DELETE", 6)) {
        result = http_delete;
    }
    else if (0 == strncasecmp(method, "OPTIONS", 7)) {
        result = http_options;
    }
    else if (0 == strncasecmp(method, "HEAD", 4)) {
        result = http_head;
    }
    else if (0 == strncasecmp(method, "TRACE", 5)) {
        result = http_trace;
    }
    else if (0 == strncasecmp(method, "CONNECT", 7)) {
        result = http_connect;
    }

    return result;
}

pi_string_t *http_parse_path(pi_string_t *request_buffer) {
    const char *query = pi_string_c_string(request_buffer);

    // Skip Method
    //
    while (*query != '\0' && *query != ' ' && *query != '\t') {
        ++query;
    }

    // Skip Spaces
    //
    while (*query != '\0' && (*query == ' ' || *query == '\t')) {
        ++query;
    }

    pi_string_t *request_path = pi_string_new(256);

    // Extract the path
    //
    while (*query != '\0' && *query != '?' && *query != ' ' && *query != '\t') {
        pi_string_append_char(request_path, *query);
        query++;
    }

    return request_path;
}

char *clean_string(char *value) {
    char *zap = strrchr(value, '\n');
    if (zap) *zap = 0;
    zap = strrchr(value, '\r');
    if (zap) *zap = 0;

    if (*value == ' ') {
        value++;
    }

    return value;
}

size_t parse_headers(int client_socket, str_map_t *headers) {
    size_t content_size = 0;

    // Read the headers...
    //
    pi_string_t *string_buffer_ptr = pi_string_new(64);

    while (http_read_line(client_socket, string_buffer_ptr)) {
        // Find the key and the value
        //
        char *key = pi_string_c_string(string_buffer_ptr);
        char *value = pi_string_c_string(string_buffer_ptr);

        // Look for the ':'
        //
        for (; *value && *value != ':'; value++);

        // Get the "Value
        //
        if (*value == ':') {
            *value = 0;
            value++;
        }

        key = clean_string(key);
        value = clean_string(value);

        if (*key && *value) {
            pi_strmap_put(headers, key, value);
        }

        // Check to see if we have hit the end...
        //
        if (*key == '\0') {
            // We hit the end!
            //
            break;
        }

        // OK look of content length
        //
        if (0 == strncmp(key, "Content-Length", 32)) {
            content_size = (size_t) atol(value);
        }
    }

    pi_string_delete(string_buffer_ptr, true);
    return content_size;
}


void *debug_thread(void __unused *arg) {

    INFO_LOG("Starting debug thread on port %hu", get_server_port());

    if (get_server_port()) {
        int socket_fd = debug_startup_connection();

        if (-1 != socket_fd) {
            INFO_LOG("[INFO] Service has taking the stage on port %d", get_server_port());

            pi_string_t *request_buffer = NULL;

            while (get_service_running()) {
                request_buffer = pi_string_new(1024);

                struct sockaddr_in sockaddr_client;
                socklen_t sockaddr_client_length = sizeof(sockaddr_client);
                memory_clear(&sockaddr_client, sockaddr_client_length);

                int client_socket = accept(socket_fd, (struct sockaddr *) &sockaddr_client, &sockaddr_client_length);

                http_read_line(client_socket, request_buffer);

                http_method_t method = http_map_string_to_method(request_buffer);

                pi_string_t *request_path = http_parse_path(request_buffer);

                pi_string_t *response_buffer = pi_string_new(256);

                str_map_t *headers = pi_strmap_new(32);

                parse_headers(client_socket, headers);

                switch (method) {
                    case http_get:
                        http_output_response(request_path, headers, response_buffer);
                        break;
                    default:
                        http_not_found(response_buffer);
                        break;
                }

                http_send_response(client_socket, response_buffer);

                close(client_socket);

                pi_string_delete(response_buffer, true);
                pi_string_delete(request_path, true);
                pi_strmap_delete(headers);
            }
            pi_string_delete(request_buffer, true);

            close(socket_fd);
        }
        else {
            ERROR_LOG("[ERROR] Service was unable to take the stage on port %d", get_server_port());
        }
    }

    return NULL;
}

pthread_t g_debug_thread_id = 0;

void pi_timer_service_start() {
    if (get_server_port()) {
        pthread_create(&g_debug_thread_id, NULL, &debug_thread, NULL);
    }
}

