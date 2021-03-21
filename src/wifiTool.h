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

#include "Arduino.h"
#include <DNSServer.h>

#include <ArduinoOTA.h>
#ifdef ESP32
#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <WiFiMulti.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFiMulti.h>
#endif

#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>


#include "definitions.h"

class WifiTool
{
public:
  WifiTool();
  ~WifiTool();
  void process();

private:
  void setUpmDNS();
  void setUpSoftAP();
  void setUpSTAService();

  std::unique_ptr<DNSServer> dnsServer;
#if defined(ESP32)
  std::unique_ptr<AsyncWebServer> server;
#else
  std::unique_ptr<AsyncWebServer> server;
#endif
  File fsUploadFile;
  const byte DNS_PORT = DEF_DNS_PORT;

  String filetoString(const char* path);
  String getJSONValueByKey(String textToSearch, String key);
  void updateUpload();
  void handleFileList(AsyncWebServerRequest *request);
  void handleFileDelete(AsyncWebServerRequest *request);
  void getWifiScanJson(AsyncWebServerRequest *request);
  void handleGetSavSecreteJson(AsyncWebServerRequest *request);
  int getRSSIasQuality(int RSSI);
  void handleUpload(AsyncWebServerRequest *request, String filename, String redirect, size_t index, uint8_t *data, size_t len, bool final);

  //???kell?
  boolean connectAttempt(String ssid, String password);

  /*public:
  WifiTool();
  uint8_t wifiAutoConnect();
  void runApPortal();
  void runWifiPortal();
  
  void begin();
  void begin(uint8_t autoConnectFlag);

private:

  boolean runAP;

  // DNS server
  
  
  void setUpAPService();
  boolean connectAttempt(String ssid, String password);
  
  
  */
};

#endif
