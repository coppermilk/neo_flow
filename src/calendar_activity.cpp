#include <utility>
#include "calendar_activity.h"
#include "ImageConvector.h"
#include "GlobalVariable.h"
#include "Pixel.h"

CalendarActivity::TimeStamp
CalendarActivity::remove_hour_min_sec(TimeStamp time)
{
  return (time / 86400L) * 86400L;
}

void CalendarActivity::drawCalendar()
{
  // Serial.println("Strang Foo...");

  const int OFFSET_DAYS = 6 - get_weekday(_today);
  const TimeStamp last_comming_day = _today + (86400ULL * OFFSET_DAYS);
  // Serial.print("LastComming day");
  // Serial.print(last_comming_day);

  // std::vector<std::vector<Pixel>> cur_frame(ROWS, std::vector<Pixel>(COLS));
  // std::pair<int, int> min_max = get_min_max_value();
  for (unsigned y = 0; y < _DAYS_IN_WEEK; ++y)
  {
    for (unsigned x = 0; x < _cols; ++x)
    {
      int index = _DAYS_IN_WEEK * (_cols - x) - y;
      // Serial.print(" Inedx");
      // Serial.print(index);

      // Serial.print(" Key");
      // Serial.print(last_comming_day - (86400ULL * index));
      TimeStamp dateKey = last_comming_day - (86400ULL * index);
      int values = _timeStampToValue[dateKey];
      // Serial.print(" Values");
      // Serial.println(values);
      if (values)
      {

        Pixel pixel = map(values, _minVal, _maxVal, _minPixel, _maxPixel);
        _matrix->drawPixel(x, y, pixel.asUint16_t());
      }
      if (dateKey == _today - 86400ULL)
      {
        _xToday = x;
        _yToday = y;
        _isNeedTodayNotification = !values && isNeedValueToday(get_weekday(_today- 86400ULL));
      }
    }
  }
}

void CalendarActivity::fill_calendarDEBUG()
{
  // fill future days offset.
  const TimeStamp offset = 6 - get_weekday(_today);

  for (int i = 0; i < (_cols * _rows - offset); ++i)
  {
    _timeStampToValue[_today - (i * 86400L)] = i + 1;
    ;
  }

  /*for (const auto &i : _timeStampToValue)
  {
    Serial.print(i.first);
    Serial.print(" ");
    Serial.println(i.second);
  }*/
}

int CalendarActivity::get_weekday(TimeStamp day)
{
  return (((_today / 86400ULL) + 4) % 7); // 0 is Sunday
}

CalendarActivity::CalendarActivity(const JsonObject &json, TimeStamp time, unsigned rows, unsigned cols, Adafruit_NeoMatrix *pMatrix)
{
  _matrix = pMatrix;
  _rows = rows;
  _cols = cols;
  update(json);
  update(time);
}

void CalendarActivity::update(TimeStamp time)
{
  _today = remove_hour_min_sec(time);
}

void CalendarActivity::update(const JsonObject &json)
{
  _timeStampToValue.clear();

  JsonArray dataPoints = json[JSON_DATA_POINTS];
  for (const JsonObject &dataPoint : dataPoints)
  {
    TimeStamp timeStamp = dataPoint[JSON_TIMESTAMP].as<TimeStamp>();
    Value value = dataPoint[JSON_VALUE].as<Value>();

    _timeStampToValue[remove_hour_min_sec(timeStamp)] = value;

    /*Serial.print("Timestamp");
    Serial.print(timeStamp);
    Serial.print(" Value");
    Serial.println(value);*/

    _minVal = min(value, _minVal);
    _maxVal = max(value, _maxVal);
  }

  _name = json[JSON_NAME].as<String>();
  /*
  Serial.print("Activity name: ");
  Serial.println(_name);*/

  auto minPixel = json[JSON_COLOR_MIN].as<String>();
  auto maxPixel = json[JSON_COLOR_MAX].as<String>();

  /*Serial.print("MinPixel: ");
  Serial.println(minPixel);
  Serial.print("MaxPixel: ");
  Serial.println(maxPixel);*/

  _minPixel = Pixel(ImageConvector::HexToDec(minPixel.c_str()));
  _maxPixel = Pixel(ImageConvector::HexToDec(maxPixel.c_str()));

  /*Serial.print("MinPixel: ");
  Serial.println(_minPixel.rgb);
  Serial.print("MaxPixel: ");
  Serial.println(_maxPixel.rgb);*/
  _iconUrl = json[JSON_IMG_URL].as<String>();

  _mo = json[JSON_WEEKDAYS_MO].as<bool>();
  _tu = json[JSON_WEEKDAYS_TU].as<bool>();
  _we = json[JSON_WEEKDAYS_WE].as<bool>();
  _th = json[JSON_WEEKDAYS_TH].as<bool>();
  _fr = json[JSON_WEEKDAYS_FR].as<bool>();
  _sa = json[JSON_WEEKDAYS_SA].as<bool>();
  _su = json[JSON_WEEKDAYS_SU].as<bool>();
  Serial.println(_mo);
  Serial.println(_tu);
  Serial.println(_we);
  Serial.println(_th);
  Serial.println(_fr);
  Serial.println(_sa);
  Serial.println(_su);
}

Pixel CalendarActivity::map(int x, int in_min, int in_max, Pixel &out_min, Pixel &out_max)
{

  // If x is zero, return a black pixel
  if (!x)
  {
    return {0, 0, 0};
  }
  else
  {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    if (x == 1)
    {
      // Clamp the ratio between 0 and 1
      r = out_min.r;
      g = out_min.g;
      b = out_min.b;
    }
    else
    {
      // Interpolate between the output colors
      r = (x - in_min) * (out_max.r - out_min.r) / (in_max - in_min) + out_min.r;
      g = (x - in_min) * (out_max.g - out_min.g) / (in_max - in_min) + out_min.g;
      b = (x - in_min) * (out_max.b - out_min.b) / (in_max - in_min) + out_min.b;
    }
    return {r, g, b};
  }
}

String CalendarActivity::getIcon()
{
  return _iconUrl;
}

int CalendarActivity::getXToday()
{
  return _xToday;
}

int CalendarActivity::getYToday()
{
  return _yToday;
}

bool CalendarActivity::isNeedTodayNotification()
{
  return _isNeedTodayNotification;
}

bool CalendarActivity::isNeedValueToday(int weekDay)
{
  Serial.print("weekday");
  Serial.println(weekDay);
  switch (weekDay)
  {
  case 0: // sunday
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

#if 0
void CalendarActivity::show()
{
  std::vector<std::vector<Pixel>> frame = get_frame();
  for (int i = COLS; i >= 0; --i)
  {
    std::vector<std::vector<Pixel>> scrolled_frame = MathPixelFlow::get_scroll_frame(frame, i, 0);
    pMatrix->fillScreen(0);
    for (unsigned row = 0; row < 8; ++row)
    {
      for (unsigned col = 0; col < COLS; ++col)
      {
        Pixel p = scrolled_frame[row][col];
        pMatrix->drawPixel(col, row, pMatrix->Color(p.r, p.g, p.b));
      }
    }
    pMatrix->show();
    if (i)
    {
      delay(60 / ((i > 0) ? i : -i));
    }
  }

  delay(20000);
  for (int i = 0; i >= -33; --i)
  {
    std::vector<std::vector<Pixel>> scrolled_frame = MathPixelFlow::get_scroll_frame(frame, i, 0);
    pMatrix->fillScreen(0);
    for (unsigned row = 0; row < ROWS; ++row)
    {
      for (unsigned col = 0; col < COLS; ++col)
      {
        Pixel p = scrolled_frame[row][col];
        pMatrix->drawPixel(col, row, pMatrix->Color(p.r, p.g, p.b));
      }
    }
    pMatrix->show();
    if (i)
    {
      delay(60 / ((i > 0) ? i : -i));
    }
  }
}

void CalendarActivity::begin()
{
  show();
  if (!calendar_vith_values[today])
  {
  }
}

void CalendarActivity::stop()
{
}

void CalendarActivity::set_min_pixel(const Pixel &min_pixel)
{
  this->min_pixel = min_pixel;
}

void CalendarActivity::set_max_pixel(const Pixel &max_pixel)
{
  this->max_pixel = max_pixel;
}


std::vector<std::vector<Pixel>> CalendarActivity::get_frame()
{

  // Fill callendar.
  const unsigned long long OFFSET_DAYS = 6 - get_weekday(today);
  const unsigned long long last_comming_day = today + (86400L * OFFSET_DAYS);

  std::vector<std::vector<Pixel>> cur_frame(ROWS, std::vector<Pixel>(COLS));
  std::pair<int, int> min_max = get_min_max_value();
  for (unsigned row = 0; row < DAYS_IN_WEEK; ++row)
  {
    for (unsigned col = 0; col < COLS; ++col)
    {
      int index = DAYS_IN_WEEK * (COLS - col) - row - 1;
      int values = calendar_vith_values[last_comming_day - (86400L * index)];
      if (values)
      {
        Pixel cur_pixel = MathPixelFlow::map(values, min_max.first, min_max.second, min_pixel, max_pixel);
        cur_frame[row][col] = cur_pixel;
      }
    }
  }

  // Override frame.
  std::vector<std::vector<unsigned>> image = pImageDB->get_image_by_name(name_this_activity);
  if (!image.empty())
  {
    for (int i = 0; i < 8; ++i)
    {
      for (int j = 0; j < 8; ++j)
      {
        cur_frame[i][j] = Pixel(image[i][j]);
        Serial.println(image[i][j]);
      }
    }
  }

  return cur_frame;
}

#endif