/**************************************************************
   wifiTool is a library for the ESP 8266&32/Arduino platform
   SPIFFS oriented AsyncWebServer based wifi configuration tool.
   https://github.com/oferzv/wifiTool
   
   Built by Ofer Zvik (https://github.com/oferzv)
   And Tal Ofer (https://github.com/talofer99)
   Licensed under MIT license
 **************************************************************/

#include "Arduino.h"
#include "wifiTool.h"
#include <stdlib.h>
/*
    class CaptiveRequestHandler
*/

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request)
  {
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request)
  {
    String RedirectUrl = "http://";
    if (ON_STA_FILTER(request))
    {
      RedirectUrl += WiFi.localIP().toString();
    }
    else
    {
      RedirectUrl += WiFi.softAPIP().toString();
    }
    RedirectUrl += "/wifi_index.html";
    //Serial.println(RedirectUrl);
    request->redirect(RedirectUrl);
  }
};

void WifiTool::begin()
{
  setUpSoftAP();
  setUpSTA();
}
/*
    WifiTool()
*/
WifiTool::WifiTool(strDateTime &strdt, NTPtime &ntp_):_strdt(strdt),_ntp(ntp_)
{
  _restartsystem          = 0;
  _last_connect_atempt    = 0;
  _last_connected_network = 0;
  _connecting             = false;
  
  WiFi.mode(WIFI_AP_STA);
  
  // start spiff
  if (!SPIFFS.begin())
  {
    // Serious problem
    Serial.println(F("SPIFFS Mount failed."));
    return;
  } //end if
}

WifiTool::~WifiTool()
{
  _apscredit.clear();
}

/*
    process()
*/
void WifiTool::process()
{
  ///DNS
  yield();
  dnsServer->processNextRequest();
  wifiAutoConnect();
  if (_restartsystem)
  {
    if ((unsigned long)millis()-_restartsystem > 60000)
    {
      //Serial.println(F("Ujraindítom."));
      ESP.restart();
    } //end if
  }   //end if
}

void WifiTool::wifiAutoConnect()
{
  if (WiFi.status() != WL_CONNECTED && !_connecting)
  {
    Serial.println(F("\nNo WiFi connection."));
    if(_apscredit[_last_connected_network].first!="")
    {
      WiFi.begin(_apscredit[_last_connected_network].first,
                _apscredit[_last_connected_network].second);          
    }
    _last_connect_atempt = millis();
    _connecting = true;
  }
  else if (WiFi.status() != WL_CONNECTED && _connecting && (millis() - _last_connect_atempt > WAIT_FOR_WIFI_TIME_OUT))
  {
    if (++_last_connected_network >= 3)
      _last_connected_network = 0;
    WiFi.begin(_apscredit[_last_connected_network].first,
                _apscredit[_last_connected_network].second);
    _last_connect_atempt = millis();
  }
  else if (WiFi.status() == WL_CONNECTED && _connecting)
  {
    _connecting = false;
    Serial.println(F("\nWiFi connected."));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
    Serial.print(F("ssid: "));
    Serial.println(WiFi.SSID());
  } 

} //end void


/*
   getRSSIasQuality(int RSSI)
*/
int WifiTool::getRSSIasQuality(int RSSI)
{
  int quality = 0;

  if (RSSI <= -100)
  {
    quality = 0;
  }
  else if (RSSI >= -50)
  {
    quality = 100;
  }
  else
  {
    quality = 2 * (RSSI + 100);
  }
  return quality;
}

/*
   getWifiScanJson()
*/
void WifiTool::getWifiScanJson(AsyncWebServerRequest *request)
{
  String json = "{\"scan_result\":[";
  int n = WiFi.scanComplete();
  if (n == -2)
  {
    WiFi.scanNetworks(true);
  }
  else if (n)
  {
    for (int i = 0; i < n; ++i)
    {
      if (i)
        json += ",";
      json += "{";
      json += "\"RSSI\":" + String(WiFi.RSSI(i));
      json += ",\"SSID\":\"" + WiFi.SSID(i) + "\"";
      json += "}";
    }
    WiFi.scanDelete();
    if (WiFi.scanComplete() == -2)
    {
      WiFi.scanNetworks(true);
    }
  }
  json += "]}";
  request->send(200, "application/json", json);
  json = String();
}

void WifiTool::getNTPJson(AsyncWebServerRequest *request)
{
  String json=_sjsonp.fileToString("/ntp.json");
  request->send(200, "application/json", json);
}
void WifiTool::getThingspeakJson(AsyncWebServerRequest *request)
{
  String json=_sjsonp.fileToString("/thingspeak.json");
  request->send(200, "application/json", json);
}



/*
   handleGetSavSecreteJson()
*/
void WifiTool::handleGetSavSecreteJson(AsyncWebServerRequest *request)
{

  AsyncWebParameter *p;
  String jsonString = "{";
  jsonString.concat("\"APpassw\":\"");
  p = request->getParam("APpass", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\",");

  jsonString.concat("\"ssid0\":\"");
  p = request->getParam("ssid0", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\",");

  jsonString.concat("\"pass0\":\"");
  p = request->getParam("pass0", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\",");

  jsonString.concat("\"ssid1\":\"");
  p = request->getParam("ssid1", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\",");

  jsonString.concat("\"pass1\":\"");
  p = request->getParam("pass1", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\",");

  jsonString.concat("\"ssid2\":\"");
  p = request->getParam("ssid2", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\",");

  jsonString.concat("\"pass2\":\"");
  p = request->getParam("pass2", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\"}");

  File file = SPIFFS.open(SECRETS_PATH, "w");
  if (!file)
  {
    Serial.println(F("Error opening file for writing"));
    return;
  }
  file.print(jsonString);
  file.flush();
  file.close();

  request->send(200, "text/html", "<h1>Restarting .....</h1>");
  _restartsystem = millis();
}

void WifiTool::handleSaveNTPJson(AsyncWebServerRequest *request)
{ 
  
  AsyncWebParameter *p;
  String jsonString = "{";
  jsonString.concat("\"NTPserver\":\"");
  p = request->getParam("NTPserver", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\",");

  jsonString.concat("\"UTCh\":\"");
  p = request->getParam("UTCh", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\",");

  jsonString.concat("\"UTCm\":\"");
  p = request->getParam("UTCm", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\",");

  jsonString.concat("\"extratsh\":\"");
  p = request->getParam("extratsh", true);
  jsonString.concat(p->value());
  jsonString.concat("\"}");

  Serial.println(jsonString);

  File file = SPIFFS.open("/ntp.json", "w");
  if (!file)
  {
    Serial.println(F("Error opening file for writing"));
    return;
  }
  file.print(jsonString);
  file.flush();
  file.close();
  request->redirect("/wifi_NTP.html");
}

void WifiTool::handleSaveThingspeakJson(AsyncWebServerRequest *request)
{ 
  AsyncWebParameter *p;
  String jsonString = "{";
  jsonString.concat("\"ChannelID\":\"");
  p = request->getParam("ChannelID", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\",");

  jsonString.concat("\"WriteAPIKey\":\"");
  p = request->getParam("WriteAPIKey", true);
  jsonString.concat(p->value().c_str());
  jsonString.concat("\"}");

  Serial.println(jsonString);

  File file = SPIFFS.open("/thingspeak.json", "w");
  if (!file)
  {
    Serial.println(F("Error opening file for writing"));
    return;
  }
  file.print(jsonString);
  file.flush();
  file.close();
  request->redirect("/wifi_thingspeak.html");
}

void WifiTool::handleSendTime(AsyncWebServerRequest *request)
{
  AsyncWebParameter *p;
  p = request->getParam("time", true);
  if(_strdt.epochTime==0 && p != nullptr)
  {  
    char atm[11];
    memset(atm,0,11);
    strncpy(atm,p->value().c_str(),10);
    char* ptr;
    unsigned long  b;
    b=strtoul(atm, &ptr, 10);//string to unsigned long
    b=_ntp.adjustTimeZone(b,_ntp.getUtcHour(),_ntp.getUtcMin(),_ntp.getSTDST());
    _strdt=_ntp.ConvertUnixTimestamp(b);
  }
  request->send(200);
}

/**
  * setUpSTA()
  * Setup the Station mode
*/
void WifiTool::setUpSTA()
{
  String json = _sjsonp.fileToString(SECRETS_PATH);
  if (json == "" || json == nullptr)
  {
    Serial.println(F("Can't open the secret file."));
    return;
  }

  for (byte i = 0; i < 3; i++)
  {
    String assid = _sjsonp.getJSONValueByKeyFromString(json, "ssid" + String(i));
    String apass = _sjsonp.getJSONValueByKeyFromString(json, "pass" + String(i));

    _apscredit.push_back(std::make_pair(assid,apass));

  } //end for
}
/**
  * setUpSoftAP()
  * Setting up the SoftAP Service
*/
void WifiTool::setUpSoftAP()
{
  Serial.println(F("RUN AP"));
  dnsServer.reset(new DNSServer());
  
  WiFi.softAPConfig(IPAddress(DEF_AP_IP),
                    IPAddress(DEF_GATEWAY_IP),
                    IPAddress(DEF_SUBNETMASK));
  WiFi.softAP(DEF_AP_NAME,_sjsonp.getJSONValueByKey(SECRETS_PATH,"APpassw"),1,0,4);
  
  
  delay(500);

  // Setup the DNS server redirecting all the domains to the apIP
  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer->start(DEF_DNS_PORT, "*", IPAddress(DEF_DNS_IP));

  Serial.println(F("DNS server started."));

#if defined(ESP32)
  server.reset(new AsyncWebServer(80));
#else
  server.reset(new AsyncWebServer(80));
#endif

  Serial.print(F("SoftAP name and server IP address: "));
  Serial.print(String(DEF_AP_NAME)+"  ");
  Serial.println(WiFi.softAPIP());

  server->serveStatic("/", SPIFFS, "/").setDefaultFile("/wifi_index.html");

  server->on("/saveSecret/", HTTP_ANY, [&, this](AsyncWebServerRequest *request) {
    handleGetSavSecreteJson(request);
  });

  server->on("/saveNTP/", HTTP_ANY, [&, this](AsyncWebServerRequest *request) {
    handleSaveNTPJson(request);
  });

   server->on("/sendTime/", HTTP_POST, [&, this](AsyncWebServerRequest *request) {
    handleSendTime(request);
  });

  server->on("/list", HTTP_ANY, [&, this](AsyncWebServerRequest *request) {
    handleFileList(request);
  });

  // spiff delete
  server->on("/edit", HTTP_DELETE, [&, this](AsyncWebServerRequest *request) {
    handleFileDelete(request);
  });

  // spiff upload
  server->on(
      "/edit", HTTP_POST, [&, this](AsyncWebServerRequest *request) {},
      [&, this](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
                size_t len, bool final) {
        handleUpload(request, filename, "/wifi_spiffs_admin.html", index, data, len, final);
      });

  server->on("/wifiScan.json", HTTP_GET, [&, this](AsyncWebServerRequest *request) {
    getWifiScanJson(request);
  });

  server->on("/ntp.json", HTTP_GET, [&, this](AsyncWebServerRequest *request) {
    getNTPJson(request);
  });

  server->on("/thingspeak.json", HTTP_GET, [&, this](AsyncWebServerRequest *request) {
    getThingspeakJson(request);
  });

  // Simple Firmware Update Form
  server->on("/update", HTTP_GET, [&, this](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/wifi_upload.html");
  });
  server->on(
      "/update", HTTP_POST, [&, this](AsyncWebServerRequest *request) {
        uint8_t isSuccess = !Update.hasError();

        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", isSuccess ? "OK" : "FAIL");
        response->addHeader("Connection", "close");
        request->send(response); },
      [&, this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index)
        {
          Serial.printf("Update Start: %s\n", filename.c_str());

#if defined(ESP8266)
          Update.runAsync(true);
          if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000))
          {
            Update.printError(Serial);
          }
#else
          if (!Update.begin(UPDATE_SIZE_UNKNOWN))
          {
            Update.printError(Serial);
          }
#endif
        }
        if (!Update.hasError())
        {
          if (Update.write(data, len) != len)
          {
            Update.printError(Serial);
          }
        }
        if (final)
        {
          if (Update.end(true))
          {
            Serial.printf("Update Success: %uB\n", index + len);
          }
          else
          {
            Update.printError(Serial);
          }
        }
      });

  server->onNotFound([](AsyncWebServerRequest *request) {
    Serial.println("handle not found.");
    request->send(404);
  });

  server->addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
  Serial.println(F("HTTP webserver started."));
  server->begin();
}

void WifiTool::handleFileList(AsyncWebServerRequest *request)
{
  //Serial.println("handle file list");
  if (!request->hasParam("dir"))
  {
    request->send(500, "text/plain", "BAD ARGS");
    return;
  }

  AsyncWebParameter *p = request->getParam("dir");
  String path = p->value().c_str();
  //Serial.println("handleFileList: " + path);
  String output = "[";
#if defined(ESP8266)

  Dir dir = SPIFFS.openDir(path);
  while (dir.next())
  {
    File entry = dir.openFile("r");
    if (output != "[")
    {
      output += ',';
    }
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

#else

  File root = SPIFFS.open("/", "r");
  if (root.isDirectory())
  {
    Serial.println("here ??");
    File file = root.openNextFile();
    while (file)
    {
      if (output != "[")
      {
        output += ',';
      }
      output += "{\"type\":\"";
      output += (file.isDirectory()) ? "dir" : "file";
      output += "\",\"name\":\"";
      output += String(file.name()).substring(1);
      output += "\"}";
      file = root.openNextFile();
    }
  }
#endif

  path = String();
  output += "]";
  //Serial.println(output);
  request->send(200, "application/json", output);
}

void WifiTool::handleFileDelete(AsyncWebServerRequest *request)
{
  Serial.println("in file delete");
  if (request->params() == 0)
  {
    return request->send(500, "text/plain", "BAD ARGS");
  }
  AsyncWebParameter *p = request->getParam(0);
  String path = p->value();
  Serial.println("handleFileDelete: " + path);
  if (path == "/")
  {
    return request->send(500, "text/plain", "BAD PATH");
  }

  if (!SPIFFS.exists(path))
  {
    return request->send(404, "text/plain", "FileNotFound");
  }

  SPIFFS.remove(path);
  request->send(200, "text/plain", "");
  path = String();
}

//==============================================================
//   handleUpload
//==============================================================
void WifiTool::handleUpload(AsyncWebServerRequest *request, String filename, String redirect, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!index)
  {
    if (!filename.startsWith("/"))
      filename = "/" + filename;
    Serial.println((String) "UploadStart: " + filename);
    fsUploadFile = SPIFFS.open(filename, "w"); // Open the file for writing in SPIFFS (create if it doesn't exist)
  }
  for (size_t i = 0; i < len; i++)
  {
    fsUploadFile.write(data[i]);
    //Serial.write(data[i]);
  }
  if (final)
  {
    Serial.println((String) "UploadEnd: " + filename);
    fsUploadFile.close();
    request->redirect(redirect);
  }
}

