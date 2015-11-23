/*
Copyright (c) 2015 Otamay
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

Gamecube::Gamecube(void){
}

bool Gamecube::attach(const byte pin){
  this->pin = pin;
  // Initialize the gamecube controller by sending it a null byte.
	// This is unnecessary for a standard controller, but is required for the
	// Wavebird.
  uint8_t command[] = { 0x00 };
	uint8_t receivedBytes = gc_n64_send_get(this->pin, command, sizeof(command), (uint8_t*)&(this->status), sizeof(this->status));
  if (receivedBytes == sizeof(this->status)){
    // switch the first two bytes to compare it easy with the documentation
		uint8_t temp = this->status.whole8[0];
		this->status.whole8[0] = this->status.whole8[1];
		this->status.whole8[1] = temp;

    this->device = this->status.device;
    return true;
	}
  return false;
}

void Gamecube::setForce(const byte force){
  this->force = force;
}
void Gamecube::setRumble(const bool rumble){
  this->rumble = rumble;
}

bool Gamecube::read(){
  uint8_t command[3];
  switch (this->device){
    // command to send to the gamecube
  case NINTENDO_DEVICE_GC_WHEEL:
    command[0] = 0x30;
    command[1] = 0x06;
    command[2] = this->force;
    break;
  default:
  // case NINTENDO_DEVICE_GC_WIRED:
    command[0] = 0x40;
    command[1] = 0x03;
    command[2] = this->rumble & 0x01;
  //   break;
  // default:
  //   return false;
  }

  // send the command and read in data
  uint8_t receivedBytes = gc_n64_send_get(this->pin, command, sizeof(command), (uint8_t*)&(this->report), sizeof(this->report));

  if (receivedBytes == sizeof(this->report)){
    this->a = this->report.a;
    this->b = this->report.b;
    this->x = this->report.x;
    this->y = this->report.y;
    this->start = this->report.start;
    this->dup = this->report.dup;
    this->ddown = this->report.ddown;
    this->dleft = this->report.dleft;
    this->dright = this->report.dright;
    this->z = this->report.z;
    this->r = this->report.r;
    this->l = this->report.l;
    this->xAxis = this->report.xAxis;
    this->yAxis = this->report.yAxis;
    this->cxAxis = this->report.cxAxis;
    this->cyAxis = this->report.cyAxis;
    this->left = this->report.left;
    this->right = this->report.right;  

    // return status information for optional use
		return true;
  }
  return false;
}

Gamecube_Status_t Gamecube::status;
Gamecube_Data_t Gamecube::report;
