#**********************************************************************
#    Copyright (c) 2016 Henry Seurer & Samuel Kelly
#
#    Permission is hereby granted, free of charge, to any person
#    obtaining a copy of this software and associated documentation
#    files (the "Software"), to deal in the Software without
#    restriction, including without limitation the rights to use,
#    copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the
#    Software is furnished to do so, subject to the following
#    conditions:
#
#    The above copyright notice and this permission notice shall be
#    included in all copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#    OTHER DEALINGS IN THE SOFTWARE.
#
#**********************************************************************

cmake_minimum_required(VERSION 3.0)
project(pi-timer)

set(PI_TIMER_VERSION "pi-timer-version-v1.0.0")

include(CheckIncludeFile)
CHECK_INCLUDE_FILE("/opt/vc/include/bcm_host.h" HAVE_BCMHOST_H)

# Set The correct compiler
#
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "-std=gnu99")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS "-std=gnu99 -lwiringPi -pthread -lrt")
endif ()

# Versioning
#
configure_file(version_config.h.in ${CMAKE_BINARY_DIR}/generated/version_config.h)
include_directories(${CMAKE_BINARY_DIR}/generated/)

set(SOURCE_FILES
        main.c
        pi_timer_server.c
        pi_string.c
        pi_timer_server.h
        pi_string.h
        pi_utils.c
        pi_utils.h
        pi_timer_settings.c
        pi_timer_settings.h
        pi_timer_gpio.c
        pi_timer_gpio.h
        pi_strmap.c
        pi_strmap.h)

add_executable(pi-timer ${SOURCE_FILES})