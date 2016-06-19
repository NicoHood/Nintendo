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

#include "Gamecube.h"

// Default data for an original, initialized, centered Gamecube controller
static constexpr Gamecube_Data_t defaultGamecubeData = {
    .status = { 0x09, 0x00, 0x03 },
    .origin = { 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x1F, 0x1F, 0x00, 0x00 },
    .report = { 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x1F, 0x1F }
};

//================================================================================
// Gamecube Controller API
//================================================================================

class CGamecubeController : protected Gamecube_Data_t {
public:
    inline CGamecubeController(const uint8_t p);
    inline void reset(void);
    inline bool begin(void);
    inline uint16_t getDevice(void);
    inline bool connected(void);
    inline bool read(void);
    inline bool getRumble(void);
    inline bool setRumble(bool rumble);
    inline bool end(void);
    inline Gamecube_Status_t getStatus(void);
    inline Gamecube_Origin_t getOrigin(void);
    inline Gamecube_Report_t getReport(void);
    inline Gamecube_Data_t getData(void);
    // TODO add a calibrate with origin function

protected:
    const uint8_t pin;
    friend class CGamecubeConsole;
};


//================================================================================
// Gamecube Host API
//================================================================================

class CGamecubeConsole{
public:
    inline CGamecubeConsole(const uint8_t p);
    inline bool write(Gamecube_Data_t &data);
    inline bool write(CGamecubeController &controller);
    inline bool write(Gamecube_Report_t &report);

protected:
    const uint8_t pin;
};

#include "GamecubeAPI.hpp"
