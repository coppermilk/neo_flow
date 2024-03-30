#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include <SdCard.h>

class FileDownloader{
    public:
    FileDownloader() = default;
    bool DownloadFileFromUrlToSd(const char *imageUrl, SdCard& sd, const String& path);
    bool createNestedDir(SdCard &sd, const String &dirPath);
};