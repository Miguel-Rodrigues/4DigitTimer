#ifndef DRIVER_H
#define DRIVER_H

#include "includes.hpp"

extern uint16_t buffer;

extern Button* minusButton;
extern Button* resetButton;
extern Button* plusButton;
extern Button* startButton;

extern Button* keypad[4];

extern const uint8_t DONE[4];
extern const bool DONE_DOTS[4];

void initializeDriver();

void flushRegister();

void clearBuffer();

void writeDigits(const uint8_t digits[], const bool dotMap[]);

void checkButtonsStates();

#endif /* DRIVER_H */
