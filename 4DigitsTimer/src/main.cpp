#include "includes.hpp"

const uint32_t HOLD_ACTION_TRIGGER = 250;
const uint32_t LONG_ACTION_TRIGGER = 3000;
uint8_t displayTimer[3] = { 0, 0, 0};
uint16_t selectedButton = 0;
uint32_t selectedButtonHold = 0;
bool resetToDefault = false;
bool refreshScreen = true;

void increaseTimerEvent(uint32_t holdingTime) {
    bool fastSkipping = holdingTime >= LONG_ACTION_TRIGGER;
    increaseTimer(fastSkipping);
}

void decreaseTimerEvent(uint32_t holdingTime) {
    bool fastSkipping = holdingTime >= LONG_ACTION_TRIGGER;
    decreaseTimer(fastSkipping);
}

void resetTimerEvent(uint32_t holdingTime) {
    if (!resetToDefault) {
        resetToDefault = holdingTime >= LONG_ACTION_TRIGGER;
        resetTimer(resetToDefault);
    }
}

void toggleTimerEvent() {
    toggleTimer();
    setLedState(isTicking());
    toggleBlink(!isTicking());
}

void onAlarmTrigger() {
    setLedState(false);
    // refreshScreen = false;
    // for (uint8_t i = 0; i < 2; i++) {
    //     displayTimer[i] = END[i];
    // }
}

void toggleUVEvent() {
    setLedState(getLedState());
}

void refresh() {
    checkButtonsStates();
    if (refreshScreen) {
        getDisplayTime(displayTimer);
    }
    writeDigits(displayTimer);
    flushRegister();
}

/// @brief Checks if the input has a button already pressed and thus block other actions if the button is being hold.
/// @param button  the button from the triggered event
/// @return if the action can be triggered
bool canPerformAction(Button* button) {
    uint16_t address = button->getAddress();
    uint32_t holdingTime = button->getHoldingTime();
    if (selectedButton == 0 || selectedButton == address) {
        selectedButton = address;
        if (holdingTime == 0 || holdingTime - selectedButtonHold > HOLD_ACTION_TRIGGER) {
            selectedButtonHold = holdingTime;
            if (hasFinished()) {
                toggleBlink(false);
            }
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

void releaseResetTimerEvent(Button *button) {
    releaseAction(button);
    resetToDefault = false;
}

void initializeEventListeners() {
    plusButton->onDown = [](Button* button) { if (canPerformAction(button)) increaseTimerEvent(0); };
    plusButton->onHold = [](Button* button) { if (canPerformAction(button)) increaseTimerEvent(button->getHoldingTime()); };
    plusButton->onUp = releaseAction;

    minusButton->onDown = [](Button* button) { if (canPerformAction(button)) decreaseTimerEvent(0); };
    minusButton->onHold = [](Button* button) { if (canPerformAction(button)) decreaseTimerEvent(button->getHoldingTime()); };
    minusButton->onUp = releaseAction;

    startButton->onDown = [](Button* button) { if (canPerformAction(button)) toggleTimerEvent(); };
    startButton->onHold = [](Button* button) { if (canPerformAction(button)) { toggleUVEvent(); } };
    startButton->onUp = releaseAction;

    resetButton->onDown = [](Button* button) { if (canPerformAction(button)) resetTimerEvent(0); };
    resetButton->onHold = [](Button* button) { if (canPerformAction(button)) resetTimerEvent(button->getHoldingTime()); };
    resetButton->onUp = releaseResetTimerEvent;
}

void setup() {
    // initialize GDB stub
    pinMode(PIN7, INPUT_PULLUP);
    debug_init();
    delay(5000);

    initializeDriver();
    initializeEventListeners();
    initializeTimers(onAlarmTrigger, refresh);
}

void loop() {
    tick();
}