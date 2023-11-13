#include <Arduino.h>
#include <BleGamepad.h>

// buttonPinA = 12;
// buttonPinB = 21;
// buttonPinX = 26;
// buttonPinY = 27;
// buttonPinTurbo = 0;
// buttonPinR3 = 2;
// buttonPinL3 = 4;
// buttonPinOptions = 5;
// buttonPinMacro = 13;
// buttonPinL1 = 14;
// buttonPinUp = 15;
// buttonPinLeft = 16;
// buttonPinDown = 17;
// buttonPinRight = 18;
// LedPower = 19;
// buttonPinR1 = 22;
// buttonPinShare = 23;
// buttonPinR2 = 25;
// buttonPinHome = 32;
// buttonPinL2 = 33;
//const int buttonPinUp = 15;
//buttonPinLeft = 16;
//buttonPinDown = 17;
//buttonPinRight = 18;

BleGamepad bleGamepad("PXN-0082 Arcade Joystick", "Ferrazzi Hardware", 100);

#define numOfButtons 15
#define numOfHats 1 // Maximum 4 hat switches supported

byte previousButtonStates[numOfButtons];
byte currentButtonStates[numOfButtons];
byte buttonPins[numOfButtons] = {12, 21, 0, 26, 27, 13, 14, 22, 33, 25, 23, 5, 32, 4, 2};
byte physicalButtons[numOfButtons] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

byte previousHatStates[numOfHats * 4];
byte currentHatStates[numOfHats * 4];
byte hatPins[numOfHats * 4] = {15, 18, 17, 16}; // in order UP, RIGHT, DOWN, LEFT. 4 pins per hat switch (Eg. List 12 pins if there are 3 hat switches)

unsigned long previousMillis  = 0;
int stateLed  = LOW;
const long interval = 300;

void setup()
{
    //Serial.begin(115200);
    //Serial.println("Starting BLE work!");
    pinMode(19, OUTPUT);

    // Setup Buttons
    for (byte currentPinIndex = 0; currentPinIndex < numOfButtons; currentPinIndex++)
    {
        pinMode(buttonPins[currentPinIndex], INPUT_PULLUP);
        previousButtonStates[currentPinIndex] = HIGH;
        currentButtonStates[currentPinIndex] = HIGH;
    }

    // Setup Hat Switches
    for (byte currentPinIndex = 0; currentPinIndex < numOfHats * 4; currentPinIndex++)
    {
        pinMode(hatPins[currentPinIndex], INPUT_PULLUP);
        previousHatStates[currentPinIndex] = HIGH;
        currentHatStates[currentPinIndex] = HIGH;
    }

    BleGamepadConfiguration bleGamepadConfig;
    bleGamepadConfig.setAutoReport(false);
    bleGamepadConfig.setButtonCount(numOfButtons);
    bleGamepadConfig.setHatSwitchCount(numOfHats);
    bleGamepad.begin(&bleGamepadConfig);
}

void loop()
{
    powerLED();
    if (bleGamepad.isConnected())
    {
        buttonPress();
        joystickPress();
        delay(20);
    }
}

void powerLED(){
  if (bleGamepad.isConnected()){
    digitalWrite(19, HIGH);
  }else{
   unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (stateLed == LOW) {
      stateLed = HIGH;
    } else {
      stateLed = LOW;
    }
    digitalWrite(19, stateLed);
  }
  }
}

void buttonPress(){
        // Deal with buttons
        for (byte currentIndex = 0; currentIndex < numOfButtons; currentIndex++)
        {
            currentButtonStates[currentIndex] = digitalRead(buttonPins[currentIndex]);

            if (currentButtonStates[currentIndex] != previousButtonStates[currentIndex])
            {
                if (currentButtonStates[currentIndex] == LOW)
                {
                    bleGamepad.press(physicalButtons[currentIndex]);
                }
                else
                {
                    bleGamepad.release(physicalButtons[currentIndex]);
                }
            }
        }
}

void joystickPress(){
          // Update hat switch pin states
        for (byte currentHatPinsIndex = 0; currentHatPinsIndex < numOfHats * 4; currentHatPinsIndex++)
        {
            currentHatStates[currentHatPinsIndex] = digitalRead(hatPins[currentHatPinsIndex]);
        }

        // Update hats
        signed char hatValues[4] = {0, 0, 0, 0};

        for (byte currentHatIndex = 0; currentHatIndex < numOfHats; currentHatIndex++)
        {
            signed char hatValueToSend = 0;

            for (byte currentHatPin = 0; currentHatPin < 4; currentHatPin++)
            {
                // Check for direction
                if (currentHatStates[currentHatPin + currentHatIndex * 4] == LOW)
                {
                    hatValueToSend = currentHatPin * 2 + 1;

                    // Account for last diagonal
                    if (currentHatPin == 0)
                    {
                        if (currentHatStates[currentHatIndex * 4 + 3] == LOW)
                        {
                            hatValueToSend = 8;
                            break;
                        }
                    }

                    // Account for first 3 diagonals
                    if (currentHatPin < 3)
                    {
                        if (currentHatStates[currentHatPin + currentHatIndex * 4 + 1] == LOW)
                        {
                            hatValueToSend += 1;
                            break;
                        }
                    }
                }
            }

            hatValues[currentHatIndex] = hatValueToSend;
        }

        // Set hat values
        bleGamepad.setHats(hatValues[0], hatValues[1], hatValues[2], hatValues[3]);

        // Update previous states to current states and send report
        if (currentButtonStates != previousButtonStates || currentHatStates != previousHatStates)
        {
            for (byte currentIndex = 0; currentIndex < numOfButtons; currentIndex++)
            {
                previousButtonStates[currentIndex] = currentButtonStates[currentIndex];
            }

            for (byte currentIndex = 0; currentIndex < numOfHats * 4; currentIndex++)
            {
                previousHatStates[currentIndex] = currentHatStates[currentIndex];
            }

            bleGamepad.sendReport();
        }
}
