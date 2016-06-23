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

#include "N64.h"

//================================================================================
// N64 Controller
//================================================================================

bool n64_init(const uint8_t pin, N64_Status_t* status)
{
    // Initialize the N64 controller by sending it a null byte.
    // This is unnecessary for a standard controller, but the console does this.
    uint8_t command[] = { 0x00 };

    // Send the command and read in data
    uint8_t receivedBytes = gc_n64_send_get(pin, command, sizeof(command), (uint8_t*)status, sizeof(N64_Status_t));

    // Return status information for optional use.
    // On error the report may have been modified!
    return (receivedBytes == sizeof(N64_Status_t));
}


bool n64_read(const uint8_t pin, N64_Report_t* report)
{
    // Command to send to the N64
    uint8_t command[] = { 0x01 };

    // Send the command and read in data
    uint8_t receivedBytes = gc_n64_send_get(pin, command, sizeof(command), (uint8_t*)report, sizeof(N64_Report_t));

    // Return status information for optional use.
    // On error the report may have been modified!
    return (receivedBytes == sizeof(N64_Report_t));
}


//================================================================================
// N64 Console
//================================================================================

// TODO
