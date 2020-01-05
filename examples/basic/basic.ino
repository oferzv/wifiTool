#include "wifiTool.h"
WifiTool wifiTool;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  Serial.println("System started");

  wifiTool.begin(false);
  if (!wifiTool.wifiAutoConnect())
  {
    Serial.println("fail to connect to wifi!!!!");
    wifiTool.runApPortal();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}