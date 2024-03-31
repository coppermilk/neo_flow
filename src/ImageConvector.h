#pragma once
#include <Arduino.h>
#include <SD.h>
#include <vector>

#include "Image.h"

class ImageConvector{
    public:
    static void createImageMatrixFromBmpFile(File &file, Image& img);
    static uint16_t createPixelColor(uint8_t red, uint8_t green, uint8_t blue);
    static uint16_t createPixelColor(uint32_t argb);
    static unsigned int colorStringToUInt(String colorString);
    static uint16_t colorStringToUint16(String colorString);
    static std::vector<std::vector<uint16_t>> rotateImage90(const std::vector<std::vector<uint16_t>> &image);

    static unsigned int HexToDec(const char *source);
};