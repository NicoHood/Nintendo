/*
  Copyright (c) 2014-2016 NicoHood
  See the readme for credit to other people.

  Gamecube_USB_HID example
  Use your Gamecube controller with PC as generic HID interface.
  The led indicated errors.

  This needs the HID Project.
  https://github.com/NicoHood/HID
*/

#include "HID-Project.h"
#include "Nintendo.h"

// Define a Gamecube Controller
CGamecubeController GamecubeController1(7);

// Pin definitions
#define pinLed LED_BUILTIN

void setup()
{
  // Set up debug led
  pinMode(pinLed, OUTPUT);

  // Sends a clean report to the host. This is important on any Arduino type.
  Gamepad.begin();
}


void loop()
{
  // Try to read the controller data
  if (GamecubeController1.read())
  {
    // Send controller data to the USB interface
    auto report = GamecubeController1.getReport();
    sendGamecubeReport(report);

    // delay a bit to not overload PC
    delay(10);
  }
  else
  {
    // Center gamepad on disconnect
    Gamepad.end();

    // Add debounce if reading failed
    digitalWrite(pinLed, HIGH);
    delay(1000);
  }
  digitalWrite(pinLed, LOW);
}


void sendGamecubeReport(Gamecube_Report_t &gc_report)
{
  // The two control sticks
  Gamepad.xAxis((gc_report.xAxis - 0x80) << 8);
  Gamepad.yAxis(~(gc_report.yAxis - 0x80) << 8); // y stick needs to be inverted
  Gamepad.rxAxis((gc_report.cxAxis - 0x80) << 8);
  Gamepad.ryAxis((gc_report.cyAxis - 0x80) << 8);

  // you can merge L/R (PS3 controller uses this methode too)
  // or you can also seperate the triggers
  // for Windows calibration comment out the l/r buttons below
  // (because l/r triggers the wizard)
  //Gamepad.zAxis(abs(gc_report.left - gc_report.right) - 0x80);
  Gamepad.zAxis(gc_report.left - 0x80);
  Gamepad.rzAxis(gc_report.right - 0x80);

  // D-Pad:
  if (gc_report.dup && gc_report.dright) {
    Gamepad.dPad1(GAMEPAD_DPAD_UP_RIGHT);
  }
  else if (gc_report.dup && gc_report.dleft) {
    Gamepad.dPad1(GAMEPAD_DPAD_UP_LEFT);
  }
  else if (gc_report.ddown && gc_report.dright) {
    Gamepad.dPad1(GAMEPAD_DPAD_DOWN_RIGHT);
  }
  else if (gc_report.ddown && gc_report.dleft) {
    Gamepad.dPad1(GAMEPAD_DPAD_DOWN_LEFT);
  }
  else if (gc_report.dup) {
    Gamepad.dPad1(GAMEPAD_DPAD_UP);
  }
  else if (gc_report.ddown) {
    Gamepad.dPad1(GAMEPAD_DPAD_DOWN);
  }
  else if (gc_report.dright) {
    Gamepad.dPad1(GAMEPAD_DPAD_RIGHT);
  }
  else if (gc_report.dleft) {
    Gamepad.dPad1(GAMEPAD_DPAD_LEFT);
  }

  // 8 Gamecube buttons
  Gamepad.buttons(0x00UL | (gc_report.buttons0 & 0x1F) | ((gc_report.buttons1 & 0x70) << 1));

  // You could add a zero value correction here or a minimum
  // for the left/right shoulder triggers but the os can
  // (and should) calibrate controllers very well.

  // Write the information to the PC
  Gamepad.write();
}
