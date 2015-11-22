Arduino Nintendo Library 1.2
============================

![header](header.jpg)

This library is made to connect Nintendo Controllers to your Arduino very easy.
Make sure you grab the right hardware, tear off some cables to use your controllers
on your PC for example. The requirement are written in each library readme part.

**IDE 1.6 compatible!**

#####Supported devices
Host mode means that you can hook up an controller to your Arduino and read its buttons.
Controller mode means that your Arduino can act as controller.

* Gamecube Host
* N64 Host

#####Planned features
* Gamecube Device(todo)
* N64 Device(todo, I don't have any)
* Wii-Mote (USB Host shield)
* Wii Nunchuk (I2C)
* Wii Classic Controller (I2C)
* Wii-Mote plus(USB Host shield)
* Wiiu Pro Controller (USB Host shield)
* SNES Controller (I don't have any)

#####Todo:
* N64 rumble -> example
* N64 -> USB example

Download
========

You have 3 versions you can download:
* The master includes all fixes to the current stable release. Download it by clicking the "Download Zip" button at the right.
Use the [online Wiki](https://github.com/NicoHood/Nintendo/wiki) to get the newest documentation.
* Download an offline version in [releases](https://github.com/NicoHood/Nintendo/releases).
It's a snapshot of the current stable release but might have missed some updates that the current master branch has included.
This also includes an offline version of the wiki. Offline versions will be available after some time when the official release is out.
You can also find older versions with the associated documentation.
* Select [branch 'dev'](https://github.com/NicoHood/Nintendo/tree/dev) to test the bleeding edge of this software. It might now work at all or has a lot of debugging stuff in it.
If the dev version gets near to a new release a note will be placed here, that you can test the new dev beta.
Currently there is no beta available.


Wiki
====

All documentation moved to the [wiki page](https://github.com/NicoHood/Nintendo/wiki).

An offline snapshot is available in [releases](https://github.com/NicoHood/Nintendo/releases).


Contact
=======

You can contact me on my wordpress blog in the contact section.

www.nicohood.de


Version History
===============
```
1.2 Release (xx.xx.2015)
* Added N64 Host support
* Separated consoles
* Added library.properties
* Added version identifier

1.1 Release (14.02.2015)
* Changes in the Gamecube Code:
 * Hardcoded all get/send functions with assembler
 * Now with all IDE versions compatible
 * Improved pin management/ram usage
 * Multiple controllers possible
 * Improved end() function
 * Added new begin() function overload
 * Improved example
 * Updated HID Project example to version 2.1
 * Updated structs
* Moved documentation to the Wiki

1.0 Release (01.07.2014)
* Added Gamecube Device support
```

License and Copyright
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
