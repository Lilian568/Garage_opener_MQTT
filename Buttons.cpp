#include "Buttons.h"
#include "pico/stdlib.h"


Button::Button(uint pin) : pin(pin), lastPressTime(0), debounceDelay(300), lastState(false) {}

void Button::init() const {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

bool Button::isPressed() const {
    return gpio_get(pin) == 0;
}

bool Button::isPressedDebounced() {

    uint32_t currentTime = to_ms_since_boot(get_absolute_time());
    bool currentPressed = (gpio_get(pin) == 0);

    if (currentPressed && !lastState && (currentTime - lastPressTime > debounceDelay)) {
        lastPressTime = currentTime;
        lastState = currentPressed;
        return true;
    }
    lastState = currentPressed;
    return false;
}
