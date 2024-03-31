#pragma once

#include "Arduino.h"
class Pixel
{
public:
    union
    {
        struct
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
        uint32_t rgb;
    };

    Pixel(uint8_t, uint8_t, uint8_t);
    Pixel(unsigned);
    Pixel(const char* hexValue);
    Pixel();


    uint16_t asUint16_t();
};