#include "includes.hpp"

const int16_t HOLD_ACTION_TIMER = 3000;
const uint8_t FAST_SKIPPING_STEPS = 10;
const int16_t MAX_TIMER = 9999;
const int16_t MIN_TIMER = 5;
const int16_t DEFAULT_TIMER = 60;

int16_t timer = 0;
bool isTicking = false;

void increaseTimerEvent(uint32_t holdingTime) {
    bool fastSkipping = holdingTime >= HOLD_ACTION_TIMER;
    if(holdingTime == 0 || fastSkipping) {
        timer += fastSkipping ? FAST_SKIPPING_STEPS : 1;
        timer = timer > MAX_TIMER ? MAX_TIMER: timer;
    }
}

void decreaseTimerEvent(uint32_t holdingTime) {
    bool fastSkipping = holdingTime >= HOLD_ACTION_TIMER;
    if(holdingTime == 0 || fastSkipping) {
        timer += fastSkipping ? FAST_SKIPPING_STEPS : 1;
        timer = timer < MIN_TIMER ? MIN_TIMER : timer;
    }
}

void resetTimerEvent(uint32_t holdingTime) {
    if(holdingTime == 0) {
        EEPROM.get(0, timer);
    }
    else if ((holdingTime >= HOLD_ACTION_TIMER) & (timer != DEFAULT_TIMER)) {
        EEPROM.put(0, DEFAULT_TIMER);
        EEPROM.get(0, timer);
    }
}

void toggleTimerEvent() {
    isTicking = !isTicking;

    if (isTicking) {
        EEPROM.put(0, timer);
        //TODO: timer start
    }
    else {
        //TODO: timer stop

    }
}

void setup() {
    // initialize GDB stub
    // debug_init();
    // delay(5000);

    initializeDriver();
    plusButton->onDown = [](Button* button) { increaseTimerEvent(0); };
    plusButton->onHold = [](Button* button) { increaseTimerEvent(button->getHoldingTime()); };
    minusButton->onDown = [](Button* button) { decreaseTimerEvent(0); };
    minusButton->onHold = [](Button* button) { decreaseTimerEvent(button->getHoldingTime()); };

    EEPROM.begin();
    EEPROM.get(0, timer);
}

void loop() {
    checkButtonStates();
    //writeNumber(DONE, DONE_DOTS);
}

