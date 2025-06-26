//
// Created by Villiam Kuusisto on 22.2.2025.
//

#ifndef ENCODER_H
#define ENCODER_H

#include "pico/stdlib.h"

#define ENCODER_A 14
#define ENCODER_B 15

class Encoder {
private:
    uint pinA, pinB;
    static volatile int stepCount;

public:
    Encoder(uint a, uint b);

    void init();

    static void encoder_callback(uint gpio, uint32_t events);

    static void updateStepCount();


    static void resetStepCount();
    static int getStepCount();
    static void incrementStepCount();
    static void decrementStepCount();
};

#endif //ENCODER_H
