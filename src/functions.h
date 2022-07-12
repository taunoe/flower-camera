/*************************************************************
 * File: functions.h
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 11.07.2022
 * Tauno Erik 2022
**************************************************************/

#include <Arduino.h>

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
