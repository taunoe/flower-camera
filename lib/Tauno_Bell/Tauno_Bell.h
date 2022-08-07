/*************************************************************
 * File: Tauno_Bell.h
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 07.08.2022
 * Tauno Erik 2022
 * 
 * To turn on Old USSR doorbell.
**************************************************************/
#include <Arduino.h>

#if !defined(LIB_TAUNOBELL_H)
#define LIB_TAUNOBELLS_H

class Tauno_Bell {
  private:
  uint8_t _pin;

  public:
  Tauno_Bell(uint8_t pin);
  ~Tauno_Bell();

  void begin();
  void on();
  void off();
  void bell();
};

#endif  // LIB_TAUNOBELL_H