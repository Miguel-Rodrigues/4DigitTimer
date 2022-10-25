#ifndef TIMER_H
#define TIMER_H

#include "includes.hpp"

extern void (*onTrigger)();
extern bool (*onRefreshSystem)();

void initializeTimers();

void increaseTimer(bool fastSkipping);

void decreaseTimer(bool fastSkipping);

void resetTimer(bool resetToDefault);

void toggleTimer();

bool isTicking();

Timer<10U, &millis> getTimer();

void getDisplayTime(uint8_t displayTimer[]);

#endif /* TIMER_H */
