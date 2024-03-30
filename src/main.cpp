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

const char *ssid = "UPCED7EFB8";       // type your wifi name
const char *password = "tFax8Er3yycw"; // Type your wifi password
#define PIN_MATRIX 25
#define MATRIX_W 32
#define MATRIX_H 8

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
  Serial.println("WiFi connected");
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

void drawImage(Image &imgMatrix, unsigned long &msPrevious, size_t &frame, const Align a)
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
          size_t xOffset = 0;
          size_t yOffset = 0;
          switch (a)
          {
          case Align::center:
          {
            xOffset = ((matrix.width() - info.w) / 2) + x;
            yOffset = y;
            break;
          }
          case Align::left:
          {
            xOffset = x;
            yOffset = y;
            break;
          }
          case Align::right:
          {
            xOffset = matrix.width() - info.w + x;
            yOffset = matrix.height() - info.h + y;
            break;
          }
          }
          matrix.drawPixel(xOffset, yOffset, color);
        }
      }
      matrix.show();
    }
    // }
  }
  else
  {
    for (size_t x = 0; x < info.w; ++x)
    {
      for (size_t y = 0; y < info.h; ++y)
      {
        auto color = imgMatrix.img[x][y];
        matrix.drawPixel(x, y, color);
      }
    }
    matrix.show();
  }
}

void loop()
{
  // 22453
  ImageDatabase db;

  String fileName = "https://raw.githubusercontent.com/coppermilk/img/main/img/10813_icon_thumb_10813_icon_thumb_20f_100ms.sprite.bmp";
  // String fileName = "https://raw.githubusercontent.com/coppermilk/neo_flow/main/img/frames/12931_icon_thumb_12931_icon_thumb_12f_100ms.sprite.bmp";
  size_t frame = 0;
  Image img;
  getImage(fileName, img);
  while (true)
  {
    drawImage(img, msGlobalPrevious, frame);
  }

#if 0
  const char *imageName = "231_icon_thumb_2f_1000ms.sprite.bmp";
  ImageInfo info = parseFileName(imageName);

  Serial.print("Frame: ");
  Serial.println(info.cntFrames);
  Serial.print("Milliseconds: ");
  Serial.println(info.msFrameDuration);

 
  // auto imgMatrix = db.createImageMatrix("https://raw.githubusercontent.com/coppermilk/img/main/img/4449.bmp");
  GoogleSheetsDownloader downloader("AKfycbzBdut8M7Xa4N57l_t1mgDhUHPCQLc0IqzwdJX_9bK_0Ve2xn2xQ3vMptxW8bE1Wr7KOw");
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
