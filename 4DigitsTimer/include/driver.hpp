#ifndef DRIVER_H
#define DRIVER_H

#include "includes.hpp"

extern uint16_t buffer;

extern Button* minusButton;
extern Button* resetButton;
extern Button* plusButton;
extern Button* startButton;

extern Button* keypad[4];

extern const uint8_t END[3];
extern const uint8_t BLANKS[3];
extern const uint8_t CLEAR_SCREEN[3];

void initializeDriver();

void flushRegister();

void clearBuffer();

void writeDigits(const uint8_t digits[]);

void checkButtonsStates();

void toggleBlink(bool blink);

void holdBlink();

bool getLedState();

void setLedState(bool uvLedsEnabled);

#endif /* DRIVER_H */
