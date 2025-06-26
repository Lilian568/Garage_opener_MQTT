//
// Created by Villiam Kuusisto on 22.2.2025.
//

#include "Encoder.h"
#include <stdio.h>

volatile int Encoder::stepCount = 0;

Encoder::Encoder(uint a, uint b) : pinA(a), pinB(b) {}

void Encoder::init() {
  gpio_init(ENCODER_A);
  gpio_init(ENCODER_B);
  gpio_set_dir(ENCODER_A, GPIO_IN);
  gpio_set_dir(ENCODER_B, GPIO_IN);
  gpio_pull_up(ENCODER_A);
  gpio_pull_up(ENCODER_B);
}


void Encoder::resetStepCount() {
  stepCount = 0;
}
int Encoder::getStepCount() {
  return stepCount;
}

void Encoder::incrementStepCount() {
  stepCount++;
}

void Encoder::decrementStepCount() {
  stepCount--;
}
