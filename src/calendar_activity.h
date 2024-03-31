#pragma once
#include "Arduino.h"
#include <ArduinoJson.h>
#include <Adafruit_NeoMatrix.h>
#include <vector>
#include <map>
#include <Pixel.h>

class CalendarActivity
{
private:
  using TimeStamp = unsigned long long;
  using Value = int;

public:
  CalendarActivity(const JsonObject &json, TimeStamp time, unsigned rows, unsigned cols, Adafruit_NeoMatrix *pMatrix);

  // Public methods
  void show();
  void update(const JsonObject &json);
  void update(TimeStamp time);
  void drawCalendar();
  void fill_calendarDEBUG();
  Pixel map(int x, int in_min, int in_max, Pixel &out_min, Pixel &out_max);
  String getIcon();
  int getXToday();
  int getYToday();
  bool isNeedTodayNotification();
  bool isNeedValueToday(int weekDay);

private:
  Pixel _minPixel;
  Pixel _maxPixel;

  Value _minVal = 0;
  Value _maxVal = 0;

  String _iconUrl;

  TimeStamp _today;
  String _name;
  std::map<TimeStamp, Value> _timeStampToValue;

  unsigned _rows;
  unsigned _cols;
  const unsigned _DAYS_IN_WEEK = 7;
  Adafruit_NeoMatrix *_matrix;

  // Private methods
  TimeStamp remove_hour_min_sec(TimeStamp time);
  int get_weekday(TimeStamp day);

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
