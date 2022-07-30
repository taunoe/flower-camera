/*************************************************************
 * File: nanomain.h
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 30.07.2022
 * Tauno Erik 2022
**************************************************************/
#include <Arduino.h>

// LEDs
#define LED_LEFT  7    // 7
#define LED_RIGHT A7   // Pin A7 is not digital output on Arduino Nano (It works on BLE)

#define BELL_PIN 11    // D11

void setup() {
  pinMode(LED_LEFT, OUTPUT);
  digitalWrite(LED_RIGHT, LOW);
  
  pinMode(LED_RIGHT, OUTPUT);
  digitalWrite(LED_RIGHT, LOW);
  
  pinMode(BELL_PIN, OUTPUT);
  digitalWrite(BELL_PIN, LOW);
}


void loop() {
  digitalWrite(LED_LEFT, HIGH);
  digitalWrite(LED_RIGHT, LOW);
  delay(500);

  digitalWrite(LED_RIGHT, LOW);
  digitalWrite(LED_LEFT, LOW);
  delay(1000);

  digitalWrite(LED_RIGHT, HIGH);
  digitalWrite(LED_LEFT, LOW);
  delay(500);

  digitalWrite(LED_RIGHT, LOW);
  digitalWrite(LED_LEFT, LOW);
  delay(1000);

  digitalWrite(BELL_PIN, HIGH);
  delay(5);
  digitalWrite(BELL_PIN, LOW);

}
