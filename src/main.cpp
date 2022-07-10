#include <Arduino.h>
#include <Arduino_APDS9960.h>  // Proximity and gesture sensor

const int LED_LEFT = 7;
const int LED_RIGHT = A7;

void setup() {
  Serial.begin(115200);

  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
/*
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor!");
  }
  */
}

void loop() {
  /*
  if (APDS.proximityAvailable()) {
    Serial.println("Tere");
    Serial.println(APDS.readProximity());
  }*/
  
  digitalWrite(LED_LEFT, HIGH);
  digitalWrite(LED_RIGHT, LOW);
  delay(1000);

  digitalWrite(LED_RIGHT, HIGH);
  digitalWrite(LED_LEFT, LOW);
  delay(1000);
  
}