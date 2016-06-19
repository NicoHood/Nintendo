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
// Gamecube Definitions
//================================================================================

#include "Gamecube_N64.h"

// Gamecube controller device status ids
// You need to switch the bytes for the docs which are widely available online.
// A default Gamecube controller would look like 0x0900.
#define NINTENDO_DEVICE_GC_WIRED     0x0009
#define NINTENDO_DEVICE_GC_NONE      0x0000


//================================================================================
// Gamecube Typedefs
//================================================================================

typedef union{
    // 8 bytes of datareport that we get from the controller
    uint8_t raw8[8];
    uint16_t raw16[0];
    uint32_t raw32[0];

    struct{
        uint8_t buttons0;
        union{
            uint8_t buttons1;
            uint8_t dpad : 4;
        };
    };

    struct {
        // first data byte (bitfields are sorted in LSB order)
        uint8_t a : 1;
        uint8_t b : 1;
        uint8_t x : 1;
        uint8_t y : 1;
        uint8_t start : 1;
        uint8_t origin : 1; // Indicates if GetOrigin(0x41) was called (LOW)
        uint8_t errlatch : 1;
        uint8_t errstat : 1;

        // second data byte
        uint8_t dleft : 1;
        uint8_t dright : 1;
        uint8_t ddown : 1;
        uint8_t dup : 1;
        uint8_t z : 1;
        uint8_t r : 1;
        uint8_t l : 1;
        uint8_t high1 : 1;

        // 3rd-8th data byte
        uint8_t xAxis;
        uint8_t yAxis;
        uint8_t cxAxis;
        uint8_t cyAxis;
        uint8_t left;
        uint8_t right;
    };
} Gamecube_Report_t;


// Gamecube an N64 use the same status schema
typedef Gamecube_N64_Status_t Gamecube_Status_t;


typedef union{
    // GetOrigin requests the initial values of the controller when it was plugged in (powered up)
    // This request consists of a normal data package and 2 unknown bytes.
    // It is possible that those mark the deadzone/toleranz, but are zero in my tests.
    // If the GetOrigin request was not sent to the controller, it will respond it in the data report.
    // The gamecube will NOT accept controller data when no GetOrigin request was made (origin bit in data report set).
    uint8_t raw8[10];
    uint16_t raw16[0];
    uint32_t raw32[0];

    struct{
        Gamecube_Report_t inititalData;
        uint8_t deadzone0;
        uint8_t deadzone1;
    };
} Gamecube_Origin_t;


struct Gamecube_Data_t{
    // All information required for reading/writing a Gamecube Controller
    Gamecube_Status_t status;
    Gamecube_Origin_t origin;
    Gamecube_Report_t report;
};


//================================================================================
// Gamecube Function Prototypes
//================================================================================

#ifdef __cplusplus
extern "C" {
#endif

// Functions to communicate with the gc controller
bool gc_init(const uint8_t pin, Gamecube_Status_t* status);
bool gc_origin(const uint8_t pin, Gamecube_Origin_t* origin);
bool gc_read(const uint8_t pin, Gamecube_Report_t* report, const bool rumble);
uint8_t gc_write(const uint8_t pin, Gamecube_Status_t* status, Gamecube_Origin_t* origin, Gamecube_Report_t* report);

#ifdef __cplusplus
}
#endif
