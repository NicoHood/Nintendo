/*
 Copyright (c) 2014-2015 NicoHood
 See the readme for credit to other people.

 N64_Test example
 Enter any key into the serial monitor to start reading.
 Press A on the controller to enable rumble.
 */

#include "Nintendo.h"

// pin definitions
#define pinN64Controller1 3
#define pinLed LED_BUILTIN

// variables to save controller information
N64_Data_t N64Data;
N64_Status_t N64Status;

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
    // Initialize the N64 controller by sending it a null byte.
    // This is unnecessary for a standard controller, but some games do this
    if (initN64Controller(pinN64Controller1, N64Status))
      error = false;
    else
      error = true;
  }
  // controller is initialized successfull
  else {
    // try to read data from the controller, translate and send to USB device
    if (N64.read(pinN64Controller1, N64Data, rumble)) {
      print_n64_report(N64Data);
      // enable rumble if button A is pressed
      rumble = N64Data.a;
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

bool initN64Controller(uint8_t pin, N64_Status_t &n64_status) {
  // try to initialize controller
  if (N64.begin(pin, n64_status)) {
    // print the values for debug
    Serial.println(F("Controller initialization successfull."));

    // print device information
    Serial.print(F("Device: "));
    switch (n64_status.device) {
      case NINTENDO_DEVICE_N64_WIRED:
        Serial.println(F("Original N64 Controller"));
        break;
      default:
        Serial.print(F("Unknown "));
        Serial.println(n64_status.device, HEX);
        break;
    }

    // print rumble state
    Serial.print(F("Rumble "));
    if (n64_status.rumble)
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

void print_n64_report(N64_Data_t &n64_report) {
  // Prints the raw data from the controller
  Serial.println();
  Serial.println(F("Printing N64 controller report:"));

  Serial.print(F("A:\t"));
  Serial.println(n64_report.a);

  Serial.print(F("B:\t"));
  Serial.println(n64_report.b);

  Serial.print(F("Z:\t"));
  Serial.println(n64_report.z);

  Serial.print(F("Start:\t"));
  Serial.println(n64_report.start);

  Serial.print(F("Dup:\t"));
  Serial.println(n64_report.dup);
  Serial.print(F("Ddown:\t"));
  Serial.println(n64_report.ddown);
  Serial.print(F("Dleft:\t"));
  Serial.println(n64_report.dleft);
  Serial.print(F("Dright:\t"));
  Serial.println(n64_report.dright);

  Serial.print(F("L:\t"));
  Serial.println(n64_report.l);
  Serial.print(F("R:\t"));
  Serial.println(n64_report.r);

  Serial.print(F("Cup:\t"));
  Serial.println(n64_report.cup);
  Serial.print(F("Cdown:\t"));
  Serial.println(n64_report.cdown);
  Serial.print(F("Cleft:\t"));
  Serial.println(n64_report.cleft);
  Serial.print(F("Cright:\t"));
  Serial.println(n64_report.cright);

  Serial.print(F("xAxis:\t"));
  Serial.println(n64_report.xAxis, DEC);
  Serial.print(F("yAxis:\t"));
  Serial.println(n64_report.yAxis, DEC);

  Serial.println();
}
