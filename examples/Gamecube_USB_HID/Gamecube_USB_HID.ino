/*
 Copyright (c) 2014 NicoHood
 See the readme for credit to other people.

 Gamecube_USB_HID example
 Use your Gamecube controller with PC as generic HID interface.
 This need the HID library from the HID Project. Also see my Github for that
 */

#include <Nintendo.h>
#include <HID.h>

const int pinButton = 8;

void setup() {
  HID.begin();
  //Serial.begin(115200); //this is replaced with HID.begin and baud 115200
  Gamepad1.begin();

  Serial.println("Code has started!");

  // Button
  pinMode(pinButton, INPUT_PULLUP);

  // Initialize the gamecube controller by sending it a null byte.
  // This is unnecessary for a standard controller, but is required for the
  // Wavebird.

  bool init = false;
  while (!init) {
    Serial.println("Press the button to initialize the controller");
    Serial.println();
    while (digitalRead(pinButton));
    delay(300); // simple debounce

    if ((init = Gamecube.begin(A0))) {
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

void loop() {
  if (Gamecube.read())
    sendGamecubeReport();
  else {
    Serial.println("Couldnt connect to the controller");
    delay(1000);
  }
}

void sendGamecubeReport(void) {
  // The two control sticks
  Gamepad1.xAxis(Gamecube.report.xAxis << 8);
  Gamepad1.yAxis(~Gamecube.report.yAxis << 8); // y stick needs to be inverted
  Gamepad1.rxAxis(Gamecube.report.cxAxis << 8);
  Gamepad1.ryAxis(Gamecube.report.cyAxis << 8);

  // optional for L/R (PS3 controller uses this methode too)
  // you can also seperate the triggers
  // for Windows calibration comment out the buttons (because l/r triggers the wizard)
  Gamepad1.zAxis((0x7F + int(Gamecube.report.left - Gamecube.report.right) / 2)<<8);
  //Gamepad1.zAxis(Gamecube.report.left<<8);
  //Gamepad1.rzAxis(Gamecube.report.right<<8);

  // D-Pad:
  switch (Gamecube.report.buttons2 & 0x0F) {
      // linear
    case 0x01:
      Gamepad1.dPad1(0x07);
      break;
    case 0x02:
      Gamepad1.dPad1(0x03);
      break;
    case 0x04:
      Gamepad1.dPad1(0x05);
      break;
    case 0x08:
      Gamepad1.dPad1(0x01);
      break;
      // diagnoal
    case 0x05:
      Gamepad1.dPad1(0x06);
      break;
    case 0x06:
      Gamepad1.dPad1(0x04);
      break;
    case 0x09:
      Gamepad1.dPad1(0x08);
      break;
    case 0x0A:
      Gamepad1.dPad1(0x02);
      break;
    default:
      Gamepad1.dPad1(0x00);
      break;
  } // end switch

  // 8 Gamecube buttons
  Gamepad1.buttons(0x00 | (Gamecube.report.buttons1 & 0x1F) | ((Gamecube.report.buttons2 & 0x70) << 1));

  // You could add a zero value correction here or a minimum
  // for the left/right shoulder triggers but Windows can calibrate controllers very well.

  // Write the information to the PC
  Gamepad1.write();
}