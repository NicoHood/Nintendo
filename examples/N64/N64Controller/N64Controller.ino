/*
  Copyright (c) 2014-2016 NicoHood
  See the readme for credit to other people.

  N64Controller example
  Enter any key into the serial monitor to start reading.
*/

#include "Nintendo.h"

// Define a N64 Controller
CN64Controller N64Controller(7);

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
  if (N64Controller.read())
  {
    // Print Controller information
    auto status = N64Controller.getStatus();
    auto report = N64Controller.getReport();
    print_n64_report(report, status);
    delay(100);
  }
  else
  {
    // Add debounce if reading failed
    Serial.println(F("Error reading N64 controller."));
    digitalWrite(pinLed, HIGH);
    delay(1000);
  }
  digitalWrite(pinLed, LOW);
}

void print_n64_report(N64_Report_t &n64_report, N64_Status_t &n64_status)
{
  // Print device information
  Serial.print(F("Device: "));
  switch (n64_status.device) {
    case NINTENDO_DEVICE_N64_NONE:
      Serial.println(F("No N64 Controller found!"));
      break;
    case NINTENDO_DEVICE_N64_WIRED:
      Serial.println(F("Original Nintendo N64 Controller"));
      break;

    default:
      Serial.print(F("Unknown "));
      Serial.println(n64_status.device, HEX);
      break;
  }

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
