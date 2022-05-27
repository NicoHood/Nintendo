/*
Copyright (c) 2014-2021 NicoHood
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
    // Command to send to the gamecube controller, LSB is rumble
    uint8_t command[] = { 0x40, 0x03, rumble };

    // Send the command and read in data
    uint8_t receivedBytes = gc_n64_send_get(pin, command, sizeof(command), (uint8_t*)report, sizeof(Gamecube_Report_t));

    // Return status information for optional use.
    // On error the report may have been modified!
    return (receivedBytes == sizeof(Gamecube_Report_t));
}

bool gc_read_keyboard(const uint8_t pin, Gamecube_Report_t* report)
{
    // Command to send to the gamecube keyboard
    uint8_t command[] = { 0x54, 0x00, 0x00 };

    // Send the command and read in data
    uint8_t receivedBytes = gc_n64_send_get(pin, command, sizeof(command), (uint8_t*)report, sizeof(Gamecube_Report_t));

    // Return status information for optional use.
    // On error the report may have been modified!
    return (receivedBytes == sizeof(Gamecube_Report_t));
}


//================================================================================
// Gamecube Console
//================================================================================

// This function converts a report from reading mode 3 (default) to a specified target mode.
// Details of the different reading modes can be found here:
// https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/Core/HW/SI/SI_DeviceGCController.cpp#L167
// Mode 0 was tested with Pokemon XD
void gc_report_convert(Gamecube_Report_t* report, Gamecube_Report_t* dest_report, uint8_t mode)
{
    memcpy(dest_report, report, sizeof(Gamecube_Report_t));
    if (mode == 1)
    {
        dest_report->mode1.cxAxis = report->cxAxis >> 4;
        dest_report->mode1.cyAxis = report->cyAxis >> 4;
        dest_report->mode1.left = report->left;
        dest_report->mode1.right = report->right;
        dest_report->mode1.analogA = 0;
        dest_report->mode1.analogB = 0;
    }
    else if (mode == 2)
    {
        dest_report->mode2.cxAxis = report->cxAxis >> 4;
        dest_report->mode2.cyAxis = report->cyAxis >> 4;
        dest_report->mode2.left = report->left >> 4;
        dest_report->mode2.right = report->right >> 4;
        dest_report->mode2.analogA = 0;
        dest_report->mode2.analogB = 0;
    }
    else if (mode == 4)
    {
        dest_report->mode4.cxAxis = report->cxAxis;
        dest_report->mode4.cyAxis = report->cyAxis;
        dest_report->mode4.analogA = 0;
        dest_report->mode4.analogB = 0;
    }
    else if (mode == 3)
    {
        return;
    }
    // Mode 0, 5, 6, 7
    else
    {
        dest_report->mode0.cxAxis = report->cxAxis;
        dest_report->mode0.cyAxis = report->cyAxis;
        dest_report->mode0.left = report->left >> 4;
        dest_report->mode0.right = report->right >> 4;
        dest_report->mode0.analogA = 0;
        dest_report->mode0.analogB = 0;
    }
}

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
    uint8_t command[3];
    uint8_t receivedBytes = gc_n64_get(command, sizeof(command), modePort, outPort, inPort, bitMask);

    // Init or reset
    if (receivedBytes == 1 && (command[0] == 0x00 || command[0] == 0xFF))
    {
        gc_n64_send(status->raw8, sizeof(Gamecube_Status_t), modePort, outPort, bitMask);
        ret = 1;
    }
    // Get origin or recalibrate
    else if (receivedBytes == 1 && (command[0] == 0x41 || command[0] == 0x42))
    {
        gc_n64_send(origin->raw8, sizeof(Gamecube_Origin_t), modePort, outPort, bitMask);
        ret = 2;
    }
    // Get data. Do not check last byte (command[2]), as the flags are unknown
    else if (receivedBytes == 3 && command[0] == 0x40 && command[1] <= 0x07)
    {
        Gamecube_Report_t dest_report;
        gc_report_convert(report, &dest_report, command[1]);
        gc_n64_send(dest_report.raw8, sizeof(dest_report), modePort, outPort, bitMask);
        ret = 3;
        // The first byte probably flags a gamecube reading (0x40), as the same
        // protocol is also used for N64. The lower nibble seems to be the mode:
        // 0x40 (followed by 2 bytes) reading
        // 0x41 get origin (1 byte)
        // 0x42 (followed by 2 bytes) seems to force mode 0x02 below
        // 0x43 (followed by 2 bytes) seems to force mode 0x02 below

        // The 2nd byte (command[1]) seems to request a special data format.
        // I've noticed formats that merge the L + R data.
        // There seem to be only 4 data formats, the rest is ignore.
        // 0x00 First 4 bytes: buttons0+1 + X + Y, C-Stick, L+R minimum of both, 0x00 fixed
        // 0x01 First 4 bytes: buttons0+1 + X + Y, C-Stick Horizontal only, R, L, 0x00 fixed
        // 0x02 First 4 bytes: buttons0+1 + X + Y, C-Stick Horizontal only, L+R minimum of both, 0x02 fixed, 0x01 fixed
        // 0x03 Normal reading

        // I've seen 3 last options for the last byte (command[2]):
        // 0x00 Normal reading
        // 0x01 Enable rumble
        // 0x02 Normal reading, rumble was at least called once
        // 0x03 ??? - never seen so far
        // Rumble: 1, 5, 9, 13, 17, ...
        // You can see that only 4 of those requests are possible,
        // the gamecube will ignore the left 6 MSB.
        if((command[2] % 4) == 0x01){
            ret = 4;
        }
        else if((command[2] % 4) == 0x02){
            ret = 5;
        }
        else if((command[2] % 4) == 0x03){
            ret = 6;
        }
    }

    // End of time sensitive code
    SREG = oldSREG;

    return ret;
}
