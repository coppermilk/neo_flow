#pragma once
#include <ArduinoJson.h>
class GoogleSheetsDownloader{
private:
  String deployment_id;
public:
  explicit GoogleSheetsDownloader(const String &deployment_id);
  
  JsonDocument getDocument();
  void update();
  String get_json();
  JsonDocument doc;
};
