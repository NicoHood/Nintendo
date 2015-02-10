/*
 Copyright (c) 2014-2015 NicoHood
 See the readme for credit to other people.

 Gamecube_USB_HID_Compact example
 Use your Gamecube controller with PC as generic HID interface.
 No serial debug output is displayed, the led indicated errors.
 This needs the HID Project.
 https://github.com/NicoHood/HID
 */

#include "Nintendo.h"

// pin definitions
#define pinGamecubeController1 2
#define pinLed LED_BUILTIN

// variables to save controller information
Gamecube_Data_t GamecubeData;

void setup() {
  // set up debug led
  pinMode(pinLed, OUTPUT);

  // sends a clean report to the host. This is important on any Arduino type.
  Gamepad.begin();
}

void loop() {
  static bool error = true;

  if (error) {
    // Initialize the gamecube controller by sending it a null byte.
    // This is unnecessary for a standard controller, but is required for the
    // Wavebird.
    if (Gamecube.begin(pinGamecubeController1))
      error = false;
    else
      error = true;
  }
  // controller is initialized successfull
  else {
    // try to read data from the controller, translate and send to USB device
    if (Gamecube.read(pinGamecubeController1, GamecubeData))
      sendGamecubeReport(GamecubeData);
    else
      error = true;
  }

  // led indicates error state
  digitalWrite(pinLed, error);

  // retry every second, else delay a bit to not overload PC
  if (error)
    delay(1000);
  else
    delay(10);
}

void sendGamecubeReport(Gamecube_Data_t &gc_report) {
  // The two control sticks
  Gamepad.xAxis(gc_report.xAxis << 8);
  Gamepad.yAxis(~gc_report.yAxis << 8); // y stick needs to be inverted
  Gamepad.rxAxis(gc_report.cxAxis << 8);
  Gamepad.ryAxis(gc_report.cyAxis << 8);

  // optional for L/R (PS3 controller uses this methode too)
  // you can also seperate the triggers
  // for Windows calibration comment out the buttons (because l/r triggers the wizard)
  Gamepad.zAxis(int8_t(gc_report.left - gc_report.right) / 2);
  //Gamepad.zAxis(gc_report.left);
  //Gamepad.rzAxis(gc_report.right);

  // D-Pad:
  switch (gc_report.dpad) {
    // linear
    case NINTENDO_GAMECUBE_DPAD_LEFT:
      Gamepad.dPad1(GAMEPAD_DPAD_LEFT);
      break;
    case NINTENDO_GAMECUBE_DPAD_RIGHT:
      Gamepad.dPad1(GAMEPAD_DPAD_RIGHT);
      break;
    case NINTENDO_GAMECUBE_DPAD_DOWN:
      Gamepad.dPad1(GAMEPAD_DPAD_DOWN);
      break;
    case NINTENDO_GAMECUBE_DPAD_UP:
      Gamepad.dPad1(GAMEPAD_DPAD_UP);
      break;
    // diagnoal
    case NINTENDO_GAMECUBE_DPAD_DOWN_LEFT:
      Gamepad.dPad1(GAMEPAD_DPAD_DOWN_LEFT);
      break;
    case NINTENDO_GAMECUBE_DPAD_DOWN_RIGHT:
      Gamepad.dPad1(GAMEPAD_DPAD_DOWN_RIGHT);
      break;
    case NINTENDO_GAMECUBE_DPAD_UP_LEFT:
      Gamepad.dPad1(GAMEPAD_DPAD_UP_LEFT);
      break;
    case NINTENDO_GAMECUBE_DPAD_UP_RIGHT:
      Gamepad.dPad1(GAMEPAD_DPAD_UP_RIGHT);
      break;
    // centered
    default:
      Gamepad.dPad1(GAMEPAD_DPAD_CENTERED);
      break;
  } // end switch

  // 8 Gamecube buttons
  Gamepad.buttons(0x00UL | (gc_report.buttons0 & 0x1F) | ((gc_report.buttons1 & 0x70) << 1));

  // You could add a zero value correction here or a minimum
  // for the left/right shoulder triggers but Windows can calibrate controllers very well.

  // Write the information to the PC
  Gamepad.write();
}

