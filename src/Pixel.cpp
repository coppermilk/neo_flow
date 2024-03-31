#include "pixel.h"

Pixel::Pixel(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0)
{
  this->r = r;
  this->g = g;
  this->b = b;
}
Pixel::Pixel(unsigned i)
{
  this->b = i & 255;
  this->g = (i >> 8) & 255;
  this->r = (i >> 16) & 255;
}

Pixel::Pixel(const char *hexValue)
{
  if (strlen(hexValue) == 6)
  {
    rgb = strtoul(hexValue, NULL, 16);
  }
  else
  {
    // Handle error or default case here
    // For simplicity, let's default to black if input is invalid
    rgb = 0;
  }
}

Pixel::Pixel()
{
  rgb = 0;
}

uint16_t Pixel::asUint16_t()
{
  return ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3);
}