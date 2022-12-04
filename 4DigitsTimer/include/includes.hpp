#ifndef INCLUDE_H
#define INCLUDES_H

#include <Arduino.h>
#include <EEPROM.h>
#include <arduino-timer.h>

#include "pins.hpp"

#include "driver.hpp"
#include "button.hpp"
#include "timer.hpp"

#ifdef AVR8_DEBUG
    #include "avr8-stub.h"

    #define breakpoint() if (!digitalRead(PIN7)) breakpoint()
#endif

#endif /* INCLUDES_H */