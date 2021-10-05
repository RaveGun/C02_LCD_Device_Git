/*************************************************** 
 ****************************************************/
#include <Arduino.h>
#include <pins_arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <MHZ.h>
#include <FS.h>   //Include File System Headers

#include <ArduinoJson.h>

/* local config and functions */
#include "config.h"
#include "functions.h"
#include "ota.h"

MHZ co2(MH_Z19_RX, MH_Z19_TX, MHZ19B);
Adafruit_ILI9341 display = Adafruit_ILI9341(cs, dc);

WiFiClient wifi_client;
MQTTClient mqtt(256+128);
/*
Adafruit_MQTT_Client mqtt(&wifi_client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);
Adafruit_MQTT_Publish mqtt_co2_level = Adafruit_MQTT_Publish(&mqtt, "cotwo/sensor/cotwo_level/state");
Adafruit_MQTT_Publish mqtt_co2_dicovery = Adafruit_MQTT_Publish(&mqtt, "mqtt/homeassistant/sensor/cotwo/cotwo_level/config");
*/


uint32_t targetTime = 0;                    // for next 1 second timeout
int32_t logData[SCREEN_X_SIZE];
uint16_t logColorData[SCREEN_X_SIZE];
int32_t minCO2 = 1999;
int32_t maxCO2 = 0;

const char* filename = "/backlog.bin";
File f;

/* *********************************
 *                LOOP
 * ********************************* */
void loop()
{
  static int32_t CO2 = 0; 
  static int32_t Temp = 0;
  static int32_t CO2_old = INIT_VALUE_CO2; 
  static int32_t Temp_old = INIT_VALUE_T;
  
  if (targetTime < millis())
  {
    mqtt.loop();
    display.setTextSize(4);

    Temp = co2.getLastTemperature();                     /* Request Temperature (in Celsius) */
    if((Temp_old != Temp) && (Temp > 0))
    {
      Temp_old = Temp;
      display.setTextColor(ILI9341_GREEN, BG_COLOR);
      display.setCursor(SCREEN_X_SIZE - 105, 5);
      display.printf("%3d", Temp_old - 5);
    }

    CO2 = co2.readCO2UART();                             /* Request CO2 (as ppm) */
    if((CO2_old != CO2) && (CO2 > 0))
    {
      CO2_old = CO2;

      display.setTextColor(ILI9341_GREEN, BG_COLOR);
      display.setCursor(30, 5);
      display.printf("%4d", CO2_old);
      MQTT_connect_and_send_c02(CO2_old);
    }

    addData(CO2_old);
    display.setTextSize(2);
    display.setTextColor(ILI9341_CYAN, BG_COLOR);
    display.setCursor(10, 40);
    display.printf("Min %4d", minCO2);
    display.setTextColor(ILI9341_ORANGE, BG_COLOR);
    display.setCursor(SCREEN_X_SIZE - 110, 40);
    display.printf("Max %4d", maxCO2);

    targetTime += UPDATE_INTERVAL;
  }
#ifdef LANDSCAPE
#else
  else
  {
    ArduinoOTA.handle();
  }
#endif
}

/*

TODO: blank screen for a while
TODO: reduce number of writes to once every hour

*/

/*
  "configuration": "                                 xtal=80,vt=flash,exception=disabled,ssl=all,eesz=4M,ip=lm2n,dbg=Disabled,lvl=None____,wipe=all,baud=921600",
  "configuration": "ResetMethod=v1,UploadTool=espota,xtal=80,vt=flash,exception=disabled,ssl=all,eesz=4M,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=115200",  
*/