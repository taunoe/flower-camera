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
  // LEDs
  uint8_t dark_red    = 0b10000000; // not bright
  uint8_t red         = 0b01000000;
  uint8_t orange      = 0b00100000;
  uint8_t yellow      = 0b00010000;
  uint8_t light_green = 0b00001000;
  uint8_t green       = 0b00000100; // not bright
  uint8_t blue        = 0b00000010;

  public:
  Tauno_Status(uint8_t latch_pin,uint8_t clock_pin, uint8_t data_pin);
  ~Tauno_Status();

  void begin();
  void write(uint8_t byte);
  void test();  // Blink one by one
};

#endif  // LIB_TAUNOSTATUS_H
