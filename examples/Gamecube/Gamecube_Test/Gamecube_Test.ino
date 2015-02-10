/*
 Copyright (c) 2014-2015 NicoHood
 See the readme for credit to other people.

 Gamecube_Test example
 Enter any key into the serial monitor to start reading.
 Press A on the controller to enable rumble.
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
  while (!Serial);
  Serial.begin(115200);

  // set up debug led
  pinMode(pinLed, OUTPUT);

  // wait for some Serial input
  Serial.println(F("Enter any key to start reading the controller."));
  Serial.println(F("Press A on the controller to enable rumble."));
  while (Serial.read() == -1);
  Serial.println(F("Starting controller emulation"));
  Serial.println();
}

void loop() {
  static bool rumble = false;
  static bool error = true;

  if (error) {
    // Initialize the gamecube controller by sending it a null byte.
    // This is unnecessary for a standard controller, but is required for the
    // Wavebird.
    if (initGamecubeController(pinGamecubeController1, GamecubeStatus))
      error = false;
    else
      error = true;
  }
  // controller is initialized successfull
  else {
    // try to read data from the controller, translate and send to USB device
    if (Gamecube.read(pinGamecubeController1, GamecubeData, rumble)) {
      print_gc_report(GamecubeData);
      // enable rumble if button A is pressed
      rumble = GamecubeData.a;
    }
    else {
      Serial.println(F("Could not connect to the controller."));
      Serial.println();
      error = true;
    }
  }

  // led indicates error state
  digitalWrite(pinLed, error);

  // retry every second, else delay a bit to not overload PC
  if (error)
    delay(1000);
  else
    delay(100);
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