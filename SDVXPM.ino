// Joystick - Version: Latest
#include <Joystick.h>

#include <Encoder.h>
// Button Config
#include "config.h"

byte pinArray[7] = {START, BTA, BTB, BTC, BTD, FXL, FXR};
byte ledArray[7] = {STARTLED, BTALED, BTBLED, BTCLED, BTDLED, FXLLED, FXRLED};

// Encoders
Encoder knobLeft(1, 0);
Encoder knobRight(20, 21);

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   8, 0,                  // Button Count, Hat Switch Count
                   true, true, false,     // X and Y, but no Z Axis
                   false, false, false,   // No Rx, Ry, or Rz
                   false, false,          // No rudder or throttle
                   false, false, false);  // No accelerator, brake, or steering

void setup() {
  pinMode(FXL, INPUT_PULLUP);
  pinMode(FXR, INPUT_PULLUP);
  pinMode(BTA, INPUT_PULLUP);
  pinMode(BTB, INPUT_PULLUP);
  pinMode(BTC, INPUT_PULLUP);
  pinMode(BTD, INPUT_PULLUP);
  pinMode(START, INPUT_PULLUP);
  pinMode(STARTLED, OUTPUT);
  pinMode(BTALED, OUTPUT);
  pinMode(BTBLED, OUTPUT);
  pinMode(BTCLED, OUTPUT);
  pinMode(BTDLED, OUTPUT);
  pinMode(FXLLED, OUTPUT);
  pinMode(FXRLED, OUTPUT);
  Serial.begin(9600);
  Serial.println("Sound Voltex Cardboard Prototype 2 Output:");
  Joystick.begin();
  Joystick.setXAxisRange(0, 255);
  Joystick.setYAxisRange(0, 255);
}

long positionLeft  = -999;
long positionRight = -999;

// Last state of the buttons
int currentState[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int previousState[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void knobDriver() {

  long newLeft, newRight;
  newLeft = knobLeft.read();
  newRight = knobRight.read();
  if (newLeft != positionLeft || newRight != positionRight) {
    if (newLeft > 255) {
      knobLeft.write(0);
    }
    if (newRight > 255) {
      knobRight.write(0);
    }
    if (newLeft < 0) {
      knobLeft.write(255);
    }
    if (newRight < 0) {
      knobRight.write(255);
    }
    Serial.print("Left = ");
    Serial.print(newLeft);
    Serial.print(", Right = ");
    Serial.print(newRight);
    Serial.println();
    Joystick.setXAxis(newLeft);
    Joystick.setYAxis(newRight);
    positionLeft = newLeft;
    positionRight = newRight;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    knobLeft.write(0);
    knobRight.write(0);
  }
}

void ledDriver() {
  for (int count = 0; count < 7; count++) {
    int led = digitalRead(ledArray[count]);
    if (led != currentState[count]) {
      if (currentState[count] == 1) {
        digitalWrite(ledArray[count], HIGH);
      }
      else {
        digitalWrite(ledArray[count], LOW);
      }
    }
  }
}
void buttonDriver() {
  for (int count = 0; count < 7; count++) {
    //Serial.println("testing");
    currentState[count] = !digitalRead(pinArray[count]);
    if (currentState[0] == 1 && currentState[5] == 1 && currentState[6] == 1 && currentState[7] == 0) {
      Joystick.setButton(7, 1);
      Serial.println("Combination BACK has been activated.");
      currentState[7] = 1;
    } else if (currentState[0] == 0 && currentState[5] == 0 && currentState[6] == 0 && currentState[7] == 1) {
      Joystick.setButton(7, 0);
      Serial.println("Combination BACK has been deactivated.");
      currentState[7] = 0;
    }
    if (previousState[count] == 1 && currentState[count] == 0) {
      Joystick.setButton(count, 0);
      // buttonRelease(firstDataArray[count], SeccondDataArray[count]);
      previousState[count] = currentState[count];
    } else if (previousState[count] == 0 && currentState[count] == 1) {
      Joystick.setButton(count, 1);
      // buttonPressed(firstDataArray[count], SeccondDataArray[count]);
      previousState[count] = currentState[count];
    //delay(40);
    }
  }
}

void loop() {
  //Knob stuff
  knobDriver();
  // buttons
  buttonDriver();
  ledDriver();
  // Button LED's
}
