#include <utility>
#include "calendar_activity.h"
#include "ImageConvector.h"
#include "GlobalVariable.h"
#include "Pixel.h"

Calendar::TimeStamp
Calendar::removeHourMinSec(TimeStamp time)
{
  return (time / 86400L) * 86400L;
}

void Calendar::drawCalendar()
{
  const int OFFSET_DAYS = 6 - getWeekday(_today);
  const TimeStamp last_comming_day = _today + (86400ULL * OFFSET_DAYS);

  for (unsigned y = 0; y < _DAYS_IN_WEEK; ++y)
  {
    for (unsigned x = 8; x < _cols; ++x)
    {
      int index = _DAYS_IN_WEEK * (_cols - x) - y;

      TimeStamp dateKey = last_comming_day - (86400ULL * index);

      Value values = _timeStampToValue[dateKey];

      if (values)
      {

        Pixel pixel = map(values, _minVal, _maxVal, _minPixel, _maxPixel);
        _matrix->drawPixel(x, y, pixel.asUint16_t());
      }
      else if (isNeedValueToday(getWeekday(dateKey)))
      {
        if (_today != dateKey)
        {
          _matrix->drawPixel(x, y, 0x10A2);
        }
        else
        {
          _xToday = x;
          _yToday = y;
          _isNeedTodayNotification = isNeedValueToday(getWeekday(dateKey));
          //_matrix->drawPixel(x, y, rand());
        }
      }
    }
  }
}

void Calendar::fill_calendarDEBUG()
{
  // fill future days offset.
  const TimeStamp offset = 6 - getWeekday(_today);

  for (int i = 0; i < (_cols * _rows - offset); ++i)
  {
    _timeStampToValue[_today - (i * 86400L)] = i + 1;
  }
}

int Calendar::getWeekday(TimeStamp timeStamp)
{
  // 1 Jan 1970 was a Thursday, so add 4 so Sunday is day 0, and mod 7
  TimeStamp days = timeStamp / 86400ULL;
  return (days + 4) % 7; // 0 is Sunday
}

Calendar::Calendar(const JsonObject &json, TimeStamp time, unsigned rows, unsigned cols, Adafruit_NeoMatrix *pMatrix)
{
  _matrix = pMatrix;
  _rows = rows;
  _cols = cols;
  update(json);
  update(time);
}

void Calendar::update(TimeStamp time)
{
  _today = removeHourMinSec(time);
}

void Calendar::update(const JsonObject &json)
{
  extractValues(json);
  extractName(json);
  extractPixelColor(json);
  extractIconUrl(json);
  extractWeekdays(json);
}

void Calendar::extractValues(const JsonObject &json)
{
  _timeStampToValue.clear();
  _minVal = 0;
  _maxVal = 0;
  JsonArray dataPoints = json[JSON_DATA_POINTS];
  for (const JsonObject &dataPoint : dataPoints)
  {
    TimeStamp timeStamp = dataPoint[JSON_TIMESTAMP].as<TimeStamp>();
    Value value = dataPoint[JSON_VALUE].as<Value>();

    _timeStampToValue[removeHourMinSec(timeStamp)] = value;

    _minVal = min(value, _minVal);
    _maxVal = max(value, _maxVal);
  }
}

void Calendar::extractName(const JsonObject &json)
{
  _name = json[JSON_NAME].as<String>();
}

void Calendar::extractPixelColor(const JsonObject &json)
{
  auto minPixel = json[JSON_COLOR_MIN].as<String>();
  auto maxPixel = json[JSON_COLOR_MAX].as<String>();

  _minPixel = Pixel(ImageConvector::HexRgbToDec(minPixel.c_str()));
  _maxPixel = Pixel(ImageConvector::HexRgbToDec(maxPixel.c_str()));
}

void Calendar::extractIconUrl(const JsonObject &json)
{
  _iconUrl = json[JSON_IMG_URL].as<String>();
}

void Calendar::extractWeekdays(const JsonObject &json)
{
  _mo = json[JSON_WEEKDAYS_MO].as<bool>();
  _tu = json[JSON_WEEKDAYS_TU].as<bool>();
  _we = json[JSON_WEEKDAYS_WE].as<bool>();
  _th = json[JSON_WEEKDAYS_TH].as<bool>();
  _fr = json[JSON_WEEKDAYS_FR].as<bool>();
  _sa = json[JSON_WEEKDAYS_SA].as<bool>();
  _su = json[JSON_WEEKDAYS_SU].as<bool>();
}

int Calendar::map(int x, int inMin, int inMax, int outMin, int outMax)
{
  return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

Pixel Calendar::map(int x, int inMin, int inMax, Pixel &outMin, Pixel &outMax)
{

  // If x is zero, return a black pixel
  if (!x)
  {
    return {0, 0, 0};
  }
  else
  {
    if (x == inMin)
    {
      // Clamp the ratio between 0 and 1
      return outMin;
    }
    else
    {
      // Interpolate between the output colors
      uint8_t r = map(x, inMin, inMax, outMin.r, outMax.r);
      uint8_t g = map(x, inMin, inMax, outMin.g, outMax.g);
      uint8_t b = map(x, inMin, inMax, outMin.b, outMax.b);
      return {r, g, b};
    }
  }
}

String Calendar::getIcon()
{
  return _iconUrl;
}

int Calendar::getXToday()
{
  return _xToday;
}

int Calendar::getYToday()
{
  return _yToday;
}

bool Calendar::isNeedTodayNotification()
{
  return _isNeedTodayNotification;
}

bool Calendar::isNeedValueToday(int weekDay)
{
  switch (weekDay)
  {
  case 0:
    return _su;
  case 1:
    return _mo;
  case 2:
    return _tu;
  case 3:
    return _we;
  case 4:
    return _th;
  case 5:
    return _fr;
  case 6:
    return _sa;
  default:
    return false;
  }
}
