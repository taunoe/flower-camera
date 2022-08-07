/*************************************************************
 * File: Tauno_Status.cpp
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 06.08.2022
 * Tauno Erik 2022
**************************************************************/

#include "Tauno_Status.h"

Tauno_Status::Tauno_Status(
  uint8_t latch_pin,
  uint8_t clock_pin,
  uint8_t data_pin)
  : _latch_pin(latch_pin),
    _clock_pin(clock_pin),
    _data_pin(data_pin)
  {}

// Cleanup
Tauno_Status::~Tauno_Status() {
  //
}

void Tauno_Status::begin() {
  pinMode(_latch_pin, OUTPUT);
  pinMode(_clock_pin, OUTPUT);
  pinMode(_data_pin, OUTPUT);
}

