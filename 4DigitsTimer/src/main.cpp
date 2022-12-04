#include "includes.hpp"

const uint16_t HOLD_ACTION_TRIGGER = 250;
const uint16_t LONG_ACTION_TRIGGER = 1500;
const uint16_t BUZZER_ALARM_TIME = 1000;
const uint32_t BUZZER_BEEP_TIME = 50;
uint8_t displayTimer[4] = { 0, 0, 0, 0 };
uint16_t selectedButton = 0;
uint32_t selectedButtonHold = 0;
bool timelessMode = false;
bool refreshScreen = true;
bool canBuzz = true;

void resetTimerEvent(bool resetToDefault) {
    if (canBuzz) buzz(BUZZER_BEEP_TIME);
    canBuzz = false;
    timelessMode = false;
    refreshScreen = true;

    if (resetTimer(resetToDefault)) {
        buzz(BUZZER_BEEP_TIME);
    };
    setLedState(false);
}

void increaseTimerEvent(bool fastSkipping) {
    if (canBuzz) buzz(BUZZER_BEEP_TIME);
    canBuzz = false;

    if (!timelessMode) {
        fastSkipping |= isTicking();
        increaseTimer(fastSkipping);
        holdBlink();
    }

    else {
        resetTimerEvent(false);
    }
}

void decreaseTimerEvent(bool fastSkipping) {
    if (canBuzz) buzz(BUZZER_BEEP_TIME);
    canBuzz = false;

    if (!timelessMode) {
        fastSkipping |= isTicking();
        decreaseTimer(fastSkipping);
        holdBlink();
    }

    else {
        resetTimerEvent(false);
    }
}

void toggleTimerEvent() {
    buzz(BUZZER_BEEP_TIME);

    if (!timelessMode) {
        setLedState(toggleTimer());
    }

    else {
        resetTimerEvent(false);
    }
}

void enableTimelessMode()
{
    if (!timelessMode) {
        buzz(BUZZER_BEEP_TIME);
    
        timelessMode = true;
        refreshScreen = false;

        resetTimer(false);
        setLedState(true);

        displayTimer[0] = BLANKS[0];
        displayTimer[1] = BLANKS[1];
        displayTimer[2] = BLANKS[2];
    }
}

void onAlarmTrigger() {
    setLedState(false);
    resetTimer(false);
    refreshScreen = false;
    displayTimer[0] = END[0];
    displayTimer[1] = END[1];
    displayTimer[2] = END[2];
    buzz(BUZZER_ALARM_TIME);
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
            if (!timelessMode) {
                refreshScreen = true;
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

    canBuzz = true;
}

void initializeEventListeners() {
    plusButton->onDown = [](Button* button) { if (canPerformAction(button)) increaseTimerEvent(false); };
    plusButton->onHold = [](Button* button) { if (canPerformAction(button)) increaseTimerEvent(selectedButtonHold > LONG_ACTION_TRIGGER); };
    plusButton->onUp = releaseAction;

    minusButton->onDown = [](Button* button) { if (canPerformAction(button)) decreaseTimerEvent(false); };
    minusButton->onHold = [](Button* button) { if (canPerformAction(button)) decreaseTimerEvent(selectedButtonHold > LONG_ACTION_TRIGGER); };
    minusButton->onUp = releaseAction;

    startButton->onDown = [](Button* button) { if (canPerformAction(button)) toggleTimerEvent(); };
    startButton->onHold = [](Button* button) { if (canPerformAction(button) && selectedButtonHold > LONG_ACTION_TRIGGER) enableTimelessMode(); };
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