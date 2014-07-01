/*
 Copyright (c) 2014 NicoHood
 See the readme for credit to other people.
 
 Gamecube_Print example
 Press a button to print Gamcube controller status/report to the Serial
 It will turn on/off rumble every read.
 */

#include <Nintendo.h>

const int pinButton = 8;

void setup(){
  Serial.begin(115200);
  Serial.println("Code has started!");

  // Button
  pinMode(pinButton, INPUT_PULLUP);

  // Initialize the gamecube controller by sending it a null byte.
  // This is unnecessary for a standard controller, but is required for the
  // Wavebird.

  bool init = false;
  while(!init){
    Serial.println("Press the button to initialize the controller");
    Serial.println();
    while(digitalRead(pinButton));
    delay(300); // simple debounce

    if((init = Gamecube.begin(A0))){
      Serial.println("Controller initialization successfull"); 
      // Print the values for debug
      Serial.print("Device: ");
      Serial.println(Gamecube.status.device, HEX);
      Serial.print("Rumble: ");
      Serial.println(Gamecube.status.rumble, HEX);
    }
    else
      Serial.println("Controller initialization failed"); 
  }

  Serial.println();
  Serial.println("Starting controller emulation");
}

void loop(){
  Serial.println();
  Serial.println("Press the button to get controller report");
  while(digitalRead(pinButton));
  delay(300); // simple debounce
  static bool rumble = false;
  // you can fully remove the rumble to just read the values without
  if(Gamecube.read(rumble)){
    print_gc_status();
	rumble^=1; // switch rumble
	}
  else
    Serial.println("Couldnt connect to the controller");
}

void print_gc_status(void){
  // Prints the raw data from the controller
  Serial.println();
  Serial.println("Printing Gamecube controller report:");
  Serial.print("Start: ");
  Serial.println(Gamecube.report.start);

  Serial.print("Y:     ");
  Serial.println(Gamecube.report.y);

  Serial.print("X:     ");
  Serial.println(Gamecube.report.x);

  Serial.print("B:     ");
  Serial.println(Gamecube.report.b);

  Serial.print("A:     ");
  Serial.println(Gamecube.report.a);

  Serial.print("L:     ");
  Serial.println(Gamecube.report.l);
  Serial.print("R:     ");
  Serial.println(Gamecube.report.r);
  Serial.print("Z:     ");
  Serial.println(Gamecube.report.z);

  Serial.print("Dup:   ");
  Serial.println(Gamecube.report.dup);
  Serial.print("Ddown: ");
  Serial.println(Gamecube.report.ddown);
  Serial.print("Dright:");
  Serial.println(Gamecube.report.dright);
  Serial.print("Dleft: ");
  Serial.println(Gamecube.report.dleft);

  Serial.print("Stick X:");
  Serial.println(Gamecube.report.xAxis, DEC);
  Serial.print("Stick Y:");
  Serial.println(Gamecube.report.yAxis, DEC);

  Serial.print("cStick X:");
  Serial.println(Gamecube.report.cxAxis, DEC);
  Serial.print("cStick Y:");
  Serial.println(Gamecube.report.cyAxis, DEC);

  Serial.print("L:     ");
  Serial.println(Gamecube.report.left, DEC);
  Serial.print("R:     ");
  Serial.println(Gamecube.report.right, DEC);
}
