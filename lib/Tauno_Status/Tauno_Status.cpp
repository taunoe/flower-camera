/*************************************************************
 * File: Tauno_Status.cpp
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 06.08.2022
 * Tauno Erik 2022 
**************************************************************/

#include "Tauno_Status.h"

/* Init pins */
Tauno_Status::Tauno_Status(
  uint8_t latch_pin,
  uint8_t clock_pin,
  uint8_t data_pin)
  : _latch_pin(latch_pin),
    _clock_pin(clock_pin),
    _data_pin(data_pin)
  {
  }

// Cleanup
Tauno_Status::~Tauno_Status() {
  //
}

/* Setup pins */
void Tauno_Status::begin() {
  pinMode(_latch_pin, OUTPUT);
  pinMode(_clock_pin, OUTPUT);
  pinMode(_data_pin, OUTPUT);
  _register = 0;
}

/* Write byte to shift register*/
void Tauno_Status::write(uint8_t byte) {
  digitalWrite(_latch_pin, LOW);
  shiftOut(_data_pin, _clock_pin, LSBFIRST, byte);
  digitalWrite(_latch_pin, HIGH);
}

/* Returns the register */
uint8_t Tauno_Status::read() {
  return _register;
}

/* Returns bit value (0 or 1) */
uint8_t Tauno_Status::read(uint8_t bit) {
  return _register & bit;
}

/* Set one bit high */
void Tauno_Status::on(uint8_t bit) {
  _register |= bit;
  write(_register);
}

/* Set one bit low */
void Tauno_Status::off(uint8_t bit) {
  _register &= ~(bit);
  write(_register);
}

/* Blink one by one */
void Tauno_Status::test() {
  /*
 uint8_t data = 0b10000000;

 for (uint8_t i = 0; i < 8; i++){
  write(data);
  data = data >> 1;
  delay(500);
 }
 write(0);
 */
 for (uint8_t i = 0; i < 8; i++) {
  on(_colors[i]);
  delay(500);
  off(_colors[i]);
 }
 
}