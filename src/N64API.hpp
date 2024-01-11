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

// Include guard
#pragma once

//================================================================================
// N64 Controller API
//================================================================================

CN64Controller::CN64Controller(const uint8_t p) : pin(p){
    // Empty
}


void CN64Controller::reset(void)
{
    // Reenumerate controller next time
    status = N64_Status_t();
}


bool CN64Controller::begin(void)
{
    // Try to init the controller
    if (!n64_init(pin, &status))
    {
        // Reset in any case, as some bytes may have been written.
        reset();
        return false;
    }

    // From https://sites.google.com/site/consoleprotocols/home/nintendo-joy-bus-documentation
    // 0x01 Something is plugged into the port under the standard controller
    // 0x02 Nothing is plugged into the port under the standard controller
    // 0x04 Controller Read/Write CRC had an error.

    // Something is connected on the expansion port. Lets assume that it's a rumble pak.
    if (status.status0 == 0x01) {
        // Initialize rumble pak
        // Rumble code from:
        // https://github.com/DavidPagels/retro-pico-switch/blob/master/src/otherController/n64/N64Controller.cpp
        delayMicroseconds(100);
        n64_writeRumble(pin, 0xee);

        delayMicroseconds(100);
        n64_writeRumble(pin, 0x80);

        delayMicroseconds(100);
        n64_setRumble(pin, false);
    }
    
    // A small delay is required between begin() and read()
    delayMicroseconds(100);

    // No error
    return true;
}


uint16_t CN64Controller::getDevice(void)
{
    // (Optional) swap the first two bytes to compare it easy with the documentation
    return status.device;
}


bool CN64Controller::connected(void)
{
    // Check if the device identifier is at least not zero
    return (status.device != NINTENDO_DEVICE_N64_NONE);
}


bool CN64Controller::read(void)
{
    // Check if the controller was initialized
    if (!connected())
    {
        // Try to init the controller
        if (!begin())
        {
            return false;
        }
    }

    //Need to enable/disable rumble?
    //only send command if something is connected on the expansion port
    static bool rumbleState = 0;
    if(status.status0 == 0x01 && status.rumble != rumbleState) {
        delayMicroseconds(100);
        n64_setRumble(pin, status.rumble);
        delayMicroseconds(100);
    }
    rumbleState = status.rumble;

    // Read the controller, abort if it fails.
    if (!n64_read(pin, &report))
    {
        reset();
        return false;
    }

    // Return status information for optional use.
    // On error the report may have been modified!
    return true;
}


bool CN64Controller::getRumble(void)
{
    // Read controller (rumble) state
    return status.rumble;
}


bool CN64Controller::setRumble(bool rumble)
{
    // Read controller (rumble) state and set new state
    bool oldRumble = getRumble();
    status.rumble = rumble;
    return oldRumble;
}


bool CN64Controller::end(void)
{
    // Try to init the controller
    // TODO rumble
    if (connected() || getRumble())
    {
        // Reset in any case, as some bytes may have been written.
        N64_Report_t tmp;
        return n64_read(pin, &tmp);
    }

    // Error: Already disconnected
    return false;
}


N64_Status_t CN64Controller::getStatus(void)
{
    return status;
}


N64_Report_t CN64Controller::getReport(void)
{
    return report;
}


N64_Data_t CN64Controller::getData(void)
{
    // Get the derivated data pointer, copy and return the data
    N64_Data_t* dataPtr = this;
    N64_Data_t data;
    memcpy(&data, dataPtr, sizeof(data));
    return data;
}


//================================================================================
// N64 Console API
//================================================================================

CN64Console::CN64Console(const uint8_t p) : pin(p){
    // Empty
}


bool CN64Console::write(N64_Data_t &data)
{
    // Don't want interrupts getting in the way
    uint8_t oldSREG = SREG;
    cli();

    // Write a respond to the N64, depending on what it requests
    uint8_t ret = n64_write(pin, &data.status, &data.report);

    // Init
    if(ret == 1)
    {
        // Try to answer a following read request
        ret = n64_write(pin, &data.status, &data.report);
    }

    // End of time sensitive code
    SREG = oldSREG;

    // Return error if no reading was possible
    return false;
}


bool CN64Console::write(CN64Controller &controller)
{
    // Cast controller to its protected (friend) data
    N64_Data_t& data = controller;
    return write(data);
}


bool CN64Console::write(N64_Report_t &report)
{
    // Inititalize status and report with default values
    N64_Data_t data = defaultN64Data;

    // Copy report into the temporary struct and write to N64
    memcpy(&data.report, &report, sizeof(data.report));
    return write(data);
}
