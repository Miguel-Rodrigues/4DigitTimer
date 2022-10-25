#include "includes.hpp"
#include <math.h>

const uint16_t _FAST_SKIPPING_STEPS = 10;
const uint16_t _MAX_TIMER = 999;
const uint16_t _MIN_TIMER = 5;
const uint16_t _DEFAULT_TIMER = 60;

auto _timer = Timer<10, millis>();
uintptr_t _timerTask;

uint16_t _time = 0;
bool _isTicking = false;
bool _hasFinished = true;

void (*onTrigger)();
bool (*onRefreshSystem)();

bool refreshSystemEvent(void* args) {
    return onRefreshSystem();
}

void initializeTimers() {
    EEPROM.begin();
    EEPROM.get(0, _time);
    _timer.every(5, refreshSystemEvent);
}

bool timer_onTick(void* args) {
    _time--;
    bool timeout = _time == 0;
    if (timeout) {
        _isTicking = false;
        _hasFinished = true;

        if (onTrigger != nullptr) {
            onTrigger();
        }

        return false;
    };
    
    return true;
}

void increaseTimer(bool fastSkipping) {
    _time += fastSkipping ? _FAST_SKIPPING_STEPS : 1;
    _time = _time > _MAX_TIMER ? _MAX_TIMER: _time;
}

void decreaseTimer(bool fastSkipping) {
    _time -= fastSkipping ? _FAST_SKIPPING_STEPS : 1;
    _time = _time < _MIN_TIMER ? _MIN_TIMER : _time;
}

void resetTimer(bool resetToDefault) {
    if (resetToDefault && (_time != _DEFAULT_TIMER)) {
        EEPROM.put(0, _DEFAULT_TIMER);
    }

    EEPROM.get(0, _time);
}

void toggleTimer() {
    _isTicking = !_isTicking;
    
    //If starts ticking
    if (_isTicking) {
        _timerTask = _timer.every(1000, timer_onTick);

        if (_hasFinished)
        {
            EEPROM.put(0, _time);
            _hasFinished = false;
        }
    }
    else {
        _timer.cancel(_timerTask);
    }

    if (!_isTicking && _hasFinished) {
        resetTimer(false);
    }
}

Timer<10U, &millis> getTimer() {
    return _timer;
}

uint32_t getTime() {
    return _time;
}

void getDisplayTime(uint8_t displayTimer[]) {
    displayTimer[0] = (uint8_t)((_time / 100) % 10);
    displayTimer[1] = (uint8_t)((_time / 10) % 10);
    displayTimer[2] = (uint8_t)(_time % 10);
}

bool isTicking() {
    return _isTicking;
}