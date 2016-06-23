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
// N64 Definitions
//================================================================================

#include "Gamecube_N64.h"

// N64 controller device status ids
// You need to switch the bytes for the docs which are widely available online.
// A default N64 controller would look like 0x0500.
#define NINTENDO_DEVICE_N64_WIRED   0x0005
#define NINTENDO_DEVICE_N64_NONE    0x0000


//================================================================================
// N64 Typedefs
//================================================================================

typedef union{
    // 4 bytes of datareport that we get from the controller
    uint8_t raw8[4];
    uint16_t raw16[0];
    uint32_t raw32[0];

    struct{
        uint8_t dpad : 4;
        uint8_t buttons0 : 4;
        uint8_t cpad : 4;
        uint8_t buttons1 : 4;
    };

    struct {
        // first data byte (bitfields are sorted in LSB order)
        uint8_t dright : 1;
        uint8_t dleft : 1;
        uint8_t ddown : 1;
        uint8_t dup : 1;
        uint8_t start : 1;
        uint8_t z : 1;
        uint8_t b : 1;
        uint8_t a : 1;

        // second data byte
        uint8_t cright : 1;
        uint8_t cleft : 1;
        uint8_t cdown : 1;
        uint8_t cup : 1;
        uint8_t r : 1;
        uint8_t l : 1;
        uint8_t low1 : 1;
        uint8_t low0 : 1;

        // 3rd-4th data byte
        int8_t xAxis;
        int8_t yAxis;
    };
} N64_Report_t;

// Gamecube an N64 use the same status schema
typedef Gamecube_N64_Status_t N64_Status_t;

struct N64_Data_t{
    // All information required for reading/writing a N64 Controller
    N64_Status_t status;
    N64_Report_t report;
};


//================================================================================
// N64 Function Prototypes
//================================================================================

#ifdef __cplusplus
extern "C" {
#endif

// Functions to communicate with the gc controller
bool n64_init(const uint8_t pin, N64_Status_t* status);
bool n64_read(const uint8_t pin, N64_Report_t* report);

#ifdef __cplusplus
}
#endif
