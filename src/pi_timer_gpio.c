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

#include "pi_timer_gpio.h"
#include "pi_timer_settings.h"
#include "pi_utils.h"

#ifndef HAVE_BCMHOST_H
#define ENABLE_PI_EMULATOR 1
#endif

#ifndef ENABLE_PI_EMULATOR

#include <wiringPi.h>

#else

#define gpio_pin_count 26
gpio_signal gpio_list[gpio_pin_count];

#endif

void setup_wring_pi() {
    DEBUG_LOG("Setting output pin mode on %d", get_pin());

#ifndef ENABLE_PI_EMULATOR
    wiringPiSetup();
    pinMode((int) get_pin(), OUTPUT);
#else
    INFO_LOG("WARNING: Running in emulation mode");

    for (int i = 0; i < gpio_pin_count; i++) {
        gpio_list[i] = LOW_SIGNAL;
    }
#endif
}

gpio_signal get_gpio(unsigned char pin) {
    gpio_signal result = LOW_SIGNAL;

#ifndef ENABLE_PI_EMULATOR
    result = digitalRead((int) pin) == LOW ? LOW_SIGNAL : HIGH_SIGNAL;
#else
    if (pin < gpio_pin_count) {
        result = gpio_list[pin];
    }
#endif

    return result;
}

const char *get_gpio_str(unsigned char pin) {
    return get_gpio(pin) == LOW_SIGNAL ? "LOW" : "HIGH";
}

gpio_signal set_gpio(unsigned char pin, gpio_signal value) {
    gpio_signal result = LOW_SIGNAL;

#ifndef ENABLE_PI_EMULATOR
    result = digitalRead((int) pin);
    digitalWrite((int) pin, value == LOW_SIGNAL ? LOW : HIGH);
#else
    if (pin < gpio_pin_count) {
        result = gpio_list[pin];
        gpio_list[pin] = value;
    }
#endif

    return result;
}