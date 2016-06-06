/*
  Copyright (c) 2014-2016 NicoHood
  See the readme for credit to other people.

  GamecubeMultiSingleplayer example
  Read 2 controllers and send merged data to the gamecube.
  This way you can play a single player game in semi coop.
*/

#include "Nintendo.h"

// Define a Gamecube Controller and a Console
CGamecubeController GamecubeController1(7);
CGamecubeConsole GamecubeConsole1(8);
CGamecubeController GamecubeController2(9);

// Pin definitions
#define pinLed LED_BUILTIN

void setup()
{
  // Set up debug led
  pinMode(pinLed, OUTPUT);

  // Start debug serial
  Serial.begin(115200);
  Serial.println(F("Start"));
}


void loop()
{
  for (uint8_t option = 0; option < 2; option++)
  {
    // Only read a single controller between each gamecube poll.
    // The gamecube polls every 1ms - 15ms.
    // In Mario football it polls 2 times fast (1ms between) and then again after 15ms.
    // It may also help to write 2 times to the gamecube instead of only a single time.
    switch (option) {
      case 0:
        {
          if (!GamecubeController1.read()) {
            Serial.println(F("Error reading Gamecube controller 1."));
            digitalWrite(pinLed, HIGH);
            delay(1000);
            return;
          }
        }
        break;
      case 1:
        {
          if (!GamecubeController2.read()) {
            Serial.println(F("Error reading Gamecube controller 2."));
            digitalWrite(pinLed, HIGH);
            delay(1000);
            return;
          }
        }
        break;
    }

    // Get the data of each controller
    auto r1 = GamecubeController1.getReport();
    auto r2 = GamecubeController2.getReport();

    // Merge both controller data into one
    Gamecube_Data_t d = defaultGamecubeData;
    d.report.buttons0 |= r1.buttons0 | r2.buttons0;
    d.report.buttons1 |= r1.buttons1 | r2.buttons1;
    for (uint8_t i = 0; i < 4; i++)
    {
      // Add axis value together
      int newAxis = r1.raw8[2 + i] + r2.raw8[2 + i] - 128;
      if (newAxis < 0) {
        newAxis = 0;
      }
      else if (newAxis > 0xFF) {
        newAxis = 255;
      }
      d.report.raw8[2 + i] = newAxis;
    }

    // Use the maximum of each left/right trigger
    if (r1.left > r2.left) {
      d.report.left = r1.left;
    }
    else {
      d.report.left = r2.left;
    }
    if (r1.right > r2.right) {
      d.report.right = r1.right;
    }
    else {
      d.report.right = r2.right;
    }

    // Mirror the controller data to the console
    if (!GamecubeConsole1.write(d))
    {
      Serial.println(F("Error writing Gamecube controller."));
      digitalWrite(pinLed, HIGH);
      delay(1000);
    }

    // Enable rumble
    if (d.status.rumble) {
      GamecubeController1.setRumble(true);
      GamecubeController2.setRumble(true);
    }
    else {
      GamecubeController1.setRumble(false);
      GamecubeController2.setRumble(false);
    }
  }

  digitalWrite(pinLed, LOW);
}
