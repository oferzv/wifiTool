/**************************************************************
   wifiTool is a library for the ESP 8266&32/Arduino platform
   SPIFFS oriented AsyncWebServer based wifi configuration tool.
   https://github.com/oferzv/wifiTool
   
   Built by Ofer Zvik (https://github.com/oferzv)
   And Tal Ofer (https://github.com/talofer99)
   Licensed under MIT license
 **************************************************************/

#ifndef WIFITOOL_h
#define WIFITOOL_h

#include <Arduino.h>
#include <DNSServer.h>
#include <vector>

#include <ArduinoOTA.h>
#ifdef ESP32
#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#endif

#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <SimpleJsonParser.h>

#include "definitions.h"

struct knownapsstruct
  {
    char *ssid;
    char *passw;
  };

class WifiTool
{
public:
  WifiTool();
  ~WifiTool();
  void process();
  void begin();

private:
  void setUpSoftAP();
  void setUpSTA();
  unsigned long _restartsystem;
  unsigned long _last_connect_atempt;
  bool  _connecting;
  byte  _last_connected_network;
  SimpleJsonParser  _sjsonp;

  std::unique_ptr<DNSServer> dnsServer;
#if defined(ESP32)
  std::unique_ptr<AsyncWebServer> server;
#else
  std::unique_ptr<AsyncWebServer> server;
#endif

  std::vector<knownapsstruct> vektknownaps;
  File fsUploadFile;
  void updateUpload();
  void handleFileList(AsyncWebServerRequest *request);
  void handleFileDelete(AsyncWebServerRequest *request);
  void getWifiScanJson(AsyncWebServerRequest *request);
  void getNTPJson(AsyncWebServerRequest *request);
  void handleGetSavSecreteJson(AsyncWebServerRequest *request);
  void handleSaveNTPJson(AsyncWebServerRequest *request);
  int getRSSIasQuality(int RSSI);
  void handleUpload(AsyncWebServerRequest *request, String filename, String redirect, size_t index, uint8_t *data, size_t len, bool final);
  void wifiAutoConnect(); 
};

#endif
