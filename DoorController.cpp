#include "DoorController.h"
#include <cmath>

#include "pico/stdlib.h"
#include <cstdio>

extern volatile bool requestRecalibration;
DoorController::DoorController(Motor m, Encoder e, uint limitUp, uint limitDown, LedController& leds)
    : motor(m), encoder(e), limitUp(limitUp), limitDown(limitDown), calibrated(false), stepRange(0),
      doorState(STOPPEDDOOR), ledController(leds) {
    gpio_init(limitUp);
    gpio_init(limitDown);
    gpio_set_dir(limitUp, GPIO_IN);
    gpio_set_dir(limitDown, GPIO_IN);
    gpio_pull_up(limitUp);
    gpio_pull_up(limitDown);


    ledController.init();
    ledController.initPWM();
}

void DoorController::calibrate() {
    printf("Starting calibration...\n");
    Encoder::resetStepCount();

    motor.move_until_limit(1, limitUp, 1500);
    Encoder::resetStepCount();
    motor.move_until_limit(0, limitDown, 1500);

    stepRange = abs(Encoder::getStepCount());
    if (stepRange > 0) {
        calibrated = true;
        printf("Calibration successful! stepRange = %d\n", stepRange);

        Encoder::resetStepCount();
        current_position = 1;
        printf("Moving door slightly away from limit switch...\n");
        motor.move(1, 1, 1500);
        printf("Final calibration position: %d\n", current_position);

        doorState = CLOSEDDOOR;
    } else {
        printf("Calibration failed!\n");
        calibrated = false;
    }
}

void DoorController::toggleDoor() {
    if (!calibrated) {
        printf("Error: System not calibrated!\n");
        return;
    }

    switch (doorState) {
        case CLOSEDDOOR:
            startOpening();
        break;
        case OPENDOOR:
            startClosing();
        break;
        case OPENINGDOOR:
        case CLOSINGDOOR:
            stopDoor();
        break;
        case STOPPEDDOOR:
            resumeDoor();
        break;
        default:
            break;
    }
}



void DoorController::startOpening() {
    if (current_position >= stepRange - 1) {
        printf("Door is already open!\n");
        doorState = OPENDOOR;
        return;
    }

    target_position = stepRange - 1;
    moveMotorSteps = 0;
    movementActive = true;
    movementStartTime = get_absolute_time();
    initialEncoderReading = abs(Encoder::getStepCount());
    lastEncoderReading = initialEncoderReading;

    printf("Starting to open door: moving %d steps (from %d to %d)...\n", target_position - current_position, current_position, target_position);
    doorState = OPENINGDOOR;
    lastDirection = 1;
}

void DoorController::startClosing() {
    int encVal = abs(Encoder::getStepCount());

    if (encVal <= 0) {
        printf("Door is already closed!\n");
        doorState = CLOSEDDOOR;
        return;
    }

    target_position = 1;
    moveMotorSteps = 0;
    movementActive = true;
    movementStartTime = get_absolute_time();
    initialEncoderReading = encVal;
    lastEncoderReading = initialEncoderReading;

    printf("Starting to close door: moving %d steps (from %d to %d)...\n", encVal, encVal, target_position);
    doorState = CLOSINGDOOR;
    lastDirection = 0;
}
void DoorController::stopDoor() {
    if (doorState == OPENINGDOOR || doorState == CLOSINGDOOR) {
        printf("Stopping door...\n");
        motor.stop();
        current_position = Encoder::getStepCount();
        doorState = STOPPEDDOOR;
    }
}

void DoorController::resumeDoor() {
    if (doorState == STOPPEDDOOR) {
        int currentEncoder = Encoder::getStepCount();
        printf("Resuming door in reverse direction from position: %d\n", currentEncoder);

        if (lastDirection == 1) {
            startClosing();
        } else {
            startOpening();
        }
    }
}

void DoorController::handleError() {
    printf("Door Stuck! Error State Entered.\n");
    motor.stop();
    calibrated = false;
    doorState = ERRORDOOR;
    ledController.setStatus(3);
    requestRecalibration = true;

}

void DoorController::update() {
    const int step_delay_us = 1500;
    absolute_time_t now = get_absolute_time();
    uint32_t elapsed = to_ms_since_boot(now) - to_ms_since_boot(movementStartTime);

    if (doorState == CLOSEDDOOR) {
        ledController.setStatus(0);
    } else if (doorState == OPENDOOR) {
        ledController.setStatus(1);
    } else if (doorState == OPENINGDOOR || doorState == CLOSINGDOOR) {
        ledController.setStatus(2);
    }

    const uint32_t gracePeriodMs = 1000;


    if (movementActive && (doorState == OPENINGDOOR || doorState == CLOSINGDOOR)) {
        int currentEncoder = abs(Encoder::getStepCount());
        int encoderDelta = abs(currentEncoder - initialEncoderReading);

        if (elapsed > gracePeriodMs) {
            if (elapsed > stuckThresholdMs && encoderDelta < MIN_EXPECTED_MOVEMENT) {
                printf("Door appears stuck! (No encoder change: delta = %d, elapsed = %d ms)\n", encoderDelta, elapsed);
                handleError();
                return;
            } else if (encoderDelta >= MIN_EXPECTED_MOVEMENT) {
                movementStartTime = now;
                initialEncoderReading = currentEncoder;
            }
        }
    }
    int encVal = abs(Encoder::getStepCount());
    current_position = encVal;

    if (doorState == OPENINGDOOR) {
        if (encVal < target_position) {
            motor.step(1, step_delay_us);
        } else {
            printf("Door fully opened at position %d.\n", encVal);
            doorState = OPENDOOR;
            movementActive = false;
            motor.stop();
        }
    } else if (doorState == CLOSINGDOOR) {
        if (encVal > target_position) {
            motor.step(0, step_delay_us);
        } else {
            printf("Door fully closed at position %d.\n", encVal);
            doorState = CLOSEDDOOR;
            movementActive = false;
            motor.stop();
        }
    }
}
