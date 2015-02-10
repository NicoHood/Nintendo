/*
 Copyright (c) 2014-2015 NicoHood
 See the readme for credit to other people.

 Gamecube_USB_HID example
 Use your Gamecube controller with PC as generic HID interface.
 This needs the HID Project.
 https://github.com/NicoHood/HID
 */

#include "Nintendo.h"

// pin definitions
#define pinGamecubeController1 2
#define pinLed LED_BUILTIN

// variables to save controller information
Gamecube_Data_t GamecubeData;
Gamecube_Status_t GamecubeStatus;

void setup() {
  // start debug serial
  Serial.begin(115200);
  Serial.println(F("Starting controller emulation"));

  // set up debug led
  pinMode(pinLed, OUTPUT);

  // sends a clean report to the host. This is important on any Arduino type.
  Gamepad.begin();
}

void loop() {
  static bool gcInitialized = false;

  if (!gcInitialized) {
    // Initialize the gamecube controller by sending it a null byte.
    // This is unnecessary for a standard controller, but is required for the
    // Wavebird.
    if (initGamecubeController(pinGamecubeController1, GamecubeStatus)) {
      gcInitialized = true;
      digitalWrite(pinLed, LOW);
    }

    // retry every second
    else {
      digitalWrite(pinLed, HIGH);
      delay(1000);
    }
  }
  // controller is initialized successfull
  else {
    // try to read data from the controller
    if (Gamecube.read(pinGamecubeController1, GamecubeData)) {
      // translate data and send to USB device
      sendGamecubeReport(GamecubeData);
      //print_gc_report(GamecubeData);
      delay(100);
    }
    else {
      Serial.println(F("Could still not connect to the controller."));
      Serial.println();
      gcInitialized = false;
      digitalWrite(pinLed, HIGH);
      delay(1000);
    }
  }
}

bool initGamecubeController(uint8_t pin, Gamecube_Status_t &gc_status) {
  // try to initialize controller
  if (Gamecube.begin(pin, gc_status)) {
    // print the values for debug
    Serial.println(F("Controller initialization successfull."));

    // print device information
    Serial.print(F("Device: "));
    switch (gc_status.device) {
      case NINTENDO_DEVICE_GC_WIRED:
        Serial.println(F("Original Nintendo Gamecube Controller"));
        break;
      default:
        Serial.print(F("Unknown "));
        Serial.println(gc_status.device, HEX);
        break;
    }

    // print rumble state
    Serial.print(F("Rumble "));
    if (gc_status.rumble)
      Serial.println(F("on"));
    else
      Serial.println(F("off"));

    Serial.println();
    return true;
  }
  else {
    Serial.println(F("Controller initialization failed, try again."));
    Serial.println();
    return false;
  }
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

void print_gc_report(Gamecube_Data_t &gc_report) {
  // Prints the raw data from the controller
  Serial.println();
  Serial.println(F("Printing Gamecube controller report:"));
  Serial.print(F("Start:\t"));
  Serial.println(gc_report.start);

  Serial.print(F("Y:\t"));
  Serial.println(gc_report.y);

  Serial.print(F("X:\t"));
  Serial.println(gc_report.x);

  Serial.print(F("B:\t"));
  Serial.println(gc_report.b);

  Serial.print(F("A:\t"));
  Serial.println(gc_report.a);

  Serial.print(F("L:\t"));
  Serial.println(gc_report.l);
  Serial.print(F("R:\t"));
  Serial.println(gc_report.r);
  Serial.print(F("Z:\t"));
  Serial.println(gc_report.z);

  Serial.print(F("Dup:\t"));
  Serial.println(gc_report.dup);
  Serial.print(F("Ddown:\t"));
  Serial.println(gc_report.ddown);
  Serial.print(F("Dright:\t"));
  Serial.println(gc_report.dright);
  Serial.print(F("Dleft:\t"));
  Serial.println(gc_report.dleft);

  Serial.print(F("xAxis:\t"));
  Serial.println(gc_report.xAxis, DEC);
  Serial.print(F("yAxis:\t"));
  Serial.println(gc_report.yAxis, DEC);

  Serial.print(F("cxAxis:\t"));
  Serial.println(gc_report.cxAxis, DEC);
  Serial.print(F("cyAxis:\t"));
  Serial.println(gc_report.cyAxis, DEC);

  Serial.print(F("L:\t"));
  Serial.println(gc_report.left, DEC);
  Serial.print(F("R:\t"));
  Serial.println(gc_report.right, DEC);
  Serial.println();
}