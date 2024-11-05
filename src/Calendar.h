#pragma once
#include "Arduino.h"
#include <ArduinoJson.h>
#include <Adafruit_NeoMatrix.h>
#include <vector>
#include <map>
#include <Pixel.h>

class Calendar
{
private:
  using TimeStamp = unsigned long long;
  using Value = int;

public:
  Calendar(const JsonObject &json, TimeStamp time, unsigned rows, unsigned cols, Adafruit_NeoMatrix *pMatrix);

public:
  void drawCalendar();

  void fill_calendarDEBUG();

  String getIconUrl();

  int getXToday();
  int getYToday();

  bool isNeedTodayNotification();
  bool isNeedValueInWeekday(int weekDay);
  bool isEnable();

private:
  void update(TimeStamp time);
  void update(const JsonObject &json);

  void extractValues(const JsonObject &json);
  void extractName(const JsonObject &json);
  void extractEnable(const JsonObject& json);
  void extractPixelColor(const JsonObject &json);
  void extractIconUrl(const JsonObject &json);
  void extractWeekdays(const JsonObject &json);

  Pixel map(int x, int in_min, int in_max, Pixel &out_min, Pixel &out_max);
  int map(int x, int in_min, int in_max, int out_min, int out_max);

  TimeStamp removeHourMinSec(TimeStamp time);
  int getWeekday(TimeStamp day);

private:
  Pixel _minPixel;
  Pixel _maxPixel;

  Value _minVal = 0;
  Value _maxVal = 0;

  String _iconUrl;

  TimeStamp _today;
  String _name;
  bool _enable = false;
  std::map<TimeStamp, Value> _timeStampToValue;

  unsigned _rows;
  unsigned _cols;
  const unsigned _DAYS_IN_WEEK = 7;
  Adafruit_NeoMatrix *_matrix;

  int _xToday = 0;
  int _yToday = 0;
  bool _isNeedTodayNotification = false;

  bool _mo = false;
  bool _tu = false;
  bool _we = false;
  bool _th = false;
  bool _fr = false;
  bool _sa = false;
  bool _su = false;
};
