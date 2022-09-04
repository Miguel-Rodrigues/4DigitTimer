
#ifndef KEY_PAD_BUTTON_H
#define KEY_PAD_BUTTON_H

#include "includes.hpp"

class Button {
        uint32_t trace;
        bool state;
        uint16_t address;

    /* data */
    public:
        char const *name;

        void (*onDown)(Button*);
        void (*onUp)(Button*);
        void (*onHold)(Button*);
        
        Button(uint16_t address_, const char *name_);
        uint32_t getHoldingTime();
        bool getState();
        void setState(bool isPressed);
        uint16_t getAddress();
};

#endif /* KEY_PAD_BUTTON_H */