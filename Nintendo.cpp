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


#include "Nintendo.h"

//================================================================================
// Gamecube
//================================================================================

Gamecube_ Gamecube;

Gamecube_::Gamecube_(void){
	// empty
}


bool Gamecube_::begin(const uint8_t pin, Gamecube_Status_t &status)
{
	// get the port mask and the pointers to the in/out/mode registers
	uint8_t bitMask = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t* modePort = portModeRegister(port);
	volatile uint8_t* outPort = portOutputRegister(port);
	volatile uint8_t* inPort = portInputRegister(port);

	// Initialize the gamecube controller by sending it a null byte.
	// This is unnecessary for a standard controller, but is required for the
	// Wavebird.
	uint8_t command[] = { 0x00 };

	// don't want interrupts getting in the way
	noInterrupts();

	// send the command
	gc_send((uint8_t*)&command, sizeof(command), modePort, outPort, bitMask);

	// read in data
	uint8_t receivedBytes = gc_get((uint8_t*)&status, sizeof(status), modePort, outPort, inPort, bitMask);

	// end of time sensitive code
	interrupts();

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
	// get the port mask and the pointers to the in/out/mode registers
	uint8_t bitMask = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t* modePort = portModeRegister(port);
	volatile uint8_t* outPort = portOutputRegister(port);
	volatile uint8_t* inPort = portInputRegister(port);

	// Turns off rumble by sending a normal reading request
	// and discards the information
	uint8_t command[sizeof(Gamecube_Data_t)] = { 0x40, 0x03, 0x00 };

	// don't want interrupts getting in the way
	noInterrupts();

	// send the command (only send the first 3 bytes use the buffer twice)
	gc_send((uint8_t*)&command, 3, modePort, outPort, bitMask);

	// read in new data, even though we do not use it
	uint8_t receivedBytes = gc_get((uint8_t*)&command, sizeof(command), modePort, outPort, inPort, bitMask);

	// end of time sensitive code
	interrupts();

	// return status information for optional use
	bool newinput;
	if (receivedBytes == sizeof(Gamecube_Data_t))
		newinput = true;
	else
		newinput = false;
	return newinput;

	/*
	// this version takes more flash and is more complicated

	// prepare pin for input with pullup
	*modePort &= ~bitMask;
	*outPort |= bitMask;

	// Stupid routine to wait for the gamecube controller to stop
	// sending its response. We don't care what it is, but we
	// can't start asking for status if it's still responding
	for (uint8_t forceBreak = 0; forceBreak < 2 * 8 * sizeof(Gamecube_Data_t); forceBreak++) {
		// get the new input
		uint8_t newInput = *inPort & bitMask;

		// make sure the line is idle for 10 iterations,
		// should be plenty. (about 7uS)
		uint8_t i = 10;
		while (--i){
			if ((*inPort & bitMask) != newInput)
				break;
		}
		if (!i) break;
	}
	// TODO return value
	*/
}


bool Gamecube_::read(const uint8_t pin, Gamecube_Data_t &report, const bool rumble)
{
	// get the port mask and the pointers to the in/out/mode registers
	uint8_t bitMask = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t* modePort = portModeRegister(port);
	volatile uint8_t* outPort = portOutputRegister(port);
	volatile uint8_t* inPort = portInputRegister(port);

	// command to send to the gamecube, LSB is rumble
	uint8_t command[] = { 0x40, 0x03, rumble & 0x01 };

	// don't want interrupts getting in the way
	noInterrupts();

	// send the command
	gc_send((uint8_t*)&command, sizeof(command), modePort, outPort, bitMask);

	// read in new data
	uint8_t receivedBytes = gc_get((uint8_t*)&report, sizeof(report), modePort, outPort, inPort, bitMask);

	// end of time sensitive code
	interrupts();

	// return status information for optional use
	bool newinput;
	if (receivedBytes == sizeof(report))
		newinput = true;
	else
		newinput = false;
	return newinput;
}


//================================================================================
// Gamecube/N64 i/o functions
//================================================================================

/**
 * This sends the given byte sequence to the controller
 * length must be at least 1
 */
void gc_send(uint8_t* buff, uint8_t len,
	volatile uint8_t* modePort, volatile uint8_t* outPort, uint8_t bitMask)
{
	// set pin to output, default high
	*outPort |= bitMask;
	*modePort |= bitMask;

	// temporary register values used as "clobbers"
	register uint8_t bitCount;
	register uint8_t data;

	asm volatile (
		"; Start of gc_send assembly\n"

		// passed in to this block are:
		// the %a[buff] register is the buffer pointer
		// %[len] is the register holding the length of the buffer in bytes

		// Instruction cycles are noted in parentheses
		// branch instructions have two values, one if the branch isn't
		// taken and one if it is

		// %[data] will be the current buffer byte loaded from memory
		// %[bitCount] will be the bit counter for the current byte. when this
		// reaches 0, we need to decrement the length counter, load
		// the next buffer byte, and loop. (if the length counter becomes
		// 0, that's our exit condition)

		// This label starts the outer loop, which sends a single byte
		".L%=_byte_loop:\n"
		"ld %[data], %a[buff]+\n" // (2) load the next byte and increment byte pointer
		"ldi %[bitCount],0x08\n" // (1) set bitcount to 8 bits

		// This label starts the inner loop, which sends a single bit
		".L%=_bit_loop:\n"
		"st %a[outPort],%[low]\n" // (2) pull the line low

		// line needs to stay low for 1탎 for a 1 bit, 3탎 for a 0 bit
		// this block figures out if the next bit is a 0 or a 1
		// the strategy here is to shift the register left, then test and
		// branch on the carry flag
		"lsl %[data]\n" // (1) shift left. MSB goes into carry bit of status reg
		"brcc .L%=_zero_bit\n" // (1/2) branch if carry is cleared


		// this block is the timing for a 1 bit (1탎 low, 3탎 high)
		// Stay low for 16 - 2 (above lsl,brcc) - 2 (below st) = 12 cycles
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\n" // (2)
		"st %a[outPort],%[high]\n" // (2) set the line high again
		// Now stay high for 2탎 of the 3탎 to sync up with the branch below
		// 2*16 - 2 (for the rjmp) = 30 cycles
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"rjmp .L%=_finish_bit\n" // (2)


		// this block is the timing for a 0 bit (3탎 low, 1탎 high)
		// Need to go high in 3*16 - 3 (above lsl,brcc) - 2 (below st) = 43 cycles
		".L%=_zero_bit:\n"
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\n" // (3)
		"st %a[outPort],%[high]\n" // (2) set the line high again


		// The two branches meet up here.
		// We are now *exactly* 3탎 into the sending of a bit, and the line
		// is high again. We have 1탎 to do the looping and iteration
		// logic.
		".L%=_finish_bit:\n"
		"subi %[bitCount],0x01\n" // (1) subtract 1 from our bit counter
		"breq .L%=_load_next_byte\n" // (1/2) branch if we've sent all the bits of this byte

		// At this point, we have more bits to send in this byte, but the
		// line must remain high for another 1탎 (minus the above
		// instructions and the jump below and the st instruction at the
		// top of the loop)
		// 16 - 2(above) - 2 (rjmp below) - 2 (st after jump) = 10
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"rjmp .L%=_bit_loop\n" // (2)


		// This block starts 3 cycles into the last 1탎 of the line being high
		// We need to decrement the byte counter. If it's 0, that's our exit condition.
		// If not we need to load the next byte and go to the top of the byte loop
		".L%=_load_next_byte:\n"
		"subi %[len], 0x01\n" // (1) len--
		"breq .L%=_loop_exit\n" // (1/2) if the byte counter is 0, exit
		// delay block:
		// needs to go high after 1탎 or 16 cycles
		// 16 - 5 (above) - 2 (the jump itself) - 5 (after jump) = 4
		"nop\nnop\nnop\nnop\n" // (4)
		"rjmp .L%=_byte_loop\n" // (2)


		// Loop exit
		".L%=_loop_exit:\n"

		// final task: send the stop bit, which is a 1 (1탎 low 3탎 high)
		// the line goes low in:
		// 16 - 6 (above since line went high) - 2 (st instruction below) = 8 cycles
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\n" // (3)
		"st %a[outPort],%[low]\n" // (2) pull the line low
		// stay low for 1탎
		// 16 - 2 (below st) = 14
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\n" // (4)
		"st %a[outPort],%[high]\n" // (2) set the line high again
		// just stay high. no need to wait 3탎 before returning

		// ----------
		// outputs:
		: [buff] "+e" (buff), // (read and write)
		[outPort] "+e" (outPort), // (read and write)
		[bitCount] "=&d" (bitCount), // (output only, ldi needs the upper registers)
		[data] "=&r" (data) // (output only)

		// inputs:
		: [len] "d" (len), // (subi needs the upper registers)
		[high] "r" (*outPort | bitMask), // precalculate new pin states
		[low] "r" (*outPort & ~bitMask) // this works because we turn interrupts off

		// no clobbers
		); // end of asm volatile
}


/**
* Read bytes from the gamecube controller
* listen for the expected bytes of data back from the controller and
* and pack it into the buff
*/
uint8_t gc_get(uint8_t* buff, uint8_t len,
	volatile uint8_t* modePort, volatile uint8_t* outPort, volatile uint8_t * inPort, uint8_t bitMask)
{
	// prepare pin for input with pullup
	*modePort &= ~bitMask;
	*outPort |= bitMask;

	// temporary register values used as "clobbers"
	register uint8_t timeoutCount; // counts down the timeout
	register uint8_t bitCount; // counts down 8 bits for each byte
	register uint8_t inputVal; // temporary variable to save the pin states
	register uint8_t data; // keeps the temporary received data byte
	register uint8_t receivedBytes; // the return value of the function

	asm volatile (
		"; Start of gc_get assembly\n"

		// [bitCount] is our bit counter. We read %[len] bytes
		// and increment the byte pointer and receivedBytes every 8 bits
		"ldi %[bitCount],0x08\n" // (1) set bitcount to 8 bits
		"ldi %[receivedBytes],0x00\n" // (1) default exit value is 0 bytes received

		// This first spinloop waits for the line to go low.
		// It loops 64 times before it gives up and returns
		".L%=_wait_for_low:\n"
		"ldi %[timeoutCount],%[timeout]\n" // (1) set the timeout
		".L%=_wait_for_low_loop:\n" // 7 cycles if loop fails
		"ld %[inputVal], %a[inPort]\n" // (2) read the pin (happens before the 2 cycles)
		"and %[inputVal], %[bitMask]\n" // (1) compare pinstate with bitmask
		"breq .L%=_wait_for_measure\n" // (1/2) jump to the measure part if pin is low
		// the following happens if the line is still high
		"dec %[timeoutCount]\n" // (1) decrease timeout by 1
		"brne .L%=_wait_for_low_loop\n" // (1/2) loop if the counter isn't 0
		"rjmp .L%=_exit\n" // (2) timeout, jump to the end


		// Next block. The line has just gone low. Wait approx 2탎
		// each cycle is 1/16 탎 on a 16Mhz processor
		// best case: 32 - 5 (above) - 1 (below) = 26 nops
		// worst case: 32 - 5 (above) - 6 (above, worst case) - 1 (below) = 20 nops
		// --> 23 nops
		".L%=_wait_for_measure:\n"
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\nnop\nnop\n" // (5)
		"nop\nnop\nnop\n" // (3)
		// save the data
		"lsl %[data]\n" // (1) left shift the current byte in %[data]
		"ld %[inputVal], %a[inPort]\n" // (2) read the pin (happens before the 2 cycles)
		"and %[inputVal], %[bitMask]\n" // (1) compare pinstate with bitmask
		"breq .L%=_check_bit_count\n" // (1/2) skip setting data to 1 if pin is low
		"sbr %[data],0x01\n" // set bit 1 in %[data] if pin is high
		".L%=_check_bit_count:\n"
		"dec %[bitCount]\n" // (1) decrement 1 from our bit counter
		"brne .L%=_wait_for_high\n" // (1/2) branch if we've not received the whole byte

		// we received a full byte
		"st %a[buff]+,%[data]\n" // (2) save %[data] back to memory and increment byte pointer
		"inc %[receivedBytes]\n" // (1) increase byte count
		"ldi %[bitCount],0x08\n" // (1) set bitcount to 8 bits again
		"cp %[len],%[receivedBytes]\n" // (1) %[len] == %[receivedBytes] ?
		"breq .L%=_exit\n" // (1/2) jump to exit if we received all bytes
		// dont wait for line to go high again


		// This next block waits for the line to go high again.
		// again, it sets a timeout counter of 64 iterations
		".L%=_wait_for_high:\n"
		"ldi %[timeoutCount],%[timeout]\n" // (1) set the timeout
		".L%=_wait_for_high_loop:\n" // 7 cycles if loop fails
		"ld %[inputVal], %a[inPort]\n" // (2) read the pin (happens before the 2 cycles)
		"and %[inputVal], %[bitMask]\n" // (1) compare pinstate with bitmask
		"brne .L%=_wait_for_low\n" // (1/2) line is high. ready for next loop
		// the following happens if the line is still low
		"dec %[timeoutCount]\n" // (1) decrease timeout by 1
		"brne .L%=_wait_for_high_loop\n" // (1/2) loop if the counter isn't 0
		// timeout, exit now
		".L%=_exit:\n"

		// ----------
		// outputs:
		: [receivedBytes] "=&d" (receivedBytes), // (ldi needs the upper registers)
		[buff] "+e" (buff), // (read and write)
		[bitCount] "=&d" (bitCount), // (output only, ldi needs the upper registers)
		[timeoutCount] "=&r" (timeoutCount), // (output only)
		[inputVal] "=&r" (inputVal), // (output only)
		[data] "=&r" (data) // (output only)

		// inputs
		: [len] "r" (len),
		[inPort] "e" (inPort),
		[bitMask] "r" (bitMask),
		[timeout] "M" (0x64) // constant
		); // end of asm volatile

	return receivedBytes;
}