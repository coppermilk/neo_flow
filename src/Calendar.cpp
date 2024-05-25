#include <utility>
#include <Calendar.h>
#include <ImageConvector.h>
#include <GlobalVariable.h>
#include <Pixel.h>

#define DAY 86400ULL

void Calendar::drawCalendar()
{
  const int todayWeekday =  getWeekday(_today);
  const int cntFutureDays = 6 - todayWeekday;
  const TimeStamp deltaFutureDays = DAY * (cntFutureDays+1);
  const TimeStamp lastCommingDay = _today + deltaFutureDays;
  /*for(int x = 31; x >= 9; --x)
  {
    for(int y = 7; y >= 0; --y)
    {
      31 - x;
    }
 /*}*/

  int dayOffset = 0;
   for (unsigned x = _cols; x--; ) {
        for (unsigned y = _DAYS_IN_WEEK; y--; ) {
   

      TimeStamp dateKey = lastCommingDay  - (DAY * ++dayOffset);
      Value values = _timeStampToValue[dateKey];
   
      if (values)
      {
        Pixel pixel = map(values, _minVal, _maxVal, _minPixel, _maxPixel);
        _matrix->drawPixel(x, y, pixel.asUint16_t());
      }
      else if (isNeedValueInWeekday(getWeekday(dateKey)))
      {
        if (_today == dateKey)
        {
          
          _xToday = x;
          _yToday = y;
          _isNeedTodayNotification = true;
          _matrix->drawPixel(x, y, rand());
        }
        else
        {
    _matrix->drawPixel(x, y, 0x10A2);
        }
      }
     // _matrix->show();
     // delay(4);

           
        }
    }
  
  #if 0

  for (unsigned y = 0; y < _DAYS_IN_WEEK; ++y)
  {
    for (unsigned x = 9; x < _cols; ++x)
    {
      const int remainingColumns = _cols - x;
      const int scaledColumns = 7 * remainingColumns;
      const int cntCurDay = scaledColumns - y;
      TimeStamp deltaCurDay = DAY * cntCurDay;

      TimeStamp dateKey = lastCommingDay - deltaCurDay;
      Value values = _timeStampToValue[dateKey];

      if(x ==31){
      Serial.print(x);
          Serial.print(":");
          Serial.print(y);
          Serial.print("Datakey");
          Serial.println(dateKey);
      }
    
      if (values)
      {
        Pixel pixel = map(values, _minVal, _maxVal, _minPixel, _maxPixel);
        _matrix->drawPixel(x, y, pixel.asUint16_t());

      }
      else if (isNeedValueInWeekday(getWeekday(dateKey)))
      {
        if (_today == dateKey)
        {
          
                _xToday = x;
          _yToday = y;
          _isNeedTodayNotification = true;
          _matrix->drawPixel(x, y, rand());
        }
        else
        {
    _matrix->drawPixel(x, y, 0x10A2);
        }
      }
      _matrix->show();
      delay(4);
    }
  }
  #endif
}

Calendar::TimeStamp Calendar::removeHourMinSec(TimeStamp time)
{
  time = time + 3600;
  return (time / DAY) * DAY;
}

void Calendar::fill_calendarDEBUG()
{
  // fill future days offset.
  const TimeStamp offset = 6 - getWeekday(_today);

  for (int i = 0; i < (_cols * _rows - offset); ++i)
  {
    _timeStampToValue[_today - (i * DAY)] = i + 1;
  }
}

int Calendar::getWeekday(TimeStamp timeStamp)
{
  // 1 Jan 1970 was a Thursday (4 index), so add 4 so Sunday is day 0, and mod 7
  const TimeStamp days = timeStamp / DAY;
  return (days + 4) % 7; // 0 is Sunday
}

Calendar::Calendar(const JsonObject &json,
                   TimeStamp time,
                   unsigned rows,
                   unsigned cols,
                   Adafruit_NeoMatrix *pMatrix)

    : _matrix(pMatrix), _rows(rows), _cols(cols)
{
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
  extractEnable(json);
  extractPixelColor(json);
  extractIconUrl(json);
  extractWeekdays(json);
}

void Calendar::extractValues(const JsonObject &json)
{
  // Reset values
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

void Calendar::extractEnable(const JsonObject & json)
{
  _enable = json[JSON_ENABLE].as<bool>();
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

bool Calendar::isNeedValueInWeekday(int weekDay)
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

bool Calendar::isEnable()
{
  return _enable;
}
