/*
 Copyright (c) 2014-2015 NicoHood
 See the readme for credit to other people.

 Gamecube_Print example
 Enter any key into the serial monitor to print Gamcube controller status/report.
 It will turn on/off rumble every read.
 */

#include "Nintendo.h"

// pin definitions
#define pinGamecubeController1 2

// variables to save controller information
Gamecube_Data_t GamecubeData;
Gamecube_Status_t GamecubeStatus;

void setup() {
  // start debug serial
  Serial.begin(115200);
  Serial.println(F("Code has started!"));

  // Initialize the gamecube controller by sending it a null byte.
  // This is unnecessary for a standard controller, but is required for the
  // Wavebird.
  while (true) {
    // wait for some Serial input
    Serial.println(F("Enter any key into the serial monitor to initialize the controller."));
    Serial.println();
    waitForSerial();

    // stop if initialization is successful
    if (initGamecubeController(pinGamecubeController1, GamecubeStatus))
      break;
  }

  // initialization successful
  Serial.println(F("Starting controller emulation"));
  Serial.println();
}

void loop() {
  // wait for some Serial input
  Serial.println(F("Enter any key into the serial monitor to read the controller."));
  char c = toupper(waitForSerial());

  // if you enter 'E' to the Serial rumble will turn off
  // the received data will just be discarded
  if (c == 'E') {
    Serial.println(F("Turning off rumble."));
    Gamecube.end(pinGamecubeController1);
    return;
  }

  // if you enter 'R' to the Serial monitor it will rumble the motor
  bool rumble;
  if (c == 'R') {
    Serial.println(F("Turning rumble on."));
    rumble = true;
  }
  else
    rumble = false;

  // try to read data from the controller
  if (Gamecube.read(pinGamecubeController1, GamecubeData, rumble)) {
    print_gc_report(GamecubeData);
  }
  else {
    // controller connection lost/something went wrong
    Serial.println(F("Could not connect to the controller."));
    Serial.println();

    // try to initialize again
    Serial.println(F("Trying to initialize again."));
    if (initGamecubeController(pinGamecubeController1, GamecubeStatus)) {
      // initialization successful, try to read again, dont use rumble right now
      if (Gamecube.read(pinGamecubeController1, GamecubeData))
        print_gc_report(GamecubeData);
      else
        Serial.println(F("Could still not connect to the controller."));
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

char waitForSerial(void) {
  // wait for a single key, then discard any further input
  // return the first received key
  char c;
  while (true) {
    c = Serial.read();
    if (c != -1)
      break;
  }
  delay(100);
  while (Serial.read() != -1);
  return c;
}

void print_gc_report(Gamecube_Data_t &gc_report) {
  // Prints the raw data from the controller
  Serial.println();
  Serial.println(F("Printing Gamecube controller report:"));
  Serial.print(F("Start: "));
  Serial.println(gc_report.start);

  Serial.print(F("Y:     "));
  Serial.println(gc_report.y);

  Serial.print(F("X:     "));
  Serial.println(gc_report.x);

  Serial.print(F("B:     "));
  Serial.println(gc_report.b);

  Serial.print(F("A:     "));
  Serial.println(gc_report.a);

  Serial.print(F("L:     "));
  Serial.println(gc_report.l);
  Serial.print(F("R:     "));
  Serial.println(gc_report.r);
  Serial.print(F("Z:     "));
  Serial.println(gc_report.z);

  Serial.print(F("Dup:   "));
  Serial.println(gc_report.dup);
  Serial.print(F("Ddown: "));
  Serial.println(gc_report.ddown);
  Serial.print(F("Dright:"));
  Serial.println(gc_report.dright);
  Serial.print(F("Dleft: "));
  Serial.println(gc_report.dleft);

  Serial.print(F("Stick X:"));
  Serial.println(gc_report.xAxis, DEC);
  Serial.print(F("Stick Y:"));
  Serial.println(gc_report.yAxis, DEC);

  Serial.print(F("cStick X:"));
  Serial.println(gc_report.cxAxis, DEC);
  Serial.print(F("cStick Y:"));
  Serial.println(gc_report.cyAxis, DEC);

  Serial.print(F("L:     "));
  Serial.println(gc_report.left, DEC);
  Serial.print(F("R:     "));
  Serial.println(gc_report.right, DEC);
  Serial.println();
}
