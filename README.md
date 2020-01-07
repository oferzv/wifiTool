# wifiTool
ESP8266 &amp; ESP32 wifi tool

SPIFFS orinted AsyncWebServer based wifi configuration tool.

This library was created to allow you with one include to have:
1. auto wifi connect, with up to 3 svaed router/password with easy wifi configuration tool.
2. AP access when no wifi is connected with captive - for easy access.
3. SPIFSS mangment tool.
4. OTA over http.

This library uses the following depndencies you need to install: 

https://github.com/me-no-dev/ESPAsyncWebServer

Only For ESP32 
https://github.com/me-no-dev/AsyncTCP

Only For ESP8266
https://github.com/me-no-dev/ESPAsyncTCP
https://github.com/datacute/DoubleResetDetector


All the files in the library are served and stored on the SPIFFS.
In order to get the tool running on the ESP you will need to upload the files.
To do so, you can download the following tool, and place it in a "tools" library in your sketchbook folder.

ESP8266
https://github.com/esp8266/arduino-esp8266fs-plugin
Downlaod JAR file from - https://github.com/esp8266/arduino-esp8266fs-plugin/releases/tag/0.5.0
ESP32
https://github.com/me-no-dev/arduino-esp32fs-plugin
Downlaod JAR file from - https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/tag/1.0

The files are stored in the "data" library. and you will need to upload the, with the uplod tool 
in order for the firmeware to work well.  
The router - password are kept in a json file named "secrets.json" so you can set the data before uploading the 
data into the SPIFSS, this way you do not even have to run the wifi manger the first time.
And it contains all the HTML pages as well. 

Once the code is uploaded, if we fail to connect the AP will run on the ESP and you can connect to it
by looking for a router name "config" and connecting to it - on connect a web page will open and 
it will be redirected to the main page of the configration tool

The page has 3 links:

Wifi setup
This page allow you to set up to 3 router/password for the system to check for connection.
Once save is hit, the file gets re-weriten and the ESP will restart and try connecting using the new data.

OTA
Pre compile the firmware on yout computer and use this page to upload and flash a new firmware to the ESP.

SPIFFS Manager
Allow you to browes, download or delete files from the SPIFSS, note not to delete any system files :) 


Code:

In order to use it you will first have to import it 

#include "wifiTool.h"
WifiTool wifiTool;

The simple way to run it is just add this in the setup. 
wifiTool.begin() 
This will run the auto connect system, and if no available router is found it will automaticly run the AP.

Now if you want a bit more control on how things run you can do the following :
wifiTool.begin(false);
This will run the auto connect system, but if no connection is found - its does not run the AP
then we can check the state of the system with : 
wifiTool.wifiAutoConnect() - which will return True if connection is open and false if not.

In the example we use that to run the following command 
wifiTool.runApPortal() - which runs the AP. 
This way we can print debug before. 

Another usefull command in the system is 
wifiTool.runWifiPortal() - which will run the portal on the existsing wifi connection. 
even if you have a web server running in your firmware this will over ride its settings. 


ESP8266 Dbl restart dedection was added so if you hit the reset button twice (fast enough) 
it will force a running of the  AP

