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

/* LEDs */
#define BLUE        1  // 0b00000001
#define DARK_GREEN  2  // 0b00000010 not a bright
#define GREEN       4  // 0b00000100
#define YELLOW      8  // 0b00001000
#define ORANGE     16  // 0b00010000
#define RED        32  // 0b00100000
#define DARK_RED   64  // 0b01000000 not a bright

class Tauno_Status {
  private:
  uint8_t _latch_pin;
  uint8_t _clock_pin;
  uint8_t _data_pin;
  uint8_t _register;
  uint8_t _colors[7] = {BLUE, DARK_GREEN, GREEN, YELLOW, ORANGE, RED, DARK_RED};

  public:
  Tauno_Status(uint8_t latch_pin,uint8_t clock_pin, uint8_t data_pin);
  ~Tauno_Status();

  void begin();              // Setup pins
  void write(uint8_t byte);  // Write byte to shift register
  uint8_t read();            // returns _register
  uint8_t read(uint8_t bit); // returns bit value (0 or 1)
  void test();               // Blink one by one
  void on(uint8_t bit);      // Set one bit high
  void off(uint8_t bit);     // Set on ebit low
};

#endif  // LIB_TAUNOSTATUS_H
