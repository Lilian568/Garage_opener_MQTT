//
// Created by Villiam Kuusisto on 22.2.2025.
//

#ifndef DOORCONTROLLER_H
#define DOORCONTROLLER_H

#include "Motor.h"
#include "Encoder.h"
#include "LedController.h"

enum DoorState {
    CLOSEDDOOR,
    OPENDOOR,
    OPENINGDOOR,
    CLOSINGDOOR,
    STOPPEDDOOR,
    ERRORDOOR
};

class DoorController {
private:
    Motor motor;
    Encoder encoder;
    uint limitUp, limitDown;
    bool calibrated;
    int stepRange;
    int current_position = 0;
    DoorState doorState = CLOSEDDOOR;
    int lastDirection = 1;
    int target_position{};
    int moveMotorSteps{};
    uint32_t lastEncoderUpdateTime = 0;
    int lastEncoderStepCount = 0;
    const uint32_t STUCK_TIMEOUT_MS = 330;
    absolute_time_t movementStartTime{};
    int initialEncoderReading = 0;
    int lastEncoderReading = 0;
    bool movementActive = false;
    const uint32_t stuckThresholdMs = 2000;
    const int MIN_EXPECTED_MOVEMENT = 2;
    LedController& ledController;

public:
    DoorController(Motor m, Encoder e, uint limitUp, uint limitDown, LedController& leds);

    void calibrate();
    //void openDoor();
    //void closeDoor();
    void toggleDoor();
    void stopDoor();
    void resumeDoor();
    void handleError();
    void startOpening();
    void startClosing();
    void update ();
    bool isCalibrated() const { return calibrated; }
};

#endif //DOORCONTROLLER_H
