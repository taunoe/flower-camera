/*************************************************************
 * File: Tauno_Status.h
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 06.08.2022
 * Tauno Erik 2022
 * 
 * Shift Register with a 7 LEDs.
**************************************************************/
#include <Arduino.h>

#if !defined(LIB_TAUNOSTATUS_H)
#define LIB_TAUNOSTATUS_H

class Tauno_Status {
  private:
  uint8_t _latch_pin;
  uint8_t _clock_pin;
  uint8_t _data_pin;

  public:
  Tauno_Status(uint8_t latch_pin,uint8_t clock_pin, uint8_t data_pin);
  ~Tauno_Status();

  void begin();
};

#endif  // LIB_TAUNOSTATUS_H
