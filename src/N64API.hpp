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
    if (!gc_init(pin, &status))
    {
        // Reset in any case, as some bytes may have been written.
        reset();
        return false;
    }

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


// TODO
// bool CN64Controller::getRumble(void)
// {
//     // Read controller (rumble) state
//     return status.rumble;
// }


// TODO
// bool CN64Controller::setRumble(bool rumble)
// {
//     // Read controller (rumble) state and set new state
//     bool oldRumble = getRumble();
//     status.rumble = rumble;
//     return oldRumble;
// }


bool CN64Controller::end(void)
{
    // Try to init the controller
    // TODO rumble
    if (connected() /* || getRumble() */)
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

// TODO
