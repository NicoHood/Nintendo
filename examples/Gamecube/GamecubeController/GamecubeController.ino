/*
  Copyright (c) 2014-2016 NicoHood
  See the readme for credit to other people.

  GamecubeController example
  Enter any key into the serial monitor to start reading.
  Press A on the controller to enable rumble.
*/

#include "Nintendo.h"

// Define a Gamecube Controller
CGamecubeController GamecubeController(7);

// Pin definitions
#define pinLed LED_BUILTIN

void setup()
{
  // Set up debug led
  pinMode(pinLed, OUTPUT);

  // Start debug serial
  while (!Serial);
  Serial.begin(115200);

  // Wait for some Serial input
  Serial.println(F("Enter any key to start reading the controller."));
  Serial.println(F("Press A on the controller to enable rumble."));
  while (Serial.read() == -1);

  Serial.println(F("Starting controller emulation"));
  Serial.println();
}

void loop()
{
  // Try to read the controller data
  if (GamecubeController.read())
  {
    // Print Controller information
    auto status = GamecubeController.getStatus();
    auto report = GamecubeController.getReport();
    print_gc_report(report, status);
    delay(100);

    // Rumble if button "A" was pressed
    if (report.a) {
      GamecubeController.setRumble(true);
    }
    else {
      GamecubeController.setRumble(false);
    }
  }
  else
  {
    // Add debounce if reading failed
    Serial.println(F("Error reading Gamecube controller."));
    digitalWrite(pinLed, HIGH);
    delay(1000);
  }
  digitalWrite(pinLed, LOW);
}

void print_gc_report(Gamecube_Report_t &gc_report, Gamecube_Status_t &gc_status)
{
  // Print device information
  Serial.print(F("Device: "));
  switch (gc_status.device) {
    case NINTENDO_DEVICE_GC_NONE:
      Serial.println(F("No Gamecube Controller found!"));
      break;
    case NINTENDO_DEVICE_GC_WIRED:
      Serial.println(F("Original Nintendo Gamecube Controller"));
      break;

    default:
      Serial.print(F("Unknown "));
      Serial.println(gc_status.device, HEX);
      break;
  }

  // Print rumble state
  Serial.print(F("Rumble "));
  if (gc_status.rumble)
    Serial.println(F("on"));
  else
    Serial.println(F("off"));

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
