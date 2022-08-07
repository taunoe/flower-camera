/*************************************************************
 * File: Tauno_LEDs.h
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 07.08.2022
 * Tauno Erik 2022
 * 
 * LEDs controlled by transistors. Two on left and two on right.
**************************************************************/

#include <Arduino.h>

#if !defined(LIB_TAUNOLEDS_H)
#define LIB_TAUNOLEDS_H

class Tauno_LEDs {
  private:
  uint8_t _left_pin;
  uint8_t _right_pin;

  public:
  Tauno_LEDs(uint8_t _left_pin, uint8_t _right_pin);
  ~Tauno_LEDs();

  void begin();
  void on();
  void off();
  void left_on();
  void right_on();
  void left_off();
  void right_off();
  void test();
};

#endif  // LIB_TAUNOLEDS_H