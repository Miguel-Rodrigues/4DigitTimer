#include "includes.hpp"
#include <EEPROM.h>
#include "driver.hpp"

void initializeDriver() {
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);

    pinMode(BUTTONS_PIN, INPUT);
    pinMode(LED_ARRAY_PIN, OUTPUT);
    
    clearBuffer();
}

void writeRegister() {
    // take the latchPin low so;
    // the LEDs don't change while you're sending in bits:
    digitalWrite(LATCH_PIN, LOW);
    // shift out the bits:
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, buffer);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, buffer >> 8);
    //take the latch pin high so the LEDs will light up:
    digitalWrite(LATCH_PIN, HIGH);
}

void clearBuffer() {
    buffer = 0;
    writeRegister();
}

void writeNumber(const uint8_t digits[], const bool dotMap[]) {

    uint16_t mask = SEVEN_SEGMENT | DIGIT_SELECT;
    for (int i = 0; i < 3; i++) {
        buffer = (buffer & ~mask) | DIGIT_MAP[digits[i]] | (1 << (i + 4)) | (dotMap[i] ? 0 : WITHOUT_DOT);
        writeRegister();
    }
}

void checkButtonStates()
{
    bool currentState = false;
    bool isButtonDown = false;
    for (uint8_t i = 0; i <= 3; i++) {
        buffer = (buffer & ~BUTTONS_MASK) | keypad[i]->getAddress();
        writeRegister();
        isButtonDown = (bool) digitalRead(BUTTONS_PIN);
        currentState = keypad[i]->getState();
        keypad[i]->setState(isButtonDown);

        if(currentState != isButtonDown) {
            if (isButtonDown && keypad[i]->onDown != nullptr) {
                keypad[i]->onDown(keypad[i]);
            }
            else if (keypad[i]->onUp != nullptr){
                keypad[i]->onUp(keypad[i]);
            }
        }
        else if (isButtonDown && keypad[i]->onHold != nullptr) {
            keypad[i]->onHold(keypad[i]);
        }
    }
}