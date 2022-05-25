/*
Copyright (c) 2014-2021 NicoHood
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

#define NINTENDO_GC_KEY_NONE           0x00

#define NINTENDO_GC_KEY_HOME           0x06  // fn + up
#define NINTENDO_GC_KEY_END            0x07  // fn + right
#define NINTENDO_GC_KEY_PAGEUP         0x08  // fn + left
#define NINTENDO_GC_KEY_PAGEDOWN       0x09  // fn + down
#define NINTENDO_GC_KEY_SCROLLLOCK     0x0a  // fn + insert

                                   // PSO EP1&2 Keymap
                                   //  normal   shift
#define NINTENDO_GC_KEY_A              0x10  //    a        A
#define NINTENDO_GC_KEY_B              0x11  //    b        B
#define NINTENDO_GC_KEY_C              0x12  //    c        C
#define NINTENDO_GC_KEY_D              0x13  //    d        D
#define NINTENDO_GC_KEY_E              0x14  //    e        E
#define NINTENDO_GC_KEY_F              0x15  //    f        F
#define NINTENDO_GC_KEY_G              0x16  //    g        G
#define NINTENDO_GC_KEY_H              0x17  //    h        H
#define NINTENDO_GC_KEY_I              0x18  //    i        I
#define NINTENDO_GC_KEY_J              0x19  //    j        J
#define NINTENDO_GC_KEY_K              0x1a  //    k        K
#define NINTENDO_GC_KEY_L              0x1b  //    l        L
#define NINTENDO_GC_KEY_M              0x1c  //    m        M
#define NINTENDO_GC_KEY_N              0x1d  //    n        N
#define NINTENDO_GC_KEY_O              0x1e  //    o        O
#define NINTENDO_GC_KEY_P              0x1f  //    p        P
#define NINTENDO_GC_KEY_Q              0x20  //    q        Q
#define NINTENDO_GC_KEY_R              0x21  //    r        R
#define NINTENDO_GC_KEY_S              0x22  //    s        S
#define NINTENDO_GC_KEY_T              0x23  //    t        T
#define NINTENDO_GC_KEY_U              0x24  //    u        U
#define NINTENDO_GC_KEY_V              0x25  //    v        V
#define NINTENDO_GC_KEY_W              0x26  //    w        W
#define NINTENDO_GC_KEY_X              0x27  //    x        X
#define NINTENDO_GC_KEY_Y              0x28  //    y        Y
#define NINTENDO_GC_KEY_Z              0x29  //    z        Z
#define NINTENDO_GC_KEY_1              0x2a  //    1        !
#define NINTENDO_GC_KEY_2              0x2b  //    2        "
#define NINTENDO_GC_KEY_3              0x2c  //    3        #
#define NINTENDO_GC_KEY_4              0x2d  //    4        $
#define NINTENDO_GC_KEY_5              0x2e  //    5        %
#define NINTENDO_GC_KEY_6              0x2f  //    6        &
#define NINTENDO_GC_KEY_7              0x30  //    7        '
#define NINTENDO_GC_KEY_8              0x31  //    8        (
#define NINTENDO_GC_KEY_9              0x32  //    9        )
#define NINTENDO_GC_KEY_0              0x33  //    0        ~
#define NINTENDO_GC_KEY_MINUS          0x34  //    -        =
#define NINTENDO_GC_KEY_CARET          0x35  //    ^        ~
#define NINTENDO_GC_KEY_YEN            0x36  //    \        |
#define NINTENDO_GC_KEY_AT             0x37  //    @        `
#define NINTENDO_GC_KEY_LEFTBRACKET    0x38  //    [        {
#define NINTENDO_GC_KEY_SEMICOLON      0x39  //    ;        +
#define NINTENDO_GC_KEY_COLON          0x3a  //    :        *
#define NINTENDO_GC_KEY_RIGHTBRACKET   0x3b  //    ]        }
#define NINTENDO_GC_KEY_COMMA          0x3c  //    ,        <
#define NINTENDO_GC_KEY_PERIOD         0x3d  //    .        >
#define NINTENDO_GC_KEY_SLASH          0x3e  //    /        ?
#define NINTENDO_GC_KEY_BACKSLASH      0x3f  //    \        _
#define NINTENDO_GC_KEY_F1             0x40
#define NINTENDO_GC_KEY_F2             0x41
#define NINTENDO_GC_KEY_F3             0x42
#define NINTENDO_GC_KEY_F4             0x43
#define NINTENDO_GC_KEY_F5             0x44
#define NINTENDO_GC_KEY_F6             0x45
#define NINTENDO_GC_KEY_F7             0x46
#define NINTENDO_GC_KEY_F8             0x47
#define NINTENDO_GC_KEY_F9             0x48
#define NINTENDO_GC_KEY_F10            0x49
#define NINTENDO_GC_KEY_F11            0x4a
#define NINTENDO_GC_KEY_F12            0x4b
#define NINTENDO_GC_KEY_ESC            0x4c
#define NINTENDO_GC_KEY_INSERT         0x4d
#define NINTENDO_GC_KEY_DELETE         0x4e
#define NINTENDO_GC_KEY_GRAVE          0x4f
#define NINTENDO_GC_KEY_BACKSPACE      0x50
#define NINTENDO_GC_KEY_TAB            0x51
#define NINTENDO_GC_KEY_CAPSLOCK       0x53
#define NINTENDO_GC_KEY_LEFTSHIFT      0x54
#define NINTENDO_GC_KEY_RIGHTSHIFT     0x55
#define NINTENDO_GC_KEY_LEFTCTRL       0x56
#define NINTENDO_GC_KEY_LEFTALT        0x57
//#define NINTENDO_GC_KEY_LEFTUNK1       0x58
#define NINTENDO_GC_KEY_SPACE          0x59
//#define NINTENDO_GC_KEY_RIGHTUNK1      0x5a
//#define NINTENDO_GC_KEY_RIGHTUNK2      0x5b
#define NINTENDO_GC_KEY_LEFT           0x5c
#define NINTENDO_GC_KEY_DOWN           0x5d
#define NINTENDO_GC_KEY_UP             0x5e
#define NINTENDO_GC_KEY_RIGHT          0x5f
#define NINTENDO_GC_KEY_ENTER          0x61

#define NINTENDO_GC_KEY_MAX            0x61