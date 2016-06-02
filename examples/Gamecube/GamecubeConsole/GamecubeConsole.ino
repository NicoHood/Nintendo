/*
  Copyright (c) 2014-2016 NicoHood
  See the readme for credit to other people.

  GamecubeConsole example
  Read controller and mirror data to the gamecube
*/

#include "Nintendo.h"

// Define a Gamecube Controller and a Console
CGamecubeController GamecubeController(7);
CGamecubeConsole GamecubeConsole(8);

// Pin definitions
#define pinLed LED_BUILTIN

void setup()
{
  // Set up debug led
  pinMode(pinLed, OUTPUT);
}


void loop()
{
  // Try to read the controller data
  if (GamecubeController.read())
  {
    // Mirror the controller data to the console
    if (!GamecubeConsole.write(GamecubeController))
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
