
#include "LedController.h"
#include "pico/time.h"


LedController::LedController(std::vector<uint> led_pins) : leds(led_pins), brightness(MAX_BRIGHTNESS / 20) {}

void LedController::init() {
    for (uint led : leds) {
        gpio_init(led);
        gpio_set_dir(led, GPIO_OUT);
    }
}

void LedController::initPWM() {
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv_int(&config, DIVIDER);
    pwm_config_set_wrap(&config, PWM_FREQ - 1);

    for (uint led : leds) {
        uint slice = pwm_gpio_to_slice_num(led);
        uint channel = pwm_gpio_to_channel(led);
        pwm_set_enabled(slice, false);
        pwm_init(slice, &config, false);
        pwm_set_chan_level(slice, channel, LEVEL + 1);
        gpio_set_function(led, GPIO_FUNC_PWM);
        pwm_set_enabled(slice, true);
    }
    turnOff();
}


void LedController::setStatus(int doorState) {
    switch (doorState) {
        case 0: // CLOSED
            pwm_set_gpio_level(leds[0], brightness);
        pwm_set_gpio_level(leds[1], MIN_BRIGHTNESS);
        pwm_set_gpio_level(leds[2], MIN_BRIGHTNESS);
        break;
        case 1: // OPEN
            pwm_set_gpio_level(leds[0], MIN_BRIGHTNESS);
        pwm_set_gpio_level(leds[1], brightness);
        pwm_set_gpio_level(leds[2], MIN_BRIGHTNESS);
        break;
        case 2: // MOVING
            pwm_set_gpio_level(leds[0], MIN_BRIGHTNESS);
        pwm_set_gpio_level(leds[1], MIN_BRIGHTNESS);
        pwm_set_gpio_level(leds[2], brightness);
        break;
        case 3: // ERROR (LED BLINKS)
            blinkError();
        break;
        default:
            turnOff();
        break;
    }
}

void LedController::turnOn() {
    for (uint led : leds) {
        pwm_set_gpio_level(led, brightness);
    }
}

void LedController::turnOff() {
    for (uint led : leds) {
        pwm_set_gpio_level(led, MIN_BRIGHTNESS);
    }
}

void LedController::blinkError() {
    for (int i = 0; i < 3; i++) {
        turnOn();
        sleep_ms(BLINK_SLEEP_TIME);
        turnOff();
        sleep_ms(BLINK_SLEEP_TIME);
    }
}
