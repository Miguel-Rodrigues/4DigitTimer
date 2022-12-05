#include "includes.hpp"
#include <math.h>

const int _FAST_SKIPPING_STEPS = 10;
const int _MAX_TIMER = 999;
const int _MIN_TIMER = 5;
const int _DEFAULT_TIMER = 60;
const int _SYSTEM_MILLIS = 1;
const int _TIMER_MILLIS = 1000;
Timer<1> _systemTimer;
Timer<1> _timer;
Timer<1> _BuzzerTimer;
uintptr_t _timerTask;

int _time = 0;
bool _isTicking = false;
bool _hasFinished = true;

void (*_onTrigger)();
void (*_onRefreshSystem)();

bool refreshSystemEvent(void* args) {
    _onRefreshSystem();
    return true;
}

void initializeTimers(void (*onTrigger)(), void (*onRefreshSystem)()) {
    EEPROM.begin();
    EEPROM.get(0, _time);

    _onTrigger = onTrigger;
    _onRefreshSystem = onRefreshSystem;

    _systemTimer.every(_SYSTEM_MILLIS, refreshSystemEvent);
}

bool timer_onTick(void* args) {
    _time--;
    bool timeout = _time <= 0;
    if (timeout) {
        _isTicking = false;
        _hasFinished = true;
        _onTrigger();

        return false;
    };

    return true;
}

void increaseTimer(bool fastSkipping) {
    _time += fastSkipping ? _FAST_SKIPPING_STEPS : 1;
    _time = min(_MAX_TIMER, _time);
}

void decreaseTimer(bool fastSkipping) {
    _time -= fastSkipping ? _FAST_SKIPPING_STEPS : 1;
    _time = max(_MIN_TIMER, _time);
}

bool resetTimer(bool resetToDefault) {
    _isTicking = false;
    _hasFinished = true;
    _timer.cancel(_timerTask);
    EEPROM.get(0, _time);

    if (resetToDefault &&  _time != _DEFAULT_TIMER) {
        _time = EEPROM.put(0, _DEFAULT_TIMER);
        return true;
    }

    return false;
}

bool toggleTimer() {
    _isTicking = !_isTicking;
    //If starts ticking
    if (_isTicking) {
        if (_hasFinished) {
            EEPROM.put(0, _time);
            _hasFinished = false;
        }

        _timerTask = _timer.every(_TIMER_MILLIS, timer_onTick);
    }
    else {
        _timer.cancel(_timerTask);
    }

    return _isTicking;
}

void tick() {
    _systemTimer.tick<void>();
    _timer.tick<void>();
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

bool hasFinished() {
    return _hasFinished;
}