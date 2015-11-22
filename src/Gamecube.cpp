/*
Copyright (c) 2014-2015 NicoHood
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
// Gamecube
//================================================================================

Gamecube_ Gamecube;

Gamecube_::Gamecube_(void){
	// empty
}

bool Gamecube_::begin(const uint8_t pin)
{
	// discard the information
	Gamecube_Status_t status;
	return begin(pin, status);
}

bool Gamecube_::begin(const uint8_t pin, Gamecube_Status_t &status)
{
	// Initialize the gamecube controller by sending it a null byte.
	// This is unnecessary for a standard controller, but is required for the
	// Wavebird.
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


bool Gamecube_::end(const uint8_t pin){
	// Turns off rumble by sending a normal reading request
	// and discards the information
	Gamecube_Data_t report;
  Gamecube_Status_t status;
	return read(pin, status, report, false);
}


bool Gamecube_::read(const uint8_t pin, Gamecube_Status_t &status, Gamecube_Data_t &report, const bool rumble)
{  
  uint8_t command[3];
  switch (status.device){
  case NINTENDO_DEVICE_GC_WHEEL:
    command[0] = 0x30;
    command[1] = 0x00;
    command[2] = 0x00;
    break;
  default:
    // command to send to the gamecube, LSB is rumble
    command[0] = 0x40;
    command[1] = 0x03;
    command[2] = rumble & 0x01;
  }

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
