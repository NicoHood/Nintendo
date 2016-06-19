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

#include "N64.h"

// Default data for an original, initialized, centered N64 controller
// TODO
// static constexpr N64_Data_t defaultN64Data = {
//     .status = { 0x05, 0x00, ??? },
//     .report = { ??? }
// };

//================================================================================
// N64 Controller API
//================================================================================

class CN64Controller : protected N64_Data_t {
public:
    inline CN64Controller(const uint8_t p);
    inline void reset(void);
    inline bool begin(void);
    inline uint16_t getDevice(void);
    inline bool connected(void);
    inline bool read(void);
    inline bool getRumble(void);
    //inline bool setRumble(bool rumble);
    inline bool end(void);
    inline N64_Status_t getStatus(void);
    inline N64_Report_t getReport(void);
    inline N64_Data_t getData(void);

protected:
    const uint8_t pin;
    friend class CN64Console;
};


//================================================================================
// N64 Host API
//================================================================================

// TODO
// class CN64Console{
// public:
//     inline CN64Console(const uint8_t p);
//     inline bool write(N64_Data_t &data);
//     inline bool write(CN64Controller &controller);
//     inline bool write(N64_Report_t &report);
//
// protected:
//     const uint8_t pin;
// };

#include "N64API.hpp"
