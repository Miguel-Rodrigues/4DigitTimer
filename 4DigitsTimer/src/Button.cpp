#include "includes.hpp"

Button::Button(uint16_t address_, const char *name_)
{
    state = false;
    address = address_;
    name = name_;
    trace = 0;
    onDown = nullptr;
    onUp = nullptr;
    onHold = nullptr;
}

uint32_t Button::getHoldingTime() {
    return state ? millis() - trace : 0;
}

bool Button::getState() {
    return state;
}

void Button::setState(bool isPressed) {
    if (state != isPressed) {
        trace = millis();
        state = isPressed;
    }
}

uint16_t Button::getAddress() {
    return address;
}