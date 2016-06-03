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
// N64
//================================================================================

N64_ N64;

N64_::N64_(void){
    // empty
}

bool N64_::begin(const uint8_t pin)
{
    // discard the information
    N64_Status_t status;
    return begin(pin, status);
}

bool N64_::begin(const uint8_t pin, N64_Status_t &status)
{
    // Initialize the N64 controller by sending it a null byte.
    // This is unnecessary for a standard controller, but some games do this
    uint8_t command[] = { 0x00 };

    // send the command and read in data
    uint8_t receivedBytes = gc_n64_send_get(pin, command, sizeof(command), (uint8_t*)&status, sizeof(status));

    // return status information for optional use
    bool newinput;
    if (receivedBytes == sizeof(status)){
        // switch the first two bytes to compare it easy with the documentation
        uint8_t temp = status.whole8[0];
        status.whole8[0] = status.whole8[1];
        status.whole8[1] = temp;

        newinput = true;
    }
    else
        newinput = false;
    return newinput;
}


bool N64_::end(const uint8_t pin){
    // Turns off rumble by sending a normal reading request
    // and discards the information
    N64_Data_t report;
    return read(pin, report, false);
}


bool N64_::read(const uint8_t pin, N64_Data_t &report, const bool rumble)
{
    // command to send to the N64
    uint8_t command[] = { 0x01 };

    // send the command and read in data
    uint8_t receivedBytes = gc_n64_send_get(pin, command, sizeof(command), (uint8_t*)&report, sizeof(report));

    // return status information for optional use
    bool newinput;
    if (receivedBytes == sizeof(report))
        newinput = true;
    else
        newinput = false;
    return newinput;
}
