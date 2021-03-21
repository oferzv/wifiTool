# wifiTool
ESP8266 &amp; ESP32 wifi tool

SPIFFS oriented AsyncWebServer based wifi configuration tool.

This library was created to allow you with one include to have:
1. auto wifi connect, with up to 3 saved router/password with easy wifi configuration tool.
2. AP access when no wifi is connected with captive - for easy access.
3. SPIFFS management tool.
4. OTA over http.


## dependencies
This library uses the following libraries you need to install: 

https://github.com/me-no-dev/ESPAsyncWebServer

Only for ESP32 
- https://github.com/me-no-dev/AsyncTCP

Only for ESP8266
- https://github.com/me-no-dev/ESPAsyncTCP
- https://github.com/datacute/DoubleResetDetector


## SPIFFS.
All the files in the library are served and stored on the SPIFFS.
In order to get the tool running on the ESP you will need to upload the files.
To do so, you can download the following tool, and place it in a "tools" library in your sketchbook folder.

ESP8266

https://github.com/esp8266/arduino-esp8266fs-plugin

Download JAR file from 
https://github.com/esp8266/arduino-esp8266fs-plugin/releases/tag/0.5.0

ESP32

https://github.com/me-no-dev/arduino-esp32fs-plugin

Download JAR file from
https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/tag/1.0

The files are stored in the "data" library. and you will need to upload the, with the uplod tool 
in order for the firmware to work well.  

The router - password are kept in a json file named "secrets.json" so you can set the data before uploading the 
data into the SPIFSS, this way you do not even have to run the wifi manger the first time.
And it contains all the HTML pages as well. 

Once the code is uploaded, if we fail to connect the AP will run on the ESP and you can connect to it
by looking for a router name "config" and connecting to it - on connect a web page will open and 
it will be redirected to the main page of the configuration tool

The page has 3 links:

## Wifi setup
This page allows you to set up to 3 router/password for the system to check for connection.
Once save is hit, the file gets re-written and the ESP will restart and try connecting using the new data.

## OTA
Pre compile the firmware on your computer and use this page to upload and flash a new firmware to the ESP.

## SPIFFS Manager
Allow you to browse, download or delete files from the SPIFSS, note not to delete any system files :) 


## Code:

In order to use it you will first have to import it 
```cpp
#include <wifiTool.h>
WifiTool wifiTool;
```

The simple way to run it is just adding this in the setup. 
```cpp
wifiTool.begin();
```
This will run the auto connect system, and if no available router is found it will automatically run the AP.

Now if you want a bit more control on how things run you can do the following:
```cpp
wifiTool.begin(false);
```
This will run the auto connect system, but if no connection is found - it’s does not run the AP


this way then we can check the state of the system with: 

```cpp
if (!wifiTool.wifiAutoConnect())
  {
    Serial.println("fail to connect to wifi!!!!");
    wifiTool.runApPortal();
  }
```
In the example we use this feature to print debug and the run the AP 


Another usefull command in the system is 
```cpp
wifiTool.runWifiPortal() 
```
which will run the portal on the existing wifi connection. 
even if you have a web server running in your firmware this will over ride its settings. 


## ESP8266 Dbl restart deduction 
We added a Dbl restart deduction and it will force a running of the AP.

For this feature to work, we use this great library
https://github.com/datacute/DoubleResetDetector


## How to install tutorial 
https://www.youtube.com/watch?v=8XnBupiNGEI

