#pragma once
#include <Arduino.h>
#include <vector>
#include "Image.h"
class ImageDatabase
{
    public:
    bool isImgUrlExistInDatabase(String url);
    void createImageMatrix(String url, Image & img);
    private:
    const String GetDirectoryNameByFileFormat(const String& format);
    std::vector<String> splitString(const String &inputString, char token);
    int dirtyStringToInt(const String &inputString);
    String getFileNameWithoutFormat(String str);
    ImageInfo getImgInfoFromFileName(const String &fileName, ImageInfo &info);
};