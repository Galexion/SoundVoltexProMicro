#include <Encoder.h>
#include <Joystick.h>
#include <ezButton.h>
#include "PluggableUSB.h"
#include "HID.h"

// Button Config
#include "config.h"

// BTN & Pin layout

byte pinArray[7] = {START, BTA, BTB, BTC, BTD, FXL, FXR};
byte ledArray[7] = {STARTLED, BTALED, BTBLED, BTCLED, BTDLED, FXLLED, FXRLED};

typedef struct {
  uint8_t brightness;
} SingleLED;

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} RGBLed;

// ******************************
// EDIT THESE LINES

// The single LEDs will be first,
// The RGB LEDs will come afterwards, with R/G/B individually
#define NUMBER_OF_SINGLE 7
#define NUMBER_OF_RGB 0


 
// Encoders
Encoder knobLeft(1, 0);
Encoder knobRight(20, 21);
unsigned long startMillisBT;
unsigned long currentMillisBT;
const unsigned long timeframeBT = 15;
unsigned long startMillisHID;
unsigned long currentMillisHID;
const unsigned long timeframeHID = 100;

void light_update(SingleLED* single_leds, RGBLed* rgb_leds) {
  currentMillisHID = millis();
  for(int i = 0; i < NUMBER_OF_SINGLE; i++) {
    // YOUR CODE HERE
        digitalWrite(ledArray[i], single_leds[i].brightness);
  }
  for(int i = 0; i < NUMBER_OF_RGB; i++) {
    // YOUR CODE HERE
    // rgb_stuff(rgb_leds[i].r, rgb_leds[i].g, rgb_leds[i].b);
  }
    startMillisBT = currentMillisBT;
}



Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   8, 0,                  // Button Count, Hat Switch Count
                   true, true, false,     // X and Y, but no Z Axis
                   false, false, false,   // No Rx, Ry, or Rz
                   false, false,          // No rudder or throttle
                   false, false, false);  // No accelerator, brake, or steering

void setup() {
  // Start the timer for BT's & Lights
  startMillisBT = millis();
  startMillisHID = millis();
  // Set Up Pins
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

long positionLeft  = 0;
long positionRight = 0;

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
  // Record Millis
  currentMillisBT = millis();
  // If enough time has passed, do this
  if (currentMillisBT - startMillisBT >= timeframeBT) {
    Serial.println("Checking.");
    for (int count = 0; count < 7; count++) {
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
      previousState[count] = currentState[count];
    } else if (previousState[count] == 0 && currentState[count] == 1) {
      Joystick.setButton(count, 1);
      previousState[count] = currentState[count];
    }
    }
    // Set start timer equal to current timer
    startMillisBT = currentMillisBT;
  }
}
void loop() {
  currentMillisHID = millis();
  //Knob stuff
  knobDriver();
  // buttons
  buttonDriver();
  if (currentMillisHID - startMillisHID >= timeframeHID) {
    ledDriver();
    }
}

// ******************************
// don't need to edit below here

#define NUMBER_OF_LIGHTS (NUMBER_OF_SINGLE + NUMBER_OF_RGB*3)
#if NUMBER_OF_LIGHTS > 63
  #error You must have less than 64 lights
#endif

union {
  struct {
    SingleLED singles[NUMBER_OF_SINGLE];
    RGBLed rgb[NUMBER_OF_RGB];
  } leds;
  uint8_t raw[NUMBER_OF_LIGHTS];
} led_data;

static const uint8_t PROGMEM _hidReportLEDs[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x00,                    // USAGE (Undefined)
    0xa1, 0x01,                    // COLLECTION (Application)
    // Globals
    0x95, NUMBER_OF_LIGHTS,        //   REPORT_COUNT
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x05, 0x0a,                    //   USAGE_PAGE (Ordinals)
    // Locals
    0x19, 0x01,                    //   USAGE_MINIMUM (Instance 1)
    0x29, NUMBER_OF_LIGHTS,        //   USAGE_MAXIMUM (Instance n)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    // BTools needs at least 1 input to work properly
    0x19, 0x01,                    //   USAGE_MINIMUM (Instance 1)
    0x29, 0x01,                    //   USAGE_MAXIMUM (Instance 1)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0xc0                           // END_COLLECTION
};

// This is almost entirely copied from NicoHood's wonderful RawHID example
// Trimmed to the bare minimum
// https://github.com/NicoHood/HID/blob/master/src/SingleReport/RawHID.cpp
class HIDLED_ : public PluggableUSBModule {

  uint8_t epType[1];

  public:
    HIDLED_(void) : PluggableUSBModule(1, 1, epType) {
      epType[0] = EP_TYPE_INTERRUPT_IN;
      PluggableUSB().plug(this);
    }

    int getInterface(uint8_t* interfaceCount) {
      *interfaceCount += 1; // uses 1
      HIDDescriptor hidInterface = {
        D_INTERFACE(pluggedInterface, 1, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
        D_HIDREPORT(sizeof(_hidReportLEDs)),
        D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 16)
      };
      return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
    }

    int getDescriptor(USBSetup& setup)
    {
      // Check if this is a HID Class Descriptor request
      if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) { return 0; }
      if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE) { return 0; }

      // In a HID Class Descriptor wIndex contains the interface number
      if (setup.wIndex != pluggedInterface) { return 0; }

      return USB_SendControl(TRANSFER_PGM, _hidReportLEDs, sizeof(_hidReportLEDs));
    }

    bool setup(USBSetup& setup)
    {
      if (pluggedInterface != setup.wIndex) {
        return false;
      }

      uint8_t request = setup.bRequest;
      uint8_t requestType = setup.bmRequestType;

      if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE)
      {
        return true;
      }

      if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE) {
        if (request == HID_SET_REPORT) {
          if(setup.wValueH == HID_REPORT_TYPE_OUTPUT && setup.wLength == NUMBER_OF_LIGHTS){
            USB_RecvControl(led_data.raw, NUMBER_OF_LIGHTS);
            light_update(led_data.leds.singles, led_data.leds.rgb);
            return true;
          }
        }
      }

      return false;
    }
};

HIDLED_ HIDLeds;
