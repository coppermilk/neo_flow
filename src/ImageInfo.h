
#pragma once
#include <cstdint>

struct ImageInfo
{
  bool isSprite = false;
  int16_t cntFrames = 0;  // Frame number
  int16_t msFrameDuration = 0; // Time in milliseconds
  int16_t w = 0;
  int16_t h = 0;
};