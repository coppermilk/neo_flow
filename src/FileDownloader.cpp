#include "FileDownloader.h"
#include "FileNamingHelper.h"

bool FileDownloader::DownloadFileFromUrlToSd(const char *imageUrl, SdCard &sd, const String& path)
{
    String fileName = FileNamingHelper::urlToFileName(imageUrl);
    String fullName = path + fileName;

  
    if (sd.fileExst(fullName.c_str()))
    {
        //Serial.println("File: \"" + fullName + "\" is already exists, skipping download.");
        return true;
    }
    else
    {
        HTTPClient http;
        http.begin(imageUrl);

        int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK)
        {
            WiFiClient *stream = http.getStreamPtr();
            sd.writeFile(fullName.c_str(), stream);
            http.end();
            return true;
        }
        else
        {
            Serial.print("Failed to download image. Error code: ");
            Serial.println(httpCode);
            http.end();
            return false;
        }
    }
    return false;
}


