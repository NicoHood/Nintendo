/*
 Copyright (c) 2014-2015 NicoHood
 See the readme for credit to other people.

 Gamecube_USB_HID example
 Use your Gamecube controller with PC as generic HID interface.
 (Optional) Serial debug output is displayed
 (Optional) Press A on the controller to enable rumble.
 The led indicated errors.

 This needs the HID Project.
 https://github.com/NicoHood/HID
*/

#include "Nintendo.h"

// do you want to use Serial debug output?
#define DEBUG
// rumble if button A is pressed?
//#define RUMBLE

// pin definitions
#define pinGamecubeController1 2
#define pinLed LED_BUILTIN

// variables to save controller information
Gamecube_Data_t GamecubeData;
#ifdef DEBUG
Gamecube_Status_t GamecubeStatus;
#endif

void setup() {
#ifdef DEBUG
  // start debug serial
  Serial.begin(115200);
  Serial.println(F("Starting controller emulation"));
  Serial.println();
#endif

  // set up debug led
  pinMode(pinLed, OUTPUT);

  // sends a clean report to the host. This is important on any Arduino type.
  Gamepad.begin();
}

void loop() {
#ifdef RUMBLE
  static bool rumble = false;
#endif
  static bool error = true;

  if (error) {
    // Initialize the gamecube controller by sending it a null byte.
    // This is unnecessary for a standard controller, but is required for the
    // Wavebird.
#ifdef DEBUG
    if (initGamecubeController(pinGamecubeController1, GamecubeStatus))
#else
    if (Gamecube.begin(pinGamecubeController1))
#endif
      error = false;
    else
      error = true;
  }
  // controller is initialized successfull
  else {
    // try to read data from the controller, translate and send to USB device
#ifdef RUMBLE
    if (Gamecube.read(pinGamecubeController1, GamecubeData, rumble)) {
      sendGamecubeReport(GamecubeData);
      // enable rumble if button A is pressed
      rumble = GamecubeData.a;
    }
#else
    if (Gamecube.read(pinGamecubeController1, GamecubeData))
      sendGamecubeReport(GamecubeData);
#endif
    else {
#ifdef DEBUG
      Serial.println(F("Could not connect to the controller."));
      Serial.println();
#endif
      error = true;
    }
  }

  // led indicates error state
  digitalWrite(pinLed, error);

  // retry every second, else delay a bit to not overload PC
  if (error) {
    // reset USB controller report
    Gamepad.end();
    delay(1000);
  }
  else
    delay(10);
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
    Serial.println(F("Controller initialization failed."));
    Serial.println();
    return false;
  }
}

void sendGamecubeReport(Gamecube_Data_t &gc_report) {
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
