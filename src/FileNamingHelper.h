#pragma once

#ifndef FileNamingHelper_h
#define FileNamingHelper_h
#include <Arduino.h>

class FileNamingHelper
{
public:
    static String urlToFileName(const String &fromSource)
    {
        String result;
        result.reserve(fromSource.length());

        for (size_t i = 0; i < fromSource.length(); ++i)
        {
            if (i == 0 || (fromSource[i] != '\\' && fromSource[i] != ':' && fromSource[i] != '/'))
            {
                result += fromSource[i];
            }
        }
        Serial.print(fromSource);
        Serial.print(" -> ");
        Serial.println(result);
        return result;
    }
};

#endif // FileNamingHelper_h