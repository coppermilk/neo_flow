#include "SdCard.h"

SdCard::SdCard()
{
    //printf("Initializing SD card...\n");
    if (!SD.begin(PIN_SD_CARD_CS))
    {
        Serial.println("Initialization failed!");
        return;
    }

    if (!SD.exists("/img"))
    {
        if (!SD.mkdir("/img"))
        {
            Serial.println("Failed to create directory: /img");
        }
        Serial.println("Directory created: /img");
    }

    if (!SD.exists("/img/bmp"))
    {
        if (!SD.mkdir("/img/bmp"))
        {
            Serial.println("Failed to create directory: /img/bmp");
        }
        Serial.println("Directory created: /img/bmp");
    }

    if (!SD.exists("/img/sprite"))
    {
        if (!SD.mkdir("/img/sprite"))
        {
            Serial.println("Failed to create directory: /img/sprite");
        }
        Serial.println("Directory created: /img/sprite");
    }

   // Serial.println("Initialization done.");
}

void SdCard::writeFile(const char *fileName, Stream *fromStream)
{
    //  WiFiClient *stream = http.getStreamPtr();
    File file = SD.open(fileName, FILE_WRITE);
    if (file)
    {
        while (fromStream->available())
        {
            file.write(fromStream->read());
        }
        file.close();
        Serial.println("Image downloaded and saved successfully");
    }
    else
    {
        Serial.println("Error opening file for writing");
    }
}

bool SdCard::readFile(const char *fileName, File &file)
{
    file = SD.open(fileName);
    if (!file)
    {
        Serial.println("Failed to open file!");

        return false;
    }

    return true;
}

bool SdCard::fileExst(const char *path)
{
    return SD.exists(path);
}

bool SdCard::exists(const String &path)
{
    return SD.exists(path);
}

bool SdCard::mkdir(const String &dir)
{
    return SD.mkdir(dir);
}