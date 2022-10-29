#ifndef INCLUDE_H
#define INCLUDES_H

#include <Arduino.h>
#include <EEPROM.h>
#include <arduino-timer.h>

#include "driver.hpp"
#include "button.hpp"
#include "timer.hpp"

#include "avr8-stub.h"
#define breakpoint() if (!digitalRead(PIN7)) breakpoint()


#endif /* INCLUDES_H */