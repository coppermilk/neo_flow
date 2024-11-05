
#include "ImageDatabase.h"
#include "FileDownloader.h"
#include "ImageConvector.h"
#include "SdCard.h"
#include "FileNamingHelper.h"

bool ImageDatabase::isImgUrlExistInDatabase(String str)
{
    FileDownloader downloader;
    String fileName = FileNamingHelper::urlToFileName(str);
    String fullName = GetDirectoryNameByFileFormat(fileName) + fileName;
    return sd.fileExst(fullName.c_str());
}

void ImageDatabase::createImageMatrix(String url, Image &img)
{
    if (url.isEmpty())
    {
        return;
    }


    String fileName = FileNamingHelper::urlToFileName(url);
    String dirName = GetDirectoryNameByFileFormat(fileName);
    if (!isImgUrlExistInDatabase(url))
    {
        FileDownloader downloader;
        downloader.DownloadFileFromUrlToSd(url.c_str(), sd, dirName);
    }

    File bmpImage;
    String fullName = dirName + fileName;
    if (sd.readFile(fullName.c_str(), bmpImage))
    {
        getImgInfoFromFileName(fileName, img.info);

        ImageConvector convector;
        convector.createImageMatrixFromBmpFile(bmpImage, img);
        if (img.info.isSprite)
        {
            img.info.w = img.img.size() / img.info.cntFrames;
        }
        return;
    }
    return;
}

const String ImageDatabase::GetDirectoryNameByFileFormat(const String &filename)
{
    if (filename.endsWith(".sprite.bmp"))
    {
        return "/img/sprite/";
    }
    if (filename.endsWith(".bmp"))
    {
        return "/img/bmp/";
    }
    return {};
}

std::vector<String> ImageDatabase::splitString(const String &inputString, char token)
{
    std::vector<String> substrings; // Vector to store substrings
    String currentSubstring = "";   // String to store current substring

    for (int i = 0; i < inputString.length(); i++)
    {
        char currentChar = inputString.charAt(i); // Get current character

        // If the current character is the token, add the current substring to the vector and reset currentSubstring
        if (currentChar == token)
        {
            substrings.push_back(currentSubstring);
            currentSubstring = ""; // Reset currentSubstring
        }
        else
        {
            currentSubstring += currentChar; // Add the current character to the currentSubstring
        }
    }

    // Add the last substring after the loop
    substrings.push_back(currentSubstring);

    return substrings;
}

int ImageDatabase::dirtyStringToInt(const String &inputString)
{
    String cleanDigit;
    for (auto ch : inputString)
    {
        if (isdigit(ch))
        {
            cleanDigit += ch;
        }
    }
    return cleanDigit.toInt();
}

String ImageDatabase::getFileNameWithoutFormat(String str)
{
    str.replace('.', '_');
    return str;
}

ImageInfo ImageDatabase::getImgInfoFromFileName(const String &fileName, ImageInfo &info)
{
    info.isSprite = fileName.endsWith(".sprite.bmp");
    if (!info.isSprite)
    {
        return info;
    }
    String nameWithoutFormat = getFileNameWithoutFormat(fileName);
    auto splittedName = splitString(nameWithoutFormat, '_');

    for (const auto &token : splittedName)
    {
       // Serial.println(token);
        if (token.endsWith("ms"))
        {
            info.msFrameDuration = dirtyStringToInt(token);
        }
        else if (token.endsWith("f"))
        {
            info.cntFrames = dirtyStringToInt(token);
        }
    }

    return info;
}