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
// Gamecube Controller API
//================================================================================

CGamecubeController::CGamecubeController(const uint8_t p) : pin(p){
    // Empty
}


void CGamecubeController::reset(void)
{
    // Reenumerate controller next time
    status = Gamecube_Status_t();
}


bool CGamecubeController::begin(void)
{
    // Try to init the controller
    if (!gc_init(pin, &status))
    {
        // Reset in any case, as some bytes may have been written.
        reset();
        return false;
    }

    // If initialization was successful also get the original controller stats
    if (!gc_origin(pin, &origin))
    {
        return false;
    }

    // No error
    return true;
}


uint16_t CGamecubeController::getDevice(void)
{
    // (Optional) swap the first two bytes to compare it easy with the documentation
    return status.device;
}


bool CGamecubeController::connected(void)
{
    // Check if the device identifier is at least not zero
    return (status.device != NINTENDO_DEVICE_GC_NONE);
}


bool CGamecubeController::read(void)
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
    // Additional information: If you press X + Y + Start on the controller for 3 seconds
    // It will turn off unless you release the buttons. The recalibration is all done
    // on the Gamecube side. If the controller resets origin will have different values for sure.
    if (!gc_read(pin, &report, status.rumble))
    {
        reset();
        return false;
    }

    // Check if controller reported that we read the origin values (check if it disconnected).
    // The Gamecube would just request (instantly) the origin again, but we keep things simple.
    if (report.origin) {
        reset();
        return false;
    }

    // Return status information for optional use.
    // On error the report may have been modified!
    return true;
}


bool CGamecubeController::getRumble(void)
{
    // Read controller (rumble) state
    return status.rumble;
}


bool CGamecubeController::setRumble(bool rumble)
{
    // Read controller (rumble) state and set new state
    bool oldRumble = getRumble();
    status.rumble = rumble;
    return oldRumble;
}


bool CGamecubeController::end(void)
{
    // Try to init the controller
    if (connected() || getRumble())
    {
        // Reset in any case, as some bytes may have been written.
        Gamecube_Report_t tmp;
        return gc_read(pin, &tmp, false);
    }

    // Error: Already disconnected
    return false;
}


Gamecube_Status_t CGamecubeController::getStatus(void)
{
    return status;
}


Gamecube_Origin_t CGamecubeController::getOrigin(void)
{
    return origin;
}


Gamecube_Report_t CGamecubeController::getReport(void)
{
    return report;
}


Gamecube_Data_t CGamecubeController::getData(void)
{
    // Get the derivated data pointer, copy and return the data
    Gamecube_Data_t* dataPtr = this;
    Gamecube_Data_t data;
    memcpy(&data, dataPtr, sizeof(data));
    return data;
}


//================================================================================
// Gamecube Console API
//================================================================================

CGamecubeConsole::CGamecubeConsole(const uint8_t p) : pin(p){
    // Empty
}


bool CGamecubeConsole::write(Gamecube_Data_t &data)
{
    // Abort if controller was not initialized.
    // Gamecube will refuse and weird connect/disconnect errors will occur.
    if (data.report.origin) {
        return false;
    }

    // Don't want interrupts getting in the way
    uint8_t oldSREG = SREG;
    cli();

    // Write a respond to the gamecube, depending on what it requests
    uint8_t ret = gc_write(pin, &data.status, &data.origin, &data.report);

    // Init
    if(ret == 1)
    {
        // Try to answer a possible following origin request
        ret = gc_write(pin, &data.status, &data.origin, &data.report);
    }

    // Origin
    if(ret == 2){
        // Try to answer a possible following read request
        ret = gc_write(pin, &data.status, &data.origin, &data.report);
    }

    // End of time sensitive code
    SREG = oldSREG;

    // Set rumble depending on read return value
    if (ret == 3) {
        data.status.rumble = false;
        return true;
    }
    else if (ret == 4) {
        data.status.rumble = true;
        return true;
    }
    else if (ret == 5) {
        data.status.rumble = false;
        return true;
    }

    // Return error if no reading was possible
    return false;
}


bool CGamecubeConsole::write(CGamecubeController &controller)
{
    // Cast controller to its protected (friend) data
    Gamecube_Data_t& data = controller;
    return write(data);
}


bool CGamecubeConsole::write(Gamecube_Report_t &report)
{
    // Inititalize init and origin with default values
    Gamecube_Data_t data = defaultGamecubeData;

    // Copy report into the temporary struct and write to gamecube
    memcpy(&data.report, &report, sizeof(data.report));
    return write(data);
}
