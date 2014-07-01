/*
Copyright (c) 2014 NicoHood
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

/**
* Gamecube controller to Nintendo 64 adapter
* by Andrew Brown
*/

#ifndef NINTENDO_H
#define NINTENDO_H

#include <Arduino.h>

//================================================================================
// Settings
//================================================================================

// Do you use a Logic Level converter? (if not use a 1k 3.3V pullup)
// ***not supported, use level converter!!!***
#define LOGIC_LEVEL_CONVERTER


//================================================================================
// Definitions
//================================================================================

// these two macros set arduino pin 2 to input or output, which with an
// external 1K pull-up resistor to the 3.3V rail, is like pulling it high or
// low.
#ifndef LOGIC_LEVEL_CONVERTER
// ***not supported, use level converter!!!***
#endif

//================================================================================
// Report Typedefinitions
//================================================================================

typedef union{
	// 8 bytes of datareport that we get from the controller
	uint8_t whole8[8];
	uint16_t whole16[8 / 2];
	uint32_t whole32[8 / 4];

	struct{
		uint8_t buttons1;
		uint8_t buttons2;
	};

	struct {
		// first data byte (Bitfields are sorted in LSB order)
		uint8_t a : 1;
		uint8_t b : 1;
		uint8_t x : 1;
		uint8_t y : 1;
		uint8_t start : 1;
		uint8_t LOW3 : 1;
		uint8_t LOW2 : 1;
		uint8_t LOW1 : 1;

		// second data byte
		uint8_t dleft : 1;
		uint8_t dright : 1;
		uint8_t ddown : 1;
		uint8_t dup : 1;
		uint8_t z : 1;
		uint8_t r : 1;
		uint8_t l : 1;
		uint8_t HIGH1 : 1;

		// 3rd-8th data byte
		uint8_t xAxis;
		uint8_t yAxis;
		uint8_t cxAxis;
		uint8_t cyAxis;
		uint8_t left;
		uint8_t right;
	};
} Gamecube_Data_t;

typedef union{
	// 3 bytes of statusreport that we get from the controller
	uint8_t whole8[3];
	uint16_t whole16[3 / 2];
	struct {
		// Device information
		uint16_t device;

		// Controller status (only rumble is known)
		uint8_t status1 : 3;
		uint8_t rumble : 1;
		uint8_t status2 : 4;
	};
} Gamecube_Status_t;

//================================================================================
// Gamecube Class
//================================================================================

class Gamecube_{
public:
	Gamecube_(void);
	bool begin(uint8_t pin);
	bool init(void);
	void end(void);

	// user read/write functions
	bool read(bool rumble = 0); // default no rumble
	void write(void);

	// writes the dump in a passed in array(in this case the report structs)
	void translate_raw_data(uint8_t raw_dumb[], void* data, uint8_t length);

	// writes variables to the intern report
	inline void xAxis(uint8_t a){ report.xAxis = a; }
	inline void yAxis(uint8_t a){ report.yAxis = a; }
	inline void cxAxis(uint8_t a){ report.cxAxis = a; }
	inline void cyAxis(uint8_t a){ report.cyAxis = a; }

	// functions to communicate with the controller
	// public for debug access
	bool sendget(uint8_t *buffer, uint8_t sendlength, uint8_t getlength,
		volatile uint8_t* modePort, volatile uint8_t* outPort, volatile uint8_t * inPort, uint8_t bitMask);

	void send(uint8_t *buffer, uint8_t length,
		volatile uint8_t* modePort, volatile uint8_t* outPort, uint8_t bitMask);

	bool get(uint8_t *buffer, uint8_t length,
		volatile uint8_t* modePort, volatile uint8_t* outPort, volatile uint8_t * inPort, uint8_t bitMask);


	// Structs that stores the controller states
	Gamecube_Status_t status;
	Gamecube_Data_t report;

private:
	uint8_t _bitMask;
	volatile uint8_t * _outPort;
	volatile uint8_t * _inPort;
	volatile uint8_t * _modePort;

};

extern Gamecube_ Gamecube;

#endif