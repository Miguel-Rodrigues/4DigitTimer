#ifndef DRIVER_H
#define DRIVER_H

#include "includes.hpp"

const int BUTTONS_PIN = 2;
const int LED_ARRAY_PIN = 3;

//Pin connected to ST_CP of 74HC595
const int LATCH_PIN = 8;
//Pin connected to SH_CP of 74HC595
const int CLOCK_PIN = 9;
//Pin connected to DS of 74HC595
const int DATA_PIN = 10;

//Register address masks

const uint16_t BUTTONS_MASK = 0x000F;
const uint16_t DIGIT_SELECT = 0x00F0;
const uint16_t SEVEN_SEGMENT = 0xFF00;

const uint16_t DIGIT_MAP[14] = {
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
    0x9F00, //E
};

const uint16_t WITHOUT_DOT = 0x0100; //.

const uint8_t DONE[4] = { 10, 11, 12, 13 };
const bool DONE_DOTS[4] = { false, false, false, true };

const uint16_t defaultTimer = 60;

uint16_t buffer;

Button* minusButton = new Button(0x0001, "minus");
Button* resetButton = new Button(0x0002, "reset");
Button* plusButton = new Button(0x0004, "plus");
Button* startButton = new Button(0x0008, "start");

Button* keypad[4] = {
    minusButton, resetButton, plusButton, startButton
};

void initializeDriver();

void writeRegister();

void clearBuffer();

void writeNumber(const uint8_t digits[], const bool dotMap[]);

void checkButtonStates();

#endif /* DRIVER_H */
