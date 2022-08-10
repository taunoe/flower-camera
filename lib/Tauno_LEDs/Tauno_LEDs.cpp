/*************************************************************
 * File: Tauno_LEDs.h
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 07.08.2022
 * Tauno Erik 2022
 * 
 * LEDs controlled by transistors. Two on left and two on right.
**************************************************************/

#include "Tauno_LEDs.h"

Tauno_LEDs::Tauno_LEDs(
  uint8_t left_pin,
  uint8_t right_pin)
  : _left_pin(left_pin),
    _right_pin(right_pin)
  {}

Tauno_LEDs::~Tauno_LEDs() {
}

void Tauno_LEDs::begin() {
  pinMode(_left_pin, OUTPUT);
  pinMode(_right_pin, OUTPUT);
}

void Tauno_LEDs::on() {
  left_on();
  right_on();
}

void Tauno_LEDs::off() {
  left_off();
  right_off();
}

void Tauno_LEDs::left_on() {
  digitalWrite(_left_pin, HIGH);
}

void Tauno_LEDs::right_on() {
  digitalWrite(_right_pin, HIGH);
}

void Tauno_LEDs::left_off() {
  digitalWrite(_left_pin, LOW);
}

void Tauno_LEDs::right_off() {
  digitalWrite(_right_pin, LOW);
}

void Tauno_LEDs::test() {
  left_on();
  delay(500);
  left_off();
  right_on();
  delay(500);
  right_off();
  delay(500);
  on();
  delay(500);
  off();
}