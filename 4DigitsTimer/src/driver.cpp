#include "includes.hpp"
#include <EEPROM.h>
#include "driver.hpp"

const int _BUTTONS_PIN = 2;
const int _LED_ARRAY_PIN = 3;

//Pin connected to ST_CP of 74HC595
const int _LATCH_PIN = 8;
//Pin connected to SH_CP of 74HC595
const int _CLOCK_PIN = 9;
//Pin connected to DS of 74HC595
const int _DATA_PIN = 10;

//Register address masks
const uint16_t _BUTTONS_MASK = 0x000F;
const uint16_t _DIGIT_SELECT = 0x00F0;
const uint16_t _SEVEN_SEGMENT = 0xFF00;

const uint16_t _DIGIT_MAP[14] = {
    0x0200, //0
    0x9E00, //1
    0x2400, //2
    0x0C00, //3
    0x9800, //4
    0x4800, //5
    0x4000, //6
    0x1E00, //7
    0x0000, //8
    0x0800, //9
    0x8500, //d
    0xC500, //o
    0xD500, //n
    0x6000, //E
};

const uint16_t _WITHOUT_DOT = 0x0100; //.

const uint8_t DONE[4] = { 10, 11, 12, 13 };
const bool DONE_DOTS[4] = { false, false, false, true };

uint8_t _buttonPointer = 0;
uint8_t _digitsPointer = 0;
uint16_t _digitsBuffer[4] = { 0, 0, 0, 0 };
uint16_t buffer = 0;

Button* minusButton = new Button(0x0001, "minus");
Button* resetButton = new Button(0x0002, "reset");
Button* plusButton = new Button(0x0004, "plus");
Button* startButton = new Button(0x0008, "start");

Button* keypad[4] = {
    minusButton, resetButton, plusButton, startButton
};

void initializeDriver() {
    pinMode(_LATCH_PIN, OUTPUT);
    pinMode(_CLOCK_PIN, OUTPUT);
    pinMode(_DATA_PIN, OUTPUT);

    pinMode(_BUTTONS_PIN, INPUT);
    pinMode(_LED_ARRAY_PIN, OUTPUT);
    
    clearBuffer();
}

void flushRegister() {
    uint16_t digits_mask = _SEVEN_SEGMENT | _DIGIT_SELECT;
    buffer = (buffer & ~digits_mask) | _digitsBuffer[_digitsPointer];
    buffer = (buffer & ~_BUTTONS_MASK) | keypad[_buttonPointer]->getAddress();

    // take the latchPin low so;
    // the LEDs don't change while you're sending in bits:
    digitalWrite(_LATCH_PIN, LOW);
    // shift out the bits:
    shiftOut(_DATA_PIN, _CLOCK_PIN, LSBFIRST, buffer);
    shiftOut(_DATA_PIN, _CLOCK_PIN, LSBFIRST, buffer >> 8);
    //take the latch pin high so the LEDs will light up:
    digitalWrite(_LATCH_PIN, HIGH);

    _digitsPointer = (_digitsPointer + 1) % 4;
    _buttonPointer = (_buttonPointer + 1) % 4;
}

void clearBuffer() {
    buffer = 0;
    _buttonPointer = 0;
    _digitsPointer = 0;
    for (int i = 0; i < 3; i++) {
        _digitsBuffer[i] = 0;
    }

    flushRegister();
}

void writeDigits(const uint8_t digits[], const bool dotMap[]) {
    for (int i = 0; i < 4; i++) {
        _digitsBuffer[i] = _DIGIT_MAP[digits[3-i]] | (1 << (i + 4)) | (dotMap[i] ? 0 : _WITHOUT_DOT);
    }
}

void checkButtonsStates()
{
    bool isButtonDown = (bool) digitalRead(_BUTTONS_PIN);
    bool currentState = keypad[_buttonPointer]->getState();

    keypad[_buttonPointer]->setState(isButtonDown);

    if(currentState != isButtonDown) {
        if (isButtonDown && keypad[_buttonPointer]->onDown != nullptr) {
            keypad[_buttonPointer]->onDown(keypad[_buttonPointer]);
        }
        else if (keypad[_buttonPointer]->onUp != nullptr){
            keypad[_buttonPointer]->onUp(keypad[_buttonPointer]);
        }
    }
    else if (isButtonDown && keypad[_buttonPointer]->onHold != nullptr) {
        keypad[_buttonPointer]->onHold(keypad[_buttonPointer]);
    }
}