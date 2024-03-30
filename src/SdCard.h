#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
/*
https://www.electronicwings.com/
 SD Card Interface code for ESP32
 SPI Pins of ESP32 SD card as follows:
 */
#define PIN_SD_CARD_CS 5
#define PIN_SD_CARD_MOSI 23
#define PIN_SD_CARD_MISO 19
#define PIN_SD_CARD_SCK 18
/*
 */
class SdCard
{
public:
  SdCard();
  /// @brief
  /// @param fileName
  /// @param fromStream
  void writeFile(const char *, Stream *fromStream);
  bool readFile(const char * fileName, File &file);
  bool fileExst(const char * fileName);
  bool exists(const String& str);

  bool mkdir(const String &dir);
};