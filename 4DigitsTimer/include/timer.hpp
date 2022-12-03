#ifndef TIMER_H
#define TIMER_H

#include "includes.hpp"

void initializeTimers(void (*onTrigger)(), void (*onRefreshSystem)());

void increaseTimer(bool fastSkipping);

void decreaseTimer(bool fastSkipping);

void resetTimer(bool resetToDefault = false);

bool toggleTimer();

bool isTicking();

bool hasFinished();

void tick();

void getDisplayTime(uint8_t displayTimer[]);

#endif /* TIMER_H */
