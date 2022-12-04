#ifndef PINS_H
#define PINS_H

#ifdef ENV_UNO
    //Pin connected to SH_CP of 74HC595
    #define CLOCK_PIN 8
    //Pin connected to ST_CP of 74HC595
    #define LATCH_PIN 9
    //Pin connected to DS of 74HC595
    #define DATA_PIN 10
    //Pin connected to the keypad
    #define BUTTONS_PIN 11
    //Pin connected to the buzzer
    #define BUZZER_PIN 12
    
    #ifdef AVR8_DEBUG
        //Pin used to enable breakpoints
        #define DEBUG_PIN 7
    #endif
#endif

#ifdef ENV_ATTINY85
    //Pin connected to SH_CP of 74HC595
    #define CLOCK_PIN 4
    //Pin connected to ST_CP of 74HC595
    #define LATCH_PIN 3
    //Pin connected to DS of 74HC595
    #define DATA_PIN 2
    //Pin connected to the keypad
    #define BUTTONS_PIN 1
    //Pin connected to the buzzer
    #define BUZZER_PIN 0
#endif

#endif /* PINS_H */
