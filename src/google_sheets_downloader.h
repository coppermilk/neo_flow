#pragma once

class GoogleSheetsDownloader{
private:
  String deployment_id;
public:
  explicit GoogleSheetsDownloader(const String &deployment_id);
  String get_json();
};
