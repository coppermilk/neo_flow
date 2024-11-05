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
#include <Calendar.h>
#include <GlobalVariable.h>

const char *ssid = "🦎 Lizard";    // type your wifi name
const char *password = "93353793"; // Type your wifi password

#define PIN_MATRIX 25
#define MATRIX_W 32
#define MATRIX_H 8

static WiFiUDP ntpUDP;
static NTPClient timeClient(ntpUDP);

enum Align : unsigned
{
  center = 0,
  left,
  right,
};

struct BoardCore
{
  ImageDatabase imageDatabase;
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

} core;

void drawImage(Image &imgMatrix, unsigned long &msPrevious, size_t &frame, const Align a = Align::left);
void getImage(const String &fileName, Image &img);

void drawCentreString(String buf, int x = 0, int y = 8)
{

  core.matrix.setCursor(0, 0);
  int16_t x1, y1;
  uint16_t w, h;

  // calc width of new string
  core.matrix.getTextBounds(buf, x, y, &x1, &y1, &w, &h);
  core.matrix.setCursor((core.matrix.width() - w / 2), y);
  core.matrix.print(buf);
}

void drawLeftrImg(const std::vector<std::vector<uint16_t>> &imgMatrix)
{
  for (size_t x = 0; x < imgMatrix.size(); ++x)
  {
    for (size_t y = 0; y < imgMatrix[x].size(); ++y)
    {
      {
        core.matrix.drawPixel(x, y, imgMatrix[x][y]);
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
      size_t xOffset = ((core.matrix.width() - imgWidh) / 2) + x;
      size_t yOffset = y;

      if (imgMatrix[x][y])
      {
        core.matrix.drawPixel(xOffset, yOffset, imgMatrix[x][y]);
      }
    }
  }
}

void setup()
{

  core.matrix.setFont(&flow);

  Serial.begin(115200);
  Serial.println("Initialization done.");

  Image img;
  getImage("https://raw.githubusercontent.com/coppermilk/neo_flow/main/img/frames/12931_icon_thumb_12931_icon_thumb_12f_100ms.sprite.bmp", img);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  size_t frame = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    drawImage(img, core.msGlobalPrevious, frame, center);
    Serial.print(".");
    core.matrix.show();
  }
  timeClient.begin();
  Serial.print("Time update");
  while (!timeClient.update())
  {
    Serial.print(".");
  }

  Serial.println(timeClient.getFormattedTime());
  Serial.println("WiFi connected");

  core.matrix.fillScreen(0);
}


void drawAccumulateProgressBackGround(int pixel_count, uint16_t color)
{
  for (int row = MATRIX_H - 1; row > 0; --row)
  {
    for (int col = 0; col < MATRIX_W; ++col)
    {

      core.matrix.drawPixel(col, row, color);
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
  core.imageDatabase.createImageMatrix(fileName, img);
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
    xOffset = ((core.matrix.width() - info.w) / 2);
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
    xOffset = core.matrix.width() - info.w + 0;
    yOffset = core.matrix.height() - info.h + 0;
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
          core.matrix.drawPixel(xOffset + x, yOffset + y, color);
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
        core.matrix.drawPixel(xOffset + x, yOffset + y, color);
      }
    }
  }
}

void processDaily(const JsonObject &daily)
{
  // Get the current timestamp
  auto currentTimestamp = timeClient.getEpochTime();

  // Initialize calendar activity object
  Calendar calendarActivity(daily, currentTimestamp, core.matrix.height(), core.matrix.width(), &core.matrix);

  // Check if calendar activity is enabled
  if (!calendarActivity.isEnable())
  {
    return;
  }

  // Draw the initial calendar display
  calendarActivity.drawCalendar();

  // Initialize parameters for animation loop
  size_t frame = 0;
  const unsigned long notificationInterval = 20000; // 20 seconds
  unsigned long startTime = millis();

  // Load the image icon for the calendar activity
  Image iconImage;
  getImage(calendarActivity.getIconUrl(), iconImage);

  // Refresh the calendar display
  // calendarActivity.drawCalendar();

  // Animation loop for the defined interval
  while (millis() < startTime + notificationInterval)
  {
    // Display notification if needed
    if (calendarActivity.isNeedTodayNotification())
    {
      core.matrix.drawPixel(calendarActivity.getXToday(), calendarActivity.getYToday(), rand());
    }

    // Draw the icon image
    drawImage(iconImage, core.msGlobalPrevious, frame, left);

    // Update the display
    core.matrix.show();
  }

  // Clear the display after the interval
  core.matrix.fillScreen(0);
}

void processAccumulateProgress(const JsonObject &accumulateProgress)
{
  // Check if the accumulate progress feature is enabled
  bool isEnabled = accumulateProgress[JSON_ENABLE].as<bool>();
  if (!isEnabled)
  {
    return;
  }

  // Parse JSON data
  const char *activityName = accumulateProgress[JSON_NAME];
  const char *colorHex = accumulateProgress[JSON_COLOR];
  const char *imageUrl = accumulateProgress[JSON_IMG_URL];
  int progressValue = accumulateProgress[JSON_VALUE].as<int>();

  // Convert the color from hex to decimal
  auto colorDecimal = ImageConvector::HexRgbToDec(colorHex);
  Pixel pixelColor(colorDecimal);

  // Draw background based on the accumulate progress value
  drawAccumulateProgressBackGround(progressValue, pixelColor.asUint16_t());

  // Display image if a URL is provided
  if (imageUrl)
  {
    Image iconImage;
    core.imageDatabase.createImageMatrix(imageUrl, iconImage);
    drawCenterImg(iconImage.img);
  }
#if 0
    // Debug output to Serial
    Serial.print("Image URL: ");
    Serial.println(imageUrl);
    Serial.print("Activity Name: ");
    Serial.println(activityName);
    Serial.print("Color (Hex): ");
    Serial.println(colorHex);
    Serial.print("Progress Value: ");
    Serial.println(progressValue);
    Serial.println();
#endif
  // Show the updated matrix display
  core.matrix.show();

  // Display for 10 seconds, then clear the screen
  delay(10000);
  core.matrix.fillScreen(0);
}

void processInfoString(const JsonObject &infoString)
{
  // Check if the info string feature is enabled
  bool isEnabled = infoString[JSON_ENABLE].as<bool>();
  if (!isEnabled)
  {
    return;
  }

  // Parse JSON data
  const char *activityName = infoString[JSON_NAME];
  const char *colorHex = infoString[JSON_COLOR];
  String displayValue = infoString[JSON_VALUE].as<String>();
  const char *imageUrl = infoString[JSON_IMG_URL];

  // Debug output for activity name
  Serial.print("Activity Name: ");
  Serial.println(activityName);

  // Display image and value if image URL is provided
  if (imageUrl)
  {
    Image iconImage;
    core.imageDatabase.createImageMatrix(imageUrl, iconImage);

    // Set up display parameters
    const unsigned long displayInterval = 20000; // 20 seconds
    unsigned long startTime = millis();
    size_t frame = 0;

    // Display the value text
    core.matrix.setCursor(9, 8); // Set cursor position
    core.matrix.print(displayValue);

    // Loop to display the image and text for the defined interval
    while (millis() < startTime + displayInterval)
    {
      // Reload and draw the image in each frame
      getImage(imageUrl, iconImage);
      drawImage(iconImage, core.msGlobalPrevious, frame, left);

      // Show the updated display
      core.matrix.show();
    }

    // Clear the display after interval
    core.matrix.fillScreen(0);
  }
}

void loop()
{
  GoogleSheetsDownloader downloader("AKfycbzARVm6CShUuEXM_Rg3plyliO1jg4tNb4VQ2vznvb7moZZ3FOrOBIsjdkCo_DZA61Zcgw");
  JsonDocument doc = downloader.getDocument();

  JsonArray dailys = doc[JSON_LIST_DAILY];

  for (const JsonObject &daily : dailys)
  {
    processDaily(daily);
  }

  JsonArray accumulateProgresses = doc[JSON_LIST_ACCUMULATE_PROGRESS];
  for (JsonObject accomulateProgress : accumulateProgresses)
  {
    processAccumulateProgress(accomulateProgress);
  }

  JsonArray infoStrings = doc[JSON_LIST_INFO_STRING];
  for (JsonObject infoString : infoStrings)
  {
    processInfoString(infoString);
  }
}