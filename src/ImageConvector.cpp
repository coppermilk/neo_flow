#include "ImageConvector.h"

void ImageConvector::createImageMatrixFromBmpFile(File &bmpImage, Image &img)
{
  if (!bmpImage)
  {
    Serial.print("No file to convert");
    return;
  }
  uint8_t header[54];
  bmpImage.read(header, sizeof(header));

  // Extract width and height information from BMP header
  int16_t width = header[18] + (header[19] << 8);
  int16_t height = header[22] + (header[23] << 8);

  // Serial.print("Image Width: ");
  // Serial.println(width);
  // Serial.print("Image Height: ");
  // Serial.println(height);

  img.img.clear();
  std::vector<std::vector<uint16_t>> imageMatrix(width, std::vector<uint16_t>(height));
  img.img = std::move(imageMatrix);

  img.info.h = height;

  if (!img.info.isSprite)
  {
    img.info.w = width;
  }

  // Jump to pixel data start position
  bmpImage.seek(54); // Assuming there is no additional header information

  int rowSize = width * 3;               // 3 bytes per pixel (RGB)
  int padding = (4 - (rowSize % 4)) % 4; // Calculate padding per row

  // Loop through each pixel
  for (int16_t y = 0; y < height; y++)
  {
    for (int16_t x = 0; x < width; x++)
    {
      // Read RGB values of current pixel
      uint8_t pixelData[3];
      bmpImage.read(pixelData, sizeof(pixelData));

      img.img[x][height - y - 1] = createPixelColor(pixelData[2], pixelData[1], pixelData[0]);
    }
    bmpImage.seek(bmpImage.position() + padding); // Skip padding at the end of row
  }
  bmpImage.close();
}

uint16_t ImageConvector::createPixelColor(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3);
}

uint16_t ImageConvector::createPixelColor(uint32_t argb)
{
  uint8_t r = (argb >> 16) & 0xFF; // Extract red component
  uint8_t g = (argb >> 8) & 0xFF;  // Extract green component
  uint8_t b = argb & 0xFF;         // Extract blue component
  return createPixelColor(r, g, b);
}

std::vector<std::vector<uint16_t>> ImageConvector::rotateImage90(const std::vector<std::vector<uint16_t>> &image)
{
  // Get dimensions of the original image
  int rows = image.size();
  int cols = image[0].size();

  // Create a new vector to store rotated image
  std::vector<std::vector<uint16_t>> rotatedImage(cols, std::vector<uint16_t>(rows));

  // Rotate the image by 90 degrees clockwise
  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      rotatedImage[j][rows - 1 - i] = image[i][j];
    }
  }

  return rotatedImage;
}

unsigned int ImageConvector::HexRgbToDec(const char *hexValue)
{
  if (!hexValue)
  {
    return 0;
  }
  if (strlen(hexValue) == 6)
  {
    return strtoul(hexValue, NULL, 16);
  }
  // Handle error or default case here
  // For simplicity, let's default to black if input is invalid
  return 0;
}

unsigned int ImageConvector::colorStringToUInt(String colorString)
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

uint16_t ImageConvector::colorStringToUint16(String colorString)
{
  return createPixelColor(colorStringToUInt(colorString));
}
