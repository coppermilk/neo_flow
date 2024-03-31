#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FileDownloader.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <HTTPClient.h>
#include <ImageConvector.h>
#include <ImageDatabase.h>
#include <ImageInfo.h>
#include <SD.h>
#include <SPI.h>
#include <SdCard.h>
#include <WiFiUdp.h>
#include <font.h>
#include <google_sheets_downloader.h>
#include <map>
#include <vector>
#include <NTPClient.h>
#include <calendar_activity.h>
#include <GlobalVariable.h>

const char *ssid = "UPCED7EFB8";       // type your wifi name
const char *password = "tFax8Er3yycw"; // Type your wifi password
#define PIN_MATRIX 25
#define MATRIX_W 32
#define MATRIX_H 8

static WiFiUDP ntpUDP;
static NTPClient timeClient(ntpUDP);

unsigned long msGlobalPrevious = 0;
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MATRIX_W,
                                               MATRIX_H,
                                               PIN_MATRIX,
                                               NEO_MATRIX_TOP +
                                                   NEO_MATRIX_LEFT +
                                                   NEO_MATRIX_COLUMNS +
                                                   NEO_MATRIX_ZIGZAG,
                                               NEO_GRB +
                                                   NEO_KHZ800);

enum Align : unsigned
{
  center = 0,
  left,
  right,
};

void drawImage(Image &imgMatrix, unsigned long &msPrevious, size_t &frame, const Align a = Align::left);
void getImage(const String &fileName, Image &img);

void drawCentreString(String buf, int x = 0, int y = 8)
{

  matrix.setCursor(0, 0);
  int16_t x1, y1;
  uint16_t w, h;

  // calc width of new string
  matrix.getTextBounds(buf, x, y, &x1, &y1, &w, &h);
  matrix.setCursor((matrix.width() - w / 2), y);
  matrix.print(buf);
}

void drawLeftrImg(const std::vector<std::vector<uint16_t>> &imgMatrix)
{
  for (size_t x = 0; x < imgMatrix.size(); ++x)
  {
    for (size_t y = 0; y < imgMatrix[x].size(); ++y)
    {
      {
        matrix.drawPixel(x, y, imgMatrix[x][y]);
      }
    }
  }
}

void drawCenterImg(const std::vector<std::vector<uint16_t>> &imgMatrix)
{
  for (size_t x = 0; x < imgMatrix.size(); ++x)
  {
    for (size_t y = 0; y < imgMatrix[x].size(); ++y)
    {
      size_t imgWidh = imgMatrix[x].size();
      size_t xOffset = ((matrix.width() - imgWidh) / 2) + x;
      size_t yOffset = y;

      if (imgMatrix[x][y])
      {
        matrix.drawPixel(xOffset, yOffset, imgMatrix[x][y]);
      }
    }
  }
}

void setup()
{
  matrix.setFont(&flow);

  Serial.begin(9600);
  Serial.println("Initialization done.");
  Image img;
  getImage("https://raw.githubusercontent.com/coppermilk/neo_flow/main/img/frames/12931_icon_thumb_12931_icon_thumb_12f_100ms.sprite.bmp", img);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  size_t frame = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    drawImage(img, msGlobalPrevious, frame, center);
    Serial.print(".");
  }
  timeClient.begin();
  Serial.print("Time update");
  while (!timeClient.update())
  {
    Serial.print(".");
  }

  Serial.println(timeClient.getFormattedTime());
  Serial.println("WiFi connected");
  matrix.fillScreen(0);
}

unsigned int colorStringToUInt(String colorString)
{
  // Remove any leading '#' character
  colorString.trim();
  if (colorString.startsWith("#"))
  {
    colorString = colorString.substring(1);
  }

  // Convert the hex string to an unsigned int
  unsigned int colorValue = 0;
  for (int i = 0; i < colorString.length(); i++)
  {
    char c = colorString.charAt(i);
    if ('0' <= c && c <= '9')
    {
      colorValue = (colorValue << 4) + (c - '0');
    }
    else if ('a' <= c && c <= 'f')
    {
      colorValue = (colorValue << 4) + (c - 'a' + 10);
    }
    else if ('A' <= c && c <= 'F')
    {
      colorValue = (colorValue << 4) + (c - 'A' + 10);
    }
    else
    {
      // Invalid character
      return 0;
    }
  }

  return colorValue;
}

void drawAccumulateProgressBackGround(int pixel_count, uint16_t color)
{
  for (int row = 7; row > 0; --row)
  {
    for (int col = 0; col < 32; ++col)
    {

      matrix.drawPixel(col, row, color);
      pixel_count--;
      if (pixel_count < 0)
      {
        return;
      }
    }
  }
}

uint16_t ramp(uint16_t pixelValue)
{
  // Извлечение компонентов цвета из пикселя
  uint8_t r = (pixelValue >> 8) & 0xF8;
  uint8_t g = (pixelValue >> 3) & 0xFC;
  uint8_t b = (pixelValue << 3) & 0xF8;

  // Проверка интенсивности каждого канала
  if (r > 40 || g > 40 || b > 40)
  {
    return pixelValue; // Возвращаем исходное значение пикселя
  }
  else
  {
    // Возвращаем черный пиксель
    return 0;
  }
}

void getImage(const String &fileName, Image &img)
{
  ImageDatabase db;
  db.createImageMatrix(fileName, img);
  // auto &info = imgMatrix.info;
}

void drawImage(Image &imgMatrix, unsigned long &msPrevious, size_t &frame, const Align align)
{

#if 0
  Serial.println(imgMatrix.img.size());
  Serial.print("Frame: ");
  Serial.println(info.cntFrames);
  Serial.print("Milliseconds: ");
  Serial.println(info.msFrameDuration);

  Serial.print("H: ");
  Serial.println(info.h);
  Serial.print("W: ");
  Serial.println(info.w);
  Serial.print("isSprite: ");
  Serial.println(info.isSprite);
#endif
  auto &info = imgMatrix.info;
  int xOffset = 0;
  int yOffset = 0;
  switch (align)
  {
  case Align::center:
  {
    xOffset = ((matrix.width() - info.w) / 2);
    yOffset = 0;
    break;
  }
  case Align::left:
  {
    xOffset = 0;
    yOffset = 0;
    break;
  }
  case Align::right:
  {
    xOffset = matrix.width() - info.w + 0;
    yOffset = matrix.height() - info.h + 0;
    break;
  }

    // }
  }
#if 0
  Serial.print("xOffset: ");
  Serial.println(xOffset);
  Serial.print("yOffset ");
  Serial.println(yOffset);
#endif
  if (imgMatrix.info.isSprite)
  {
    const unsigned long msInterval = info.msFrameDuration;

    // while (true)
    //{
    unsigned long msCurrent = millis();

    if (msCurrent - msPrevious >= msInterval)
    {
      msPrevious = msCurrent;
      // Increment frame and wrap around
      frame = (frame + 1) % info.cntFrames;

      for (size_t x = 0; x < info.w; ++x)
      {
        for (size_t y = 0; y < info.h; ++y)
        {
          auto color = imgMatrix.img[x + (frame * info.w)][y];
          color = ramp(color);
          matrix.drawPixel(xOffset + x, yOffset + y, color);
        }
      }
    }
  }
  else
  {
    for (size_t x = 0; x < info.w; ++x)
    {
      for (size_t y = 0; y < info.h; ++y)
      {
        auto color = imgMatrix.img[x][y];
        matrix.drawPixel(xOffset + x, yOffset + y, color);
      }
    }
  }
}

void loop()
{
  GoogleSheetsDownloader downloader("AKfycbwc-E7dJE0IzJ5tjiMTiWWpzX4ZXj1hI_NM5-MG3x1BgaLWtj4F-f2HgqR9ZAqi9lV5lQ");
  String json_str = downloader.get_json();
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json_str);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }
  JsonArray dailys = doc[JSON_LIST_DAILY];
  for (const JsonObject &daily : dailys)
  {

    CalendarActivity calendarActivity(daily, timeClient.getEpochTime(), 8, 32, &matrix);
    calendarActivity.drawCalendar();
    size_t frame = 0;
    unsigned long msInterval = 10 * 1000;
    unsigned long msStart = millis();

    while (millis() < msStart + msInterval)
    {
      calendarActivity.drawCalendar();
      if (!calendarActivity.isHasValueToday())
      {
        matrix.drawPixel(calendarActivity.getXToday(), calendarActivity.getYToday(), rand());
      }

      Image img;
      getImage(calendarActivity.getIcon(), img);
      drawImage(img, msGlobalPrevious, frame, left);

      //Serial.print("Frame: ");
      //Serial.print(frame);

      //Serial.print("Frame: ");
      //Serial.print(frame);

      matrix.show();
    }
    matrix.fillScreen(0);
  }

  return;
  // 22453
  ImageDatabase db;

  String fileName = "https://raw.githubusercontent.com/coppermilk/img/main/img/10813_icon_thumb_10813_icon_thumb_20f_100ms.sprite.bmp";
  // String fileName = "https://raw.githubusercontent.com/coppermilk/neo_flow/main/img/frames/12931_icon_thumb_12931_icon_thumb_12f_100ms.sprite.bmp";
  size_t frame = 0;
  Image img;
  getImage(fileName, img);
  while (true)
  {
    drawImage(img, msGlobalPrevious, frame, left);
  }

#if 0
  const char *imageName = "231_icon_thumb_2f_1000ms.sprite.bmp";
  ImageInfo info = parseFileName(imageName);

  Serial.print("Frame: ");
  Serial.println(info.cntFrames);
  Serial.print("Milliseconds: ");
  Serial.println(info.msFrameDuration);

 
  // auto imgMatrix = db.createImageMatrix("https://raw.githubusercontent.com/coppermilk/img/main/img/4449.bmp");
  GoogleSheetsDownloader downloader("AKfycbzHaM4gtd83bzdSqMTw86lydsu2_ekZub4z-S1eCMpQw8guI1AaVqZOmb-6nLBLPRNNxg");
  String json = downloader.get_json();

  Serial.println(json);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  JsonArray accumulate_progress = doc["accumulate_progress"];
  for (JsonObject obj : accumulate_progress)
  {
    const char *activity_name = obj["activity_name"];
    const char *color = obj["color"];
    const char *img_url = obj["img_url"];
    int value = obj["value"].as<int>(); // Parse value as float
    matrix.fillScreen(0);
    drawAccumulateProgressBackGround(value, convertARGBtoRGB(color));
    if (img_url)
    {

      auto imgMatrix = db.createImageMatrix(img_url);
      drawCenterImg(imgMatrix);
    }
    Serial.print("Img url");
    Serial.println(img_url);
    Serial.print("Activity Name: ");
    Serial.println(activity_name);
    // drawCentreString(activity_name);
    Serial.print("Color: ");
    Serial.println(color);
    Serial.print("Value: ");
    Serial.println(value);
    Serial.println();
    matrix.show();
    delay(10000);
    matrix.fillScreen(0);
  }

  matrix.fillScreen(0);
  JsonArray info_string = doc["info_string"];
  for (JsonObject obj : info_string)
  {
    const char *activity_name = obj["activity_name"];
    const char *color = obj["color"];
    String value = obj["value"].as<String>(); // Parse value as float
    const char *img_url = obj["img_url"];

    Serial.print("Activity Name: ");
    Serial.println(activity_name);

    if (img_url)
    {
      auto imgMatrix = db.createImageMatrix(img_url);
      drawLeftrImg(imgMatrix);
    }
    // drawCentreString(String(value));
    matrix.setCursor(12, 8);
    matrix.print(value);
    Serial.print("Color: ");
    Serial.println(color);
    Serial.print("Value: ");
    Serial.println(value);
    Serial.println();
    matrix.show();
    delay(10000);
    matrix.fillScreen(0);
  }
#endif
}
