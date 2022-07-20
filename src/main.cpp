/*************************************************************
 * File: main.h
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 09.07.2022
 * Tauno Erik 2022
**************************************************************/
#include <Arduino.h>
#include "mbed.h"
#include <Arduino_APDS9960.h>  // Proximity and gesture sensor
#include <Arduino_OV767X.h>    // Camera
#include "functions.h"         // image functions

// Camera
#define PRESSED     0
#define BUTTON      p30  // A2

//static mbed::DigitalIn button(BUTTON);

int bytes_per_frame;
int bytes_per_pixel;

//uint8_t data[320 * 240 * 2];  // QVGA RGB565
uint8_t data[160 * 120 * 2];  // QQVGA YCbCr422

template <typename T>
inline T clamp_0_255(T x) {
  return std::max(std::min(x, static_cast<T>(255)), static_cast<T>(0));
}

inline void convert_yuv422_rgb888_f(uint8_t* out, uint8_t* in) {
  // Note: U and V are swapped
	const int32_t Y0 = in[0];
	const int32_t V  = in[1];
	const int32_t Y1 = in[2];
	const int32_t U  = in[3];

  out[0] = clamp_0_255(Y0 + 1.402f * (V - 128.0f));
  out[1] = clamp_0_255(Y0 - 0.344f * (U - 128.0f) - (0.714f * (V - 128.0f)));
  out[2] = clamp_0_255(Y0 + 1.772f * (U - 128.0f));
  out[3] = clamp_0_255(Y1 + 1.402f * (V - 128.0f));
  out[4] = clamp_0_255(Y1 - 0.344f * (U - 128.0f) - (0.714f * (V - 128.0f)));
  out[5] = clamp_0_255(Y1 + 1.774f * (U - 128.0f));
}

inline void ycbcr422_to_rgb888(int32_t Y, int32_t Cb, int32_t Cr, uint8_t* out) {
  Cr = Cr - 128;
  Cb = Cb - 128;

  out[0] = clamp_0_255((int)(Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5)));
  out[1] = clamp_0_255((int)(Y - ((Cb >> 2) + (Cb >> 4) + (Cb >> 5)) - ((Cr >> 1) + (Cr >> 3) + (Cr >> 4)) + (Cr >> 5)));
  out[2] = clamp_0_255((int)(Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6)));
}

// LEDs
#define LED_LEFT  7  // 7
#define LED_RIGHT A7   // A7

void setup() {
  Serial.begin(115600);
  //while (!Serial);

/*
  //if (!Camera.begin(QVGA, RGB565, 1)) {
  if (!Camera.begin(QQVGA, YUV422, 1)) {
    Serial.println("Failed to initialize camera!");
    while (1);
  } else {
    Serial.println("OV7670 Camera initialized!");
  }

  button.mode(PullUp);

  bytes_per_pixel = Camera.bytesPerPixel();
  bytes_per_frame = Camera.width() * Camera.height() * bytes_per_pixel;
*/
  //Camera.testPattern();  // The Camera module will always return a 
                         // fixed test pattern image with color bands 

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
  
/*
  if(button == PRESSED) {
    Camera.readFrame(data);
    uint8_t rgb888[3];

    Serial.println("<image>");
    Serial.println(Camera.width());
    Serial.println(Camera.height());

    //const int step_bytes = bytes_per_pixel * 2;

    for(int i = 0; i < bytes_per_frame; i+=bytes_per_pixel * 2) {
      // Note: U and V are swapped
      const int32_t Y0 = data[i + 0];
      const int32_t Cr = data[i + 1];
      const int32_t Y1 = data[i + 2];
      const int32_t Cb = data[i + 3];
      ycbcr422_to_rgb888(Y0, Cb, Cr, &rgb888[0]);
      Serial.println(rgb888[0]);
      Serial.println(rgb888[1]);
      Serial.println(rgb888[2]);
      ycbcr422_to_rgb888(Y1, Cb, Cr, &rgb888[0]);
      Serial.println(rgb888[0]);
      Serial.println(rgb888[1]);
      Serial.println(rgb888[2]);
    }

    Serial.println("</image>");
  }
  */
}