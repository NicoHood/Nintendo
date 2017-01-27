/*
Copyright (c) 2014-2016 NicoHood
See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

// Include guard
#pragma once

#include <Arduino.h>

//================================================================================
// Typedefs
//================================================================================

typedef union{
    // 3 bytes of statusreport that we get from the controller
    uint8_t raw8[3];
    uint16_t raw16[0];
    struct {
        // Device information, needs to be swapped to fit the documentation below:
        // 15 wireless (1: wireless Controller)
        // 14 wireless receive (0: not wireless 1: wireless)
        // 13 Rumble Motor (0: available 1: not available)
        // 12 Type of Controller (always 0 ?)
        // 11 Type of Controller (0: N64 1: Dolphin)
        // 10 wireless type (0:IF 1:RF)
        //  9 wireless state (0: variable 1: fixed)
        //  8 0: non standard Controller 1: Dolphin Standard Controller
        //  7
        //  6
        //  5 wireless origin (0:invalid 1:valid)
        //  4 wireless fix id (0:not fixed 1:fixed)
        //  3 wireless type - 0:normal 1: non-controller (?)
        //  2 wireless type - 0:normal 1: lite controller
        //  1 wireless type -
        //  0 wireless type -
        uint16_t device;

        // controller status (only rumble is known)
        union{
            uint8_t status;
            struct{
                uint8_t status0 : 3;
                uint8_t rumble : 1;
                uint8_t status1 : 4;
            };
        };
    };
} Gamecube_N64_Status_t;

//================================================================================
// Function prototypes
//================================================================================

#ifdef __cplusplus
extern "C" {
#endif

// functions to communicate with the gc/n64 controller
uint8_t gc_n64_send_get(const uint8_t pin, uint8_t* command, const uint8_t commandLen,
    uint8_t* report, const uint8_t reportLen) __attribute__((noinline));

void gc_n64_send(const uint8_t* buff, uint8_t len,
    volatile uint8_t* modePort, volatile uint8_t* outPort, uint8_t bitMask)
    __attribute__((noinline));

uint8_t gc_n64_get(uint8_t* buff, uint8_t len,
    volatile uint8_t* modePort, volatile uint8_t* outPort, volatile uint8_t * inPort, uint8_t bitMask)
    __attribute__((noinline));

#ifdef __cplusplus
}
#endif
