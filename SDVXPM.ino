#include <ezButton.h>
#include <Joystick.h>
#include <Encoder.h>
// Button Config
#include "config.h"

byte ledArray[7] = {STARTLED, BTALED, BTBLED, BTCLED, BTDLED, FXLLED, FXRLED};
long BTBACK = 0;

ezButton buttonArray[] = {
  ezButton(START),
  ezButton(BTA),
  ezButton(BTB),
  ezButton(BTC),
  ezButton(BTD),
  ezButton(FXL),
  ezButton(FXR),
  ezButton(BTBACK)
};
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
  for (byte i = 0; i < 6; i++) {
    buttonArray[i].setDebounceTime(13); // set debounce time to 50 milliseconds
  }

}

long positionLeft  = -999;
long positionRight = -999;

// Last state of the buttons

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
int currentState[8] = {0, 0, 0, 0, 0, 0, 0, 0};
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
    if (buttonArray[0].isPressed() && buttonArray[5].isPressed() && buttonArray[6].isPressed() && BTBACK == 0) {
      Joystick.setButton(7, 1);
      Serial.println("Combination BACK has been activated.");
      BTBACK = 1;
    } else if (buttonArray[0].isReleased() && buttonArray[5].isReleased() && buttonArray[6].isReleased() && BTBACK == 0) {
      Joystick.setButton(7, 0);
      Serial.println("Combination BACK has been deactivated.");
      BTBACK = 0;
    }
    if (buttonArray[count].isReleased()) {
      Joystick.setButton(count, 0);
      currentState[count] = 0;
    } else if (buttonArray[count].isPressed()) {
      Joystick.setButton(count, 1);
       currentState[count] = 1;
    //delay(40);
    }
  }
}

void loop() {
  for (byte i = 0; i < 7; i++)
    buttonArray[i].loop(); // MUST call the loop() function first
  //Knob stuff
  knobDriver();
  // buttons
  buttonDriver();
  ledDriver();
  // Button LED's
}
