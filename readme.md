Arduino Nintendo Library 1.1
============================

This library is made to connect Nintendo Controllers to your Arduino very easy.
Make sure you grab the right hardware, tear off some cables and use your controllers
on your PC for example. The requirement are written in each library readme part.

**IDE 1.6 compatible!**

**The library supports the following devices:**
* Gamecube Host

**These features are planned:**
* Gamecube Device(todo)
* N64 Host(todo, I don't have any)
* N64 Device(todo, I don't have any)
* Wii-Mote (USB Host shield)
* Wii Nunchuk (I2C)
* Wii Classic Controller (I2C)
* Wii-Mote plus(USB Host shield)
* Wiiu Pro Controller (USB Host shield)
* SNES Controller (I don't have any)

Gamecube Library
===================

###Features

* IDE 1.6 compatible
* 16MHz compatible (8/20 possible soon)
* Host mode
 * Controller status readable
 * Controller report readable
 * Controller Rumble usable
* Device mode (todo)

### Hardware

The Gamecube controller uses 3.3V logic (bidirectional), 3.3V power, 5V rumble power.
**Make sure to use a logic level converter (it's 1€) to not burn your controller.**
This _is needed_ compared to [Brownans version](https://github.com/brownan/Gamecube-N64-Controller),
Using the In/Output method will cause weird delay in the sending and the pulse is 2cycles off sometimes.
This is needed for more accurate controller timings since the in/out version is 2 cycles off.

A 1k resistor on the 3.3V side is needed to pull up the line. See schematic below.
The rumble draws about 30mA from what I've measured.
Currently the library only works for 16MHz, but 20MHz could be possible and 8MHz with some tricks.

You better get an extension cable and cut it half. I used a Lioncast cable and the colors were
**(can be different to yours!)**:
```
1 Yellow: 5V
2 White:  Data
3 Red:    Gnd
4 Brown:  Gnd
5 Green:  NC
6 Blue:   3.3V
7 Black:  Gnd
```

Cut the extension cable and solder some jumper wires to connect them to a breadboard.
Make sure they don't connect with each other like in the picture. You can use shrink tube around each wire.

![cable](development/Gamecube/extensioncable.jpg)

Set up your breadboard like this with a logic level converter.

![Breadboard](Gamecube/Gamecube_bb.png)


### Software

Checkout the examples on how to access and initialize the controller.

Be aware that every reading turns off interrupts. A logic bit pulse is 4uS long.
```
4 uS * 8 * (1 + 3) = 128 microseconds to read the status.
4 uS * 8 * (3 + 8) = 352 microseconds to read the data.
```

#####bool begin(const uint8_t pin, Gamecube_Status_t &status)
Initializes the controller and updates the passed in status report.
The status report tells you the controller type and the rumble state.
Normally you don't have to do this, but for a Wavebird Controller it seems to be necessary.
Returns true if initialization was successful, otherwise false.

#####bool end(const uint8_t pin)
Sends a rumble off signal to the controller and discards any incoming data.
Mainly does the same as read, but discards the data.
Returns true if operation was successful, otherwise false.

#####bool read(const uint8_t pin, Gamecube_Data_t &report, const bool rumble = false)
Reads in the new controller state to the passed in report.
Rumble will be turned on/off by the passed in bool.
If you don't pass the rumble boot it, it will by default not rumble.
Returns true if reading was successful, otherwise false.
If it fails, it still might have updated some bytes of the report.

#####Gamecube_Data_t
```cpp
typedef union{
	// 8 bytes of datareport that we get from the controller
	uint8_t whole8[];
	uint16_t whole16[];
	uint32_t whole32[];

	struct{
		uint8_t buttons1;
		uint8_t buttons2;
	};

	struct {
		// first data byte (bitfields are sorted in LSB order)
		uint8_t a : 1;
		uint8_t b : 1;
		uint8_t x : 1;
		uint8_t y : 1;
		uint8_t start : 1;
		uint8_t low0 : 1;
		uint8_t low1 : 1;
		uint8_t low2 : 1;

		// second data byte
		uint8_t dleft : 1;
		uint8_t dright : 1;
		uint8_t ddown : 1;
		uint8_t dup : 1;
		uint8_t z : 1;
		uint8_t r : 1;
		uint8_t l : 1;
		uint8_t high0 : 1;

		// 3rd-8th data byte
		uint8_t xAxis;
		uint8_t yAxis;
		uint8_t cxAxis;
		uint8_t cyAxis;
		uint8_t left;
		uint8_t right;
	};
} Gamecube_Data_t;
```

#####Gamecube_Status_t
```cpp
typedef union{
	// 3 bytes of statusreport that we get from the controller
	uint8_t whole8[];
	uint16_t whole16[];
	struct {
		// device information
		uint16_t device;

		// controller status (only rumble is known)
		uint8_t status0 : 3;
		uint8_t rumble : 1;
		uint8_t status1 : 4;
	};
} Gamecube_Status_t;
```

###Development

Here I just refer to other documentations, its explained pretty good.
Logical pulses are 3uS+1uS (0) or 1uS+3uS (1) in 8bit packs + one stop bit.
The Arduino sends the controller a command and the controller responds to that.
This all happens on the same data line bidirectional.

I started with [Brownans version](https://github.com/brownan/Gamecube-N64-Controller)
and improved it over the time. Now the library is written in assembler
to ensure the most accurate timings and compiler independence.

While developing I used a [logic analyser](https://www.saleae.com/logic/) which is very important.
I also had to look at the assembly output of the previous C code.
You have to look at the [AVR Instruction Set](http://www.atmel.com/Images/doc0856.pdf)
to count the cycles and understand what each command does.

How to manually [get assembler output](http://rcarduino.blogspot.de/2012/09/how-to-view-arduino-assembly.html)
you may use this command. Or you can also use the provided .bat file.
You have to edit the Arduino IDE path in the .bat file and start the .bat as administrator.

```
 C:\Users\Nico\Documents\arduino-1.6.0\hardware\tools\avr\bin\avr-objdump -S  C:\Users\Nico\AppData\Local\Temp\build8182289464639461553.tmp\GamecubeController.cpp.elf > C:\Arduino.txt
```

###Known Bugs
Ports next to the input can crosstalk maybe. This should be 0.2uS or so, not really important.
But this is a general hardware bug of the AVR uCs. Just want to note that.

###Links
* [brownan's N64 to Gamecube Controller Adapter](https://github.com/brownan/Gamecube-N64-Controller)
* [Yet Another Gamecube Documentation](http://hitmen.c02.at/files/yagcd/yagcd/chap9.html)
* [Nintendo Gamecube Controller Pinout](http://www.int03.co.uk/crema/hardware/gamecube/gc-control.htm)
* [Salae Logic](https://www.saleae.com/logic)
* [How to get assembler output](http://rcarduino.blogspot.de/2012/09/how-to-view-arduino-assembly.html)
* [AVR Instruction Set](http://www.atmel.com/Images/doc0856.pdf)
* [HID Project](https://github.com/NicoHood/HID)


N64 Library
===========

Soon

Contact
=======

Contact me via my blog:
http://nicohood.de/


Version History
===============
```
1.1 Release (02.xx.2015)
* Changes in the Gamecube Code:
 * Hardcoded all get/send functions with assembler
 * Now with all IDE versions compatible
 * Improved pin management/ram usage
 * Multiple controllers possible
 * Improved end() functions
 * Improved example
 * Updated HID Project example to version 2.1

1.0 Release (01.07.2014)
* Added Gamecube Device support
```

Licence and Copyright
=====================
If you use this library for any cool project let me know!

```
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
```
