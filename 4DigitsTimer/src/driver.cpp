#include "includes.hpp"
#include <EEPROM.h>

#ifdef ENV_UNO
    //Pin connected to ST_CP of 74HC595
    const int _LATCH_PIN = 8;
    //Pin connected to SH_CP of 74HC595
    const int _CLOCK_PIN = 9;
    //Pin connected to DS of 74HC595
    const int _DATA_PIN = 10;
    //Pin connected to the keypad
    const int _BUTTONS_PIN = 11;
    //Pin connected to the buzzer
    const int _BUZZER_PIN = 12;
#endif

#ifdef ENV_ATTINY85
    //Pin connected to ST_CP of 74HC595
    const int _LATCH_PIN = 2;
    //Pin connected to SH_CP of 74HC595
    const int _CLOCK_PIN = 3;
    //Pin connected to DS of 74HC595
    const int _DATA_PIN = 7;
    //Pin connected to the keypad
    const int _BUTTONS_PIN = 5;
    //Pin connected to the buzzer
    const int _BUZZER_PIN = 6;
#endif

// Visual Reference:
// | | | | | | |  | |    |    |   |     |     |      |     |   | |  |  |   //
// A B C D E F G  | D1   D2   D3  |     R     |      U     |   S P RST M   //
//       FF       |       E       |     1     |      1     |       F       //
// _SEVEN_SEGMENT | _DIGIT_SELECT | _UV_ARRAY | _RED_ARRAY | _BUTTONS_MASK //

// Visual reference of each 7 segment digit (0: ON, 1: OFF)
// ┌ A ┐
// F   B
// ├ G ┤
// E   C
// └ D ┘ *

//Register address masks
const uint16_t _SEVEN_SEGMENT = 0xFE00; //1111111000000000
const uint16_t _DIGIT_SELECT  = 0x01C0; //0000000111000000
const uint16_t _UV_ARRAY      = 0x0020; //0000000000100000
const uint16_t _RED_ARRAY     = 0x0010; //0000000000010000
const uint16_t _BUTTONS_MASK  = 0x000F; //0000000000001111

const uint16_t _DIGIT_MAP[15] = {
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
    0x6000, //E
    0xD400, //n
    0x8400, //d
    0xFC00, //-
    0xFE00  // <clear>
};

const uint8_t END[3] = { 10, 11, 12 };
const uint8_t BLANKS[3] = { 13, 13, 13 };
const uint8_t CLEAR_SCREEN[3] = { 14, 14, 14 };

const uint32_t _BLINK_TIME = 1000;
const uint16_t _BUZZER_TONE = 1000; //Hz

uint64_t _blinkingMillis = 0;
bool _isBlinking = false;
bool _blink = false;
bool _holdBlink = false;
bool _uvLedsEnabled = false;

uint8_t _buttonPointer = 0;
uint8_t _digitsPointer = 0;
uint16_t _digitsBuffer[3] = { 0, 0, 0 };
uint16_t _buffer = 0;

Button* minusButton = new Button(0x0001, "minus");
Button* resetButton = new Button(0x0002, "reset");
Button* plusButton = new Button(0x0004, "plus");
Button* startButton = new Button(0x0008, "start");

Button* keypad[4] = {
    minusButton, resetButton, plusButton, startButton
};

void clearBuffer() {
    _buffer = 0;
    _buttonPointer = 0;
    _digitsPointer = 0;
    for (int i = 0; i < 3; i++) {
        _digitsBuffer[i] = 0;
    }

    flushRegister();
}

void initializeDriver() {
    pinMode(_LATCH_PIN, OUTPUT);
    pinMode(_CLOCK_PIN, OUTPUT);
    pinMode(_DATA_PIN, OUTPUT);

    pinMode(_BUTTONS_PIN, INPUT);
    pinMode(_BUZZER_PIN, OUTPUT);

    clearBuffer();
    setLedState(false);
}

const uint8_t* checkBlinkMode(const uint8_t* digits) {
    uint64_t currentMillis = millis();
    if (_isBlinking && currentMillis >= (_blinkingMillis + _BLINK_TIME)) {
        _blinkingMillis = currentMillis;
        if (!_holdBlink) {
            _blink = !_blink;
        }
        else {
            _holdBlink = !_holdBlink;
            _blink = false;
        }

    }

    if (_blink) {
        return CLEAR_SCREEN;
    }

    return digits;
}

void flushRegister() {
    const uint16_t _DIGITS_MASK = _SEVEN_SEGMENT | _DIGIT_SELECT;
    _buffer = (_buffer & ~_DIGITS_MASK) | _digitsBuffer[_digitsPointer];
    _buffer = (_buffer & ~_BUTTONS_MASK) | keypad[_buttonPointer]->getAddress();

    // take the latchPin low so;
    // the LEDs don't change while you're sending in bits:
    digitalWrite(_LATCH_PIN, LOW);
    // shift out the bits:
    shiftOut(_DATA_PIN, _CLOCK_PIN, LSBFIRST, _buffer);
    shiftOut(_DATA_PIN, _CLOCK_PIN, LSBFIRST, _buffer >> 8);
    //take the latch pin high so the LEDs will light up:
    digitalWrite(_LATCH_PIN, HIGH);

    _digitsPointer = (_digitsPointer + 1) % 3;
    _buttonPointer = (_buttonPointer + 1) % 4;
}

void writeDigits(const uint8_t digits[]) {
    const uint8_t* value = checkBlinkMode(digits);

    for (int i = 0; i < 3; i++) {
        _digitsBuffer[i] = (_DIGIT_MAP[value[2 - i]]) | (1 << (i + 6));
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

void toggleBlink(bool enabled) {
    if (_isBlinking != enabled) {
        _isBlinking = enabled;
        _blink = false;
    }
}

void holdBlink() {
    _blink = false;
    _holdBlink = true;
}

bool getLedState() {
    return _uvLedsEnabled;
}

void setLedState(bool uvLedsEnabled) {
    _uvLedsEnabled = uvLedsEnabled;
    _buffer = (_buffer & ~_RED_ARRAY) | (_uvLedsEnabled ? 0 : _RED_ARRAY);
    _buffer = (_buffer & ~_UV_ARRAY)  | (_uvLedsEnabled ? _UV_ARRAY :  0);
}

void buzz(int milliseconds) {
    tone(_BUZZER_PIN, _BUZZER_TONE, milliseconds);
}