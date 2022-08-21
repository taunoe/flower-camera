/*************************************************************
 * File: functions.h
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 11.07.2022
 * Tauno Erik 2022
 * 
 * Functions to build dataset
**************************************************************/
#include <Arduino.h>

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

/* From TinyML book:
The function takes 2 bytes from the input buffer to form the 16-bit RGB565 pixel.
The first byte (in[0]) is left-shifted by eight positions to place it in the higher half
of the uint16_t p variable. The second byte (in[1]) is set in the lower part.
Once we have the 16-bit pixel, we get the 8-bit color components from p by
right-shifting each channel towards the beginning of the least significant byte:
* The 8-bit red channel (out[0]) is obtained by shifting p by 11 positions so
that R0 is the first bit of the uint16_t variable. After, we clear all the non-red
bits by applying a bitmask with 0x1F (all bits cleared except the first five).
* The 8-bit green channel (out[1]) is obtained by shifting p by five positions so
that G0 is the first bit of the uint16_t variable. After, we clear all the non-green
bits by applying a bitmask with 0x3F (all bits cleared except the first six).
* The 8-bit blue channel (out[2]) is obtained without shifting because B0 is
already the first bit of the uint16_t variable. Therefore, we just need to clear
the non-blue bits by applying a bitmask with 0x1F (all bits cleared except the
first five).
In the end, we perform an extra left-shifting to move the most significant bit of each
channel to the eighth position of the byte.
*/
inline void rgb565_to_rgb888(uint8_t* in, uint8_t* out) {
  uint16_t p = (in[0] << 8) | in[1];

  out[0] = ((p >> 11) & 0x1f) << 3;
  out[1] = ((p >> 5) & 0x3f) << 2;
  out[2] = (p & 0x1f) << 3;
}

