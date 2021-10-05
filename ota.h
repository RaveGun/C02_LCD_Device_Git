#include "config.h"
#include "functions.h"

#if defined(ENABLE_OTA)

  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>

  extern const char* ssid;
  extern const char* password;

  extern void setup_OTA(void);

#endif
