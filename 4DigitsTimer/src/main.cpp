#include "includes.hpp"

const uint32_t HOLD_ACTION_TRIGGER = 250;
const uint32_t LONG_ACTION_TRIGGER = 1500;
uint8_t displayTimer[3] = { 0, 0, 0 };
uint16_t selectedButton = 0;
uint32_t selectedButtonHold = 0;
bool indefiniteUVMode = false;
bool refreshScreen = true;

void increaseTimerEvent(bool fastSkipping) {
    fastSkipping |= isTicking();
    increaseTimer(fastSkipping);
    holdBlink();
}

void decreaseTimerEvent(bool fastSkipping) {
    fastSkipping |= isTicking();
    decreaseTimer(fastSkipping);
    holdBlink();
}

void resetTimerEvent(bool resetToDefault) {
    resetTimer(resetToDefault);
    setLedState(false);
}

void toggleTimerEvent() {
    bool ticking;

    ticking = toggleTimer();
    setLedState(ticking);
}

void onAlarmTrigger() {
    setLedState(false);
    resetTimer(false);
    refreshScreen = false;
    displayTimer[0] = END[0];
    displayTimer[1] = END[1];
    displayTimer[2] = END[2];
}

void refresh() {
    checkButtonsStates();
    if (refreshScreen) {
        getDisplayTime(displayTimer);
    }

    toggleBlink(!hasFinished() && !isTicking());
    writeDigits(displayTimer);
    flushRegister();
}

/// @brief Checks if the input has a button already pressed and blocks other actions if the button is being hold.
/// @param button  the button from the triggered event
/// @return if the action can be triggered
bool canPerformAction(Button* button) {
    uint16_t address = button->getAddress();
    uint32_t holdingTime = button->getHoldingTime();
    if (selectedButton == 0 || selectedButton == address) {
        selectedButton = address;
        if (holdingTime == 0 || holdingTime - selectedButtonHold > HOLD_ACTION_TRIGGER) {
            selectedButtonHold = holdingTime;
            refreshScreen = true;
            return true;
        };
    }

    return false;
}

void releaseAction(Button* button) {
    if (selectedButton == button->getAddress()) {
        selectedButton = 0;
        selectedButtonHold = 0;
    }
}

void initializeEventListeners() {
    plusButton->onDown = [](Button* button) { if (canPerformAction(button)) increaseTimerEvent(false); };
    plusButton->onHold = [](Button* button) { if (canPerformAction(button)) increaseTimerEvent(selectedButtonHold > LONG_ACTION_TRIGGER); };
    plusButton->onUp = releaseAction;

    minusButton->onDown = [](Button* button) { if (canPerformAction(button)) decreaseTimerEvent(false); };
    minusButton->onHold = [](Button* button) { if (canPerformAction(button)) decreaseTimerEvent(selectedButtonHold > LONG_ACTION_TRIGGER); };
    minusButton->onUp = releaseAction;

    startButton->onDown = [](Button* button) { if (canPerformAction(button)) toggleTimerEvent(); };
    // startButton->onHold = [](Button* button) { if (canPerformAction(button)) toggleTimerEvent(selectedButtonHold > LONG_ACTION_TRIGGER); };
    startButton->onUp = releaseAction;

    resetButton->onDown = [](Button* button) { if (canPerformAction(button)) resetTimerEvent(false); };
    resetButton->onHold = [](Button* button) { if (canPerformAction(button)) resetTimerEvent(selectedButtonHold > LONG_ACTION_TRIGGER); };
    resetButton->onUp = releaseAction;
}

void setup() {
    #ifdef AVR8_DEBUG
        // initialize GDB stub
        debug_init();    // initialize the debugger
        pinMode(PIN7, INPUT_PULLUP);
        delay(5000);
    #endif

    initializeDriver();
    initializeEventListeners();
    initializeTimers(onAlarmTrigger, refresh);
}

void loop() {
    tick();
}