/*
  Copyright (c) 2014-2016 NicoHood
  See the readme for credit to other people.

  GamecubeConsole example
  Read controller and mirror data to the gamecube
*/

#include "Nintendo.h"

// Define a Gamecube Controller and a Console
CGamecubeController GamecubeController1(7);
CGamecubeConsole GamecubeConsole1(8);

// Pin definitions
#define pinLed LED_BUILTIN

void setup()
{
  // Set up debug led
  pinMode(pinLed, OUTPUT);

  // Start debug serial
  Serial.begin(115200);
}


void loop()
{
  // Try to read the controller data
  if (GamecubeController1.read())
  {
    // Mirror the controller data to the console
    if (!GamecubeConsole1.write(GamecubeController1))
    {
      Serial.println(F("Error writing Gamecube controller."));
      digitalWrite(pinLed, HIGH);
      delay(1000);
    }
  }
  else
  {
    Serial.println(F("Error reading Gamecube controller."));
    digitalWrite(pinLed, HIGH);
    delay(1000);
  }
  digitalWrite(pinLed, LOW);
}
