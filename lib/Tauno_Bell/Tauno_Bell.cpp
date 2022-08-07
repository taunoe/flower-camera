/*************************************************************
 * File: Tauno_Bell.h
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 07.08.2022
 * Tauno Erik 2022
 * 
 * To turn on Old USSR doorbell.
**************************************************************/

#include "Tauno_Bell.h"

Tauno_Bell::Tauno_Bell(
  uint8_t pin)
  : _pin(pin)
  {}

Tauno_Bell::~Tauno_Bell() {
  //
}

void Tauno_Bell::begin() {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void Tauno_Bell::on() {
  digitalWrite(_pin, HIGH);
}

void Tauno_Bell::off() {
  digitalWrite(_pin, LOW);
}

void Tauno_Bell::bell() {
  digitalWrite(_pin, HIGH);
  delay(1);
  digitalWrite(_pin, LOW);

}