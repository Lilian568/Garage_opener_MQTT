//
// Created by Lilian on 23.2.2025.
//

#ifndef BUTTONS_H
#define BUTTONS_H
#include "pico/stdlib.h"

class Button {
private:
    uint pin;
    uint32_t lastPressTime;
    uint32_t debounceDelay;
    bool lastState;
public:
    Button(uint pin);
    void init() const;
    bool isPressed() const;
    bool isPressedDebounced();
};

#endif //BUTTONS_H
