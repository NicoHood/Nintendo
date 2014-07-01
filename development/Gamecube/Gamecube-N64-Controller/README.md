==================================
N64 to Gamecube Controller Adapter
==================================

In 2009, I wanted to play some N64 games of mine, but all the
controllers had worn down control sticks and it made things frustrating. I
looked at cleaning or repairing them, but after a few failed attempts, it was
clear I needed to try something else. I started looking at making an adapter to
play the N64 using a Gamecube controller.

I found the existing `Cube64 Project`_ which did exactly that using a PIC
microcontroller. At the time, I had 0 experience with programming
microcontrollers, and programming a PIC required some programming circuitry
that I didn't quite understand. Perhaps it wasn't that complicated, but my
brother did happen to have an Arduino_ laying around, so I decided to take a
shot at writing the code for the Arduino.

This is that code. With only an Arduino and a 1K resistor, I successfully
enjoyed my N64 games using a Gamecube controller, complete with Rumble!

.. _Arduino: http://arduino.cc/en/Main/ArduinoBoardDuemilanove

Materials
=========
This is an extensive list of what's required. A lot I had laying around already, nothing was that expensive.

* A Gamecube controller

* An N64

* An Arduino. I used the Duemilanove with an AtMega328 running at 16MHz. A
  different chip may have different timings, and a different speed will most
  certainly require modifications to the timing code.

* Gamecube controller extension cables. I picked a couple of these off eBay so
  that I didn't have to splice my GC controller cables up. I spliced the
  extension cables to plug into the arduino, so I would have a nice socket to
  plug the GC controllers into.

* A 64 controller, to splice the plug from. A 64 extension cable would also
  work, but since my existing controllers were pretty much useless, I cut the
  cables to use to connect the Arduino to the N64 itself.

* Some wire and alligator clips or a breadboard or something to hook everything
  up with

* A single 1K ohm resistor, as a pull-up resistor. Higher resistance will
  probably work just fine. I'm not an electronics guy, but the 1K works fine
  for me.

Quick Setup
===========
To just get things up and running, here's what to do:

Hooking the N64 to the Arduino
------------------------------
The N64 controller cord had 3 wires: ground, +3.3V, and data. The pin-out is shown in Figure 1 (left).

1. +3.3V (red) - connect to nothing

2. Data (white) - connect to Arduino digital I/O 8

3. GND (black) - connect to Arduino ground

The wire colors may or may not vary depending on the model, I would check that
the wire colors match the pin-out before you connect them up.

.. figure:: https://github.com/brownan/Gamecube-N64-Controller/raw/master/connections.png
    :alt: Gamecube and N64 controller connections

    Figure 1: Pin numbers for an N64 plug (left) and a Gamecube socket (right).
    Credit to this diagram goes to the `Cube64 project`_.

Hooking up the Gamecube controller to the Arduino
-------------------------------------------------
The GC controller cord had 6 wires: +5V, +3.3V, data, 2x ground, and one unused wire. The pin-out is shown in Figure 1 (right).

1. +5V (green) - connect to Arduino +5V supply

2. Data (red) - connect to Arduino digital I/O 2

3. GND (yellow) - connect to Arduino ground

4. GND (brown) - connect to Arduino ground

5. N/C (black) - don't connect

6. +3.3V (orange) - connect to Arduino +3.3V supply

The internal wire colors may vary from the ones listed here. Remember, I
spliced some third party GC cables I got off eBay, so I wouldn't be surprised
if the wire colors were not the same as an official GC controller. Verify the
colors match the actual pin-out.

Also, as you can see, the Arduino provides power to the controller: 3.3V to the
circuits, and 5V for the rumble. The Arduino needs its own power supplied to
it, either via USB or an external power brick. The N64 doesn't provide enough
power for the Arduino.

Attaching the Pull-up Resistor
------------------------------
The protocol requires the data line to be idle-high, so **attach the 1K
resistor between digital I/O 2 and the 3.3V supply**. This will keep the line
at 3.3V unless the Arduino or the controller pulls it down to ground.

Compiling the Code
------------------
``gamecube.pde`` provided is the entire source. You can open it with the
Arduino IDE and compile it, that should work. I've also provided a Makefile
that I used when developing it (since I needed to analyze the pre-assembled
code). If you use the makefile, you may need to edit the top for the locations
of the Arduino libraries.

Running it
----------
Once the code is loaded on the Arduino, and everything is hooked up, it's ready
to use. When the Arduino first powers up, it waits for a signal from the N64.
**Therefore, the Arduino must be powered on and ready (wait a couple seconds)
before the N64 is turned on**. Then turn the N64 on and it should be good to go.

If you turn the N64 off to e.g. load a new game, you'll need to reset the
Arduino. Just hit the reset button when the N64 is off before you turn it back
on.

I've tested this with every game I have, and it works with both wired
controllers and the wavebird. I haven't tried it with any third party
controllers.

Configuration
=============
There are two things that people may want to customize. Both of them require
editing source code, I didn't make any easy or dynamic interface to them.

X and Y button mappings
-----------------------
Since the X and Y buttons don't exist on the N64, one has some freedom in
mapping these buttons. I like to map them to C-down and C-left respectively for
games like Starfox where those buttons are more significant. For something like
Perfect Dark where C-left and C-right strafe, I map X and Y to those instead.

You can go to around line 235 in ``gamecube.pde`` to configure the mapping. Try
uncommenting the mapping for X -> Cdown and comment out the line for X ->
Cright if you'd prefer that mapping.

Analog Stick Curve
------------------
On some games, such as Perfect Dark, the control stick feels a bit weird. That
is, it feels too sensitive, like there's not enough difference between fully
tilted and slightly tilted. (or maybe it was not sensitive enough, I forget)

To help with this, I apply a curve mapping inputs on the GC controller to
outputs on the N64 "controller" in a non-linear fashion.

.. figure:: https://github.com/brownan/Gamecube-N64-Controller/raw/master/curve.png
    :alt: Analog Stick curve graph

    Figure 2: A graph showing a linear mapping of inputs to outputs (red) and a
    cubic mapping from inputs to outputs (green). Inputs (from the GC
    controller) are along the X axis, while outputs (to the N64) are on the Y
    axis.

To turn this off, head to line 279 in ``gamecube.pde`` and change the 0 to a 1.
In my experience, this curve helps in some games, but hurts in others.

Method
======
Here's the technical info on how all this works

Hardware Setup
--------------
The gamecube connection has 6 wires: 2 ground, a 3.3V rail, a 5V rail for rumble, a data line, and an unused line. The data line goes into digital I/O 2. The rest go in their obvious places.

The N64 has 3 wires: 3.3V power supply, data, and ground. I don't use the power, the arduino needs to be powered externally anyways and provides its own 3.3V supply. The data plugs into digital I/O 8 and ground goes to ground.

Pull-up Resistor
----------------
The line to the controller is idle-high at 3.3V and is brought low to signal a bit. This means we can't use the Arduino's built-in pull-up resistors to signal, since they operate at 5V. The solution I found works is to bridge the Arduino's 3.3V supply and digital I/O pin 2 with a 1K ohm resistor. This keeps the line high at 3.3V when the pin is in input mode, and can be lowered by setting the pin to output a 0. Thus forming the signaling mechanism.

Signaling
---------
The protocol is simple, it uses low pulses of either 1μs or 3μs to indicate a 1 bit or 0 bit respectively. Bits come in every 4μs, so a 1 bit is 1μs low followed by 3μs high.

This microsecond timing is no problem for the AtMega328, but it does cut it kind of close. At 16MHz I get exactly 16 clock cycles per microsecond. Which is for the most part plenty, but one code path where the loops iterate on a byte boundary with a 1μs budget takes exactly 16 cycles.

Coding
------
I coded the entire signaling routine (sending and receiving) in C, and then analyzed the assembly output, calculated the number of cycles each branch took using the `AVR Instruction Set manual`_, and added in the necessary number of "nop" instructions. Then re-compiled and tested.

.. _AVR Instruction Set manual: http://www.atmel.com/dyn/resources/prod_documents/doc0856.pdf

After some trial and error, I was successfully sending and receiving commands from a gamecube controller. The N64 was easy after that, since they used the same encoding.

Resources
=========
* The `Cube64 Project`_
* `Gamecube Controller Protocol information`_
* `Nintendo 64 Controller Protocol information`_
* `N64/Gamecube to USB adapter Project`_ had some code that was useful as a reference
* `N64 to GameCube conversion project`_ (not sure why anyone would want to go in this direction)

.. _Cube64 Project: http://cia.vc/stats/project/navi-misc/cube64
.. _Gamecube Controller Protocol information: http://www.int03.co.uk/crema/hardware/gamecube/gc-control.htm
.. _Nintendo 64 Controller Protocol information: http://www.mixdown.ca/n64dev/
.. _N64/Gamecube to USB adapter Project: http://www.raphnet.net/electronique/gc_n64_usb/index_en.php
.. _N64 to GameCube conversion project: http://www.raphnet.net/electronique/x2wii/index_en.php
