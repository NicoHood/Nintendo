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

#include "Gamecube.h"

//================================================================================
// Gamecube Controller
//================================================================================

bool gc_init(const uint8_t pin, Gamecube_Status_t* status)
{
    // Initialize the gamecube controller by sending it a null byte.
    // This is unnecessary for a standard controller, but is required for the
    // Wavebird.
    uint8_t command[] = { 0x00 };

    // Send the command and read in data
    uint8_t receivedBytes = gc_n64_send_get(pin, command, sizeof(command), (uint8_t*)status, sizeof(Gamecube_Status_t));

    // Return status information for optional use.
    // On error the report may have been modified!
    return (receivedBytes == sizeof(Gamecube_Status_t));
}


bool gc_origin(const uint8_t pin, Gamecube_Origin_t* origin)
{
    // Get the initital controller values when it was plugged in.
    // This is used for calibration of the controller.
    // If you do not call this function the controller data bit "origin" will be set.
    // This results in a not accepting Gamecube host/reinitialization.
    uint8_t command[] = { 0x41 };

    // Send the command and read in data
    uint8_t receivedBytes = gc_n64_send_get(pin, command, sizeof(command), (uint8_t*)origin, sizeof(Gamecube_Origin_t));

    // Return status information for optional use.
    // On error the report may have been modified!
    return (receivedBytes == sizeof(Gamecube_Origin_t));
}


bool gc_read(const uint8_t pin, Gamecube_Report_t* report, const bool rumble)
{
    // Command to send to the gamecube, LSB is rumble
    uint8_t command[] = { 0x40, 0x03, rumble };

    // Rend the command and read in data
    uint8_t receivedBytes = gc_n64_send_get(pin, command, sizeof(command), (uint8_t*)report, sizeof(Gamecube_Report_t));

    // Return status information for optional use.
    // On error the report may have been modified!
    return (receivedBytes == sizeof(Gamecube_Report_t));
}


//================================================================================
// Gamecube Console
//================================================================================

uint8_t gc_write(const uint8_t pin, Gamecube_Status_t* status, Gamecube_Origin_t* origin, Gamecube_Report_t* report)
{
    // 0 = no input/error, 1 = init, 2 = origin, 3 = read, 4 = read with rumble
    uint8_t ret = 0;

    // Get the port mask and the pointers to the in/out/mode registers
    uint8_t bitMask = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    volatile uint8_t* modePort = portModeRegister(port);
    volatile uint8_t* outPort = portOutputRegister(port);
    volatile uint8_t* inPort = portInputRegister(port);

    // Don't want interrupts getting in the way
    uint8_t oldSREG = SREG;
    cli();

    // Read in data from the console
    // After receiving the init command you have max 80us to respond (for the data command)!
    uint8_t command[3] = {0,0,0}; // TODO do not init
    uint8_t receivedBytes = gc_n64_get(command, sizeof(command), modePort, outPort, inPort, bitMask);

    // Init
    if (receivedBytes == 1 && command[0] == 0x00)
    {
        gc_n64_send(status->raw8, sizeof(Gamecube_Status_t), modePort, outPort, bitMask);
        ret = 1;
    }
    // Get origin
    else if (receivedBytes == 1 && command[0] == 0x41)
    {
        gc_n64_send(origin->raw8, sizeof(Gamecube_Origin_t), modePort, outPort, bitMask);
        ret = 2;
    }
    // Get data. Do not check last byte (command[2]), as the flags are unknown
    else if (receivedBytes == 3 && command[0] == 0x40 && command[1] == 0x03 \
        && (command[2] == 0x00 || command[2] == 0x01))
    {
        gc_n64_send(report->raw8, sizeof(Gamecube_Report_t), modePort, outPort, bitMask);
        ret = 3;
        if(command[2] & 0x01){
            ret = 4;
        }
    }

    // End of time sensitive code
    SREG = oldSREG;

    return ret;
}
