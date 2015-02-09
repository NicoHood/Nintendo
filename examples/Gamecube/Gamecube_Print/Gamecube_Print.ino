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
  Serial.println("Code has started!");

  // Initialize the gamecube controller by sending it a null byte.
  // This is unnecessary for a standard controller, but is required for the
  // Wavebird.
  while (true) {
    // wait for some Serial input
    Serial.println("Enter any key into the serial monitor to initialize the controller.");
    Serial.println();
    waitForSerial();

    // stop if initialization is successful
    if (initGamecubeController(pinGamecubeController1, GamecubeStatus))
      break;
  }

  // initialization successful
  Serial.println("Starting controller emulation");
  Serial.println();
}

void loop() {
  // wait for some Serial input
  Serial.println("Enter any key into the serial monitor to read the controller.");
  char c = toupper(waitForSerial());

  // if you enter 'E' to the Serial rumble will turn off
  // the received data will just be discarded
  if (c == 'E') {
    Serial.println("Turning off rumble.");
    Gamecube.end(pinGamecubeController1);
    return;
  }

  // if you enter 'R' to the Serial monitor it will rumble the motor
  bool rumble;
  if (c == 'R') {
    Serial.println("Turning rumble on.");
    rumble = true;
  }
  else
    rumble = false;

  // try to read data from the controller
  if (Gamecube.read(pinGamecubeController1, GamecubeData, rumble)) {
    print_gc_status(GamecubeData);
  }
  else {
    // controller connection lost/something went wrong
    Serial.println("Could not connect to the controller.");
    Serial.println();

    // try to initialize again
    Serial.println("Trying to initialize again.");
    if (initGamecubeController(pinGamecubeController1, GamecubeStatus)) {
      // initialization successful, try to read again, dont use rumble right now
      if (Gamecube.read(pinGamecubeController1, GamecubeData))
        print_gc_status(GamecubeData);
      else
        Serial.println("Could still not connect to the controller.");
    }
  }
}

bool initGamecubeController(uint8_t pin, Gamecube_Status_t &status) {
  // try to initialize controller
  if (Gamecube.begin(pin, status)) {
    // print the values for debug
    Serial.println("Controller initialization successfull.");

    // print device information
    Serial.print("Device: ");
    switch (status.device) {
      case NINTENDO_DEVICE_GC_WIRED:
        Serial.println("Original Nintendo Gamecube Controller");
        break;
      default:
        Serial.print("Unknown ");
        Serial.println(status.device, HEX);
        break;
    }

    // print rumble state
    Serial.print("Rumble ");
    if (status.rumble)
      Serial.println("on");
    else
      Serial.println("off");

    Serial.println();
    return true;
  }
  else {
    Serial.println("Controller initialization failed, try again.");
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

void print_gc_status(Gamecube_Data_t &report) {
  // Prints the raw data from the controller
  Serial.println();
  Serial.println("Printing Gamecube controller report:");
  Serial.print("Start: ");
  Serial.println(report.start);

  Serial.print("Y:     ");
  Serial.println(report.y);

  Serial.print("X:     ");
  Serial.println(report.x);

  Serial.print("B:     ");
  Serial.println(report.b);

  Serial.print("A:     ");
  Serial.println(report.a);

  Serial.print("L:     ");
  Serial.println(report.l);
  Serial.print("R:     ");
  Serial.println(report.r);
  Serial.print("Z:     ");
  Serial.println(report.z);

  Serial.print("Dup:   ");
  Serial.println(report.dup);
  Serial.print("Ddown: ");
  Serial.println(report.ddown);
  Serial.print("Dright:");
  Serial.println(report.dright);
  Serial.print("Dleft: ");
  Serial.println(report.dleft);

  Serial.print("Stick X:");
  Serial.println(report.xAxis, DEC);
  Serial.print("Stick Y:");
  Serial.println(report.yAxis, DEC);

  Serial.print("cStick X:");
  Serial.println(report.cxAxis, DEC);
  Serial.print("cStick Y:");
  Serial.println(report.cyAxis, DEC);

  Serial.print("L:     ");
  Serial.println(report.left, DEC);
  Serial.print("R:     ");
  Serial.println(report.right, DEC);
  Serial.println();
}
