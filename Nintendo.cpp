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

#include "Nintendo.h"

//================================================================================
// Gamecube Init
//================================================================================

Gamecube_ Gamecube;

Gamecube_::Gamecube_(void){
	// prevents sendget from being called without calling begin()
	_bitMask = 0;
	// clears the reports
	memset(&status, 0x00, sizeof(status));
	memset(&report, 0x00, sizeof(report));
}

bool Gamecube_::begin(uint8_t pin){
	// Get the port mask and the pointers to the in/out/mode registers
	_bitMask = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	_outPort = portOutputRegister(port);
	_inPort = portInputRegister(port);
	_modePort = portModeRegister(port);

	// Initialize the gamecube controller by sending it a null byte.
	// This is unnecessary for a standard controller, but is required for the
	// Wavebird.

	// return status information for optional use
	return init();
}

bool Gamecube_::init(void){
	// only read values if begin() was called before
	if (!_bitMask) return false;

	// Initialize the gamecube controller by sending it a null byte.
	// This is unnecessary for a standard controller, but is required for the
	// Wavebird.

	// 1 received bit per byte raw input dump
	// for fast measurement

	// the dump is used for sending and getting new information in the same array
	//uint8_t raw_dump[sizeof(status)* 8] = { 0x00 };

	uint8_t raw_dump[sizeof(status)* 8];
	uint8_t command[] = { 0x00 };

	// don't want interrupts getting in the way
	noInterrupts();

	// send those byte, read data and dump it into raw_dump
	//bool newinput = sendget(raw_dump, 1, sizeof(raw_dump), _modePort, _outPort, _inPort, _bitMask);

	// send the command
	send(command, sizeof(command), _modePort, _outPort, _bitMask);

	// read in data and dump it to raw_dump
	bool newinput = get(raw_dump, sizeof(raw_dump), _modePort, _outPort, _inPort, _bitMask);

	// end of time sensitive code
	interrupts();

	// only translate the data if the input was valid
	if (newinput){
		// translate the data in raw_dump to something useful
		translate_raw_data(raw_dump, &status, sizeof(status));

		// switch the first two bytes to compare it easy with the documentation
		uint8_t temp = status.whole8[0];
		status.whole8[0] = status.whole8[1];
		status.whole8[1] = temp;
	}

	// return status information for optional use
	return newinput;
}

void Gamecube_::end(void){
	// only read values if begin() was called before
	if (!_bitMask) return;

	// Turns off rumble by sending a normal reading request
	// and discards the information

	// 1 received bit per byte raw input dump
	// for fast measurement

	// the dump is used for sending and getting new information in the same array
	//uint8_t raw_dump[sizeof(report)* 8] = { 0x40, 0x03, 0x00 };

	uint8_t command[] = { 0x40, 0x03, 0x00 };

	// don't want interrupts getting in the way
	noInterrupts();

	// send the command
	send(command, sizeof(command), _modePort, _outPort, _bitMask);

	// end of time sensitive code
	interrupts();

	// Stupid routine to wait for the gamecube controller to stop
	// sending its response. We don't care what it is, but we
	// can't start asking for status if it's still responding
	for (int i = 0; i < 10; i++) {
		// make sure the line is idle for 10 iterations, should
		// be plenty. (about 7uS)
		if (!(*_inPort & _bitMask))
			i = 0;
	}

	// reset that you need to call begin() again first
	_bitMask = 0;
}

//================================================================================
// Gamecube Read
//================================================================================

bool Gamecube_::read(bool rumble){
	// only read values if begin() was called before
	if (!_bitMask) return false;

	// Command to send to the gamecube
	// The last bit is rumble

	// 1 received bit per byte raw input dump
	// for fast measurement

	// the dump is used for sending and getting new information in the same array
	//uint8_t raw_dump[sizeof(report)* 8] = { 0x40, 0x03, rumble & 0x01 };

	uint8_t raw_dump[sizeof(report)* 8];
	uint8_t command[] = { 0x40, 0x03, rumble & 0x01 };

	// don't want interrupts getting in the way
	noInterrupts();

	// send those bytes, read data and dump it into raw_dump
	//bool newinput = sendget(raw_dump, 3, sizeof(raw_dump), _modePort, _outPort, _inPort, _bitMask);

	// send the command
	send(command, sizeof(command), _modePort, _outPort, _bitMask);

	// read in data and dump it to raw_dump
	bool newinput = get(raw_dump, sizeof(raw_dump), _modePort, _outPort, _inPort, _bitMask);

	// end of time sensitive code
	interrupts();

	// only translate the data if the input was valid
	if (newinput){
		// translate the data in raw_dump to something useful
		translate_raw_data(raw_dump, &report, sizeof(report));
	}

	// return status information for optional use
	return newinput;
}


void Gamecube_::translate_raw_data(uint8_t raw_dumb[], void* data, uint8_t length){
	// The get function sloppily dumps its data 1 bit per byte
	// into the get_status_extended char array. It's our job to go through
	// that and put each piece neatly into the struct
	uint8_t* report = (uint8_t*)data;
	memset(report, 0, length);

	for (int i = 0; i < length * 8; i++){
		report[i / 8] |= raw_dumb[i] ? (0x80 >> (i % 8)) : 0;
		//raw_dump[i] ? data[i/8] |= (0x80 >> (i%8)) :  data[i/8] &= ~(0x80 >> (i%8));
	}
}

//================================================================================
// Gamecube i/o functions
//================================================================================

//================================================================================
// Send
//================================================================================

/**
* This sends the given byte sequence to the controller
* length must be at least 1
* Oh, it destroys the buffer passed in as it sends it
* it fills the buff with the answer from the controller
*/
void Gamecube_::send(uint8_t *buffer, uint8_t length,
	volatile uint8_t* modePort, volatile uint8_t* outPort, uint8_t bitMask){

	// Prepare pin for output and pull it high
	*outPort |= bitMask; //HIGH
	*modePort |= bitMask; //OUT
	*outPort |= bitMask; //HIGH

	// This routine is very carefully timed by examining the assembly output.
	// Do not change any statements, it could throw the timings off
	//
	// We get 16 cycles per microsecond, which should be plenty, but we need to
	// be conservative. Most assembly ops take 1 cycle, but a few take 2
	//
	// I use manually constructed for-loops out of gotos so I have more control
	// over the outputted assembly. I can insert nops where it was impossible
	// with a for loop

	// Starting outer for loop
outer_loop:
	{
		// Starting inner for loop
		// Send these bytes
		uint8_t bits = 8;
	inner_loop:
		{
			// Starting a bit, set the line low
			*outPort &= ~bitMask; //LOW, 5 cycles

			// branching
			if (*buffer >> 7) { //8 cycles to enter
				// Bit is a 1, 2 cycles
				// remain low for 1us, then go high for 3us
				// nop block 1
				asm volatile ("nop\n");

				// Setting line to high
				*outPort |= bitMask; //HIGH, 5 cycles

				// nop block 2
				// we'll wait only 2us to sync up with both conditions
				// at the bottom of the if statement
				asm volatile ("nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					);

			}
			else {
				// Bit is a 0, 3 cycles to enter
				// remain low for 3us, then go high for 1us
				// nop block 3
				asm volatile ("nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\n");

				// Setting line to high
				*outPort |= bitMask; //HIGH, 5 cycles

				// wait for 1us
				// end of conditional branch, need to wait 1us more before next bit
			}
			// end of the if, the line is high and needs to remain
			// high for exactly 16 more cycles, regardless of the previous
			// branch path

			// finishing inner loop body
			--bits;
			if (bits != 0) {
				// nop block 4
				// this block is why a for loop was impossible
				asm volatile ("nop\nnop\n");
				// rotating out bits
				*buffer <<= 1;

				goto inner_loop;
			} // fall out of inner loop
		}
		// continuing outer loop
		// In this case: the inner loop exits and the outer loop iterates,
		// there are /exactly/ 16 cycles taken up by the necessary operations.
		// So no nops are needed here (that was lucky!)
		// Due to the modification the 2 unnecessary cycles somehow disappeared
		// and this information might be wrong

		--length;
		if (length != 0) {
			++buffer;
			// nop block 5 (added)
			//asm volatile ("nop\n");
			goto outer_loop;
		} // fall out of outer loop
	}

	// send a single stop (1) bit
	// nop block 6
	asm volatile ("nop\nnop\nnop\nnop\nnop\n");
	*outPort &= ~bitMask; //LOW, 5 cycles

	// wait 1 us, 16 cycles, then raise the line 
	// nop block 7
	asm volatile ("nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\n");
	*outPort |= bitMask; //HIGH, 5 cycles
}

//================================================================================
// Get
//================================================================================

bool Gamecube_::get(uint8_t *buffer, uint8_t length,
	volatile uint8_t* modePort, volatile uint8_t* outPort, volatile uint8_t * inPort, uint8_t bitMask){

	// Prepare pin for input with pullup
	*modePort &= ~bitMask; //IN
	*outPort |= bitMask; //HIGH, 5 cycles

	// listen for the expected 8 bytes of data back from the controller and
	// blast it out to the raw_dump array, one bit per byte for extra speed.
	// Afterwards, call translate_raw_data() to interpret the raw data and pack
	// it into the struct.
	// Starting to listen
	uint8_t timeout;

	// Again, using gotos here to make the assembly more predictable and
	// optimization easier (please don't kill me)
get_loop:
	// the Port thing takes way longer, so timeout is 5 loops
	// this means after 4uS it will timeout
	timeout = 0x05;
	// wait for line to go low
	while (*inPort & bitMask) {
		timeout--;
		if (!timeout)
			return false;
	}

	// wait approx 1,5us and poll the line
	asm volatile (
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\n"
		);

	*buffer = *inPort & bitMask;

	++buffer;
	--length;
	if (length == 0)
		return true;

	// wait for line to go high again 1,5uS
	asm volatile (
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\n"
		);
	goto get_loop;
}


/**
* This sends the given byte sequence to the controller
* length must be at least 1
* Oh, it destroys the buffer passed in as it sends it
* it fills the buff with the answer from the controller
*/
bool Gamecube_::sendget(uint8_t *buffer, uint8_t sendlength, uint8_t getlength,
	volatile uint8_t* modePort, volatile uint8_t* outPort, volatile uint8_t * inPort, uint8_t bitMask){

	// only read values if begin() was called before
	if (!bitMask) return false;

	//================================================================================
	// Send
	//================================================================================


	// We use this buffer for sending and getting the values. We need to copy the
	// beginning of the buffer to use it with get after.
	// Send + get in one function for more speed + accuracy
	uint8_t *sendbuffer = buffer;

	// Prepare pin for output and pull it high
	*outPort |= bitMask; //HIGH
	*modePort |= bitMask; //OUT
	*outPort |= bitMask; //HIGH

	// This routine is very carefully timed by examining the assembly output.
	// Do not change any statements, it could throw the timings off
	//
	// We get 16 cycles per microsecond, which should be plenty, but we need to
	// be conservative. Most assembly ops take 1 cycle, but a few take 2
	//
	// I use manually constructed for-loops out of gotos so I have more control
	// over the outputted assembly. I can insert nops where it was impossible
	// with a for loop

	// Starting outer for loop
outer_loop:
	{
		// Starting inner for loop
		// Send these bytes
		uint8_t bits = 8;
	inner_loop:
		{
			// Starting a bit, set the line low
			*outPort &= ~bitMask; //LOW, 5 cycles

			// branching
			if (*sendbuffer >> 7) { //8 cycles to enter
				// Bit is a 1, 2 cycles
				// remain low for 1us, then go high for 3us
				// nop block 1
				asm volatile ("nop\n");

				// Setting line to high
				*outPort |= bitMask; //HIGH, 5 cycles

				// nop block 2
				// we'll wait only 2us to sync up with both conditions
				// at the bottom of the if statement
				asm volatile ("nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					);

			}
			else {
				// Bit is a 0, 3 cycles to enter
				// remain low for 3us, then go high for 1us
				// nop block 3
				asm volatile ("nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\nnop\nnop\nnop\n"
					"nop\nnop\n");

				// Setting line to high
				*outPort |= bitMask; //HIGH, 5 cycles

				// wait for 1us
				// end of conditional branch, need to wait 1us more before next bit
			}
			// end of the if, the line is high and needs to remain
			// high for exactly 16 more cycles, regardless of the previous
			// branch path

			// finishing inner loop body
			--bits;
			if (bits != 0) {
				// nop block 4
				// this block is why a for loop was impossible
				asm volatile ("nop\nnop\n");
				// rotating out bits
				*sendbuffer <<= 1;

				goto inner_loop;
			} // fall out of inner loop
		}
		// continuing outer loop
		// In this case: the inner loop exits and the outer loop iterates,
		// there are /exactly/ 16 cycles taken up by the necessary operations.
		// So no nops are needed here (that was lucky!)
		// Due to the modification the 2 unnecessary cycles somehow disappeared
		// and this information might be wrong

		--sendlength;
		if (sendlength != 0) {
			++sendbuffer;
			// nop block 5 (added)
			//asm volatile ("nop\n");
			goto outer_loop;
		} // fall out of outer loop
	}

	// send a single stop (1) bit
	// nop block 6
	asm volatile ("nop\nnop\nnop\nnop\nnop\n");
	*outPort &= ~bitMask; //LOW, 5 cycles

	// wait 1 us, 16 cycles, then raise the line 
	// nop block 7
	asm volatile ("nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\n");
	*outPort |= bitMask; //HIGH, 5 cycles

	//================================================================================
	// Get
	//================================================================================

	// Prepare pin for input with pullup
	*modePort &= ~bitMask; //IN
	*outPort |= bitMask; //HIGH, 5 cycles

	// listen for the expected 8 bytes of data back from the controller and
	// blast it out to the raw_dump array, one bit per byte for extra speed.
	// Afterwards, call translate_raw_data() to interpret the raw data and pack
	// it into the struct.
	// Starting to listen
	uint8_t timeout;
	uint8_t *getbuffer = buffer;

	// Again, using gotos here to make the assembly more predictable and
	// optimization easier (please don't kill me)
get_loop:
	// the Port thing takes way longer, so timeout is 5
	// this means after 4uS it will timeout
	timeout = 0x05;
	// wait for line to go low
	while (*inPort & bitMask) {
		timeout--;
		if (!timeout)
			return false;
	}

	// wait approx 1,5us and poll the line
	asm volatile (
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\n"
		);

	*getbuffer = *inPort & bitMask;

	++getbuffer;
	--getlength;
	if (getlength == 0)
		return true;

	// wait for line to go high again 1,5uS
	asm volatile (
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\nnop\nnop\nnop\nnop\n"
		"nop\n"
		);
	goto get_loop;
}
