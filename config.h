#include <Adafruit_ILI9341.h>
#include <MHZ.h>
#include "ESP8266WiFi.h"
//#include <Adafruit_MQTT.h>
//#include <Adafruit_MQTT_Client.h>
#include <MQTT.h>
#include <MQTTClient.h>
#include <FS.h>   //Include File System Headers


#define PORTRAIT
//#define LANDSCAPE

#define ENABLE_OTA

#define F(a) (a)

#define MQTT_SERVER       "mqtt.com"
#define MQTT_SERVERPORT   1337
#define MQTT_USERNAME     "mqttuser"
#define MQTT_KEY          "mqttpass"
#define DISCOVERY_PREFIX  "homeassistant"
#define DICOVERY_CONFIG   "homeassistant/sensor/cotwo/cotwo_level/config"
#define COTWO_STATE_TOPIC "cotwo/sensor/cotwo_level/state"

#if defined(ENABLE_OTA)
    /* for OTA */
    #ifndef STASSID
        #define STASSID "searching"
        #define STAPSK  "theinternet"
    #endif
    #define SENSORNAME "CO2MHZ19B" //change this to whatever you want to call your device
    #define OTAPASSWORD ("OTA_PASSWORD") //the password you will need to enter to upload remotely via the ArduinoIDE
#endif

#define sclk D5 //--- connect this to the display module SCL pin      (Serial Clock)
#define mosi D7 //--- connect this to the display module SDA/MOSI pin (Serial Data)
#define rst  RES //--- connect this to the display module RES pin      (Reset)
#define dc   D4 //--- connect this to the display module DC  pin      (Data or Command)
#define cs   D8 //--- connect this to the display module CS  pin      (Chip Select)

#define MH_Z19_RX D1 //5  RX on ESP8266 - TX on MHZ19B
#define MH_Z19_TX D2 //4  TX on ESP8266 - RX on MHZ19B

#define LED_PIN D6

#define UPDATE_INTERVAL (5005u)              /* The time in mili-seconds to read the sensor */
#define INIT_VALUE_CO2 (666)
#define INIT_VALUE_T (25)

#define BG_COLOR (ILI9341_BLACK)

#ifdef LANDSCAPE
    #define SCREEN_X_SIZE (320u)
    #define SCREEN_Y_SIZE (240u)
    #define SCREEN_HIST_MARK (45u)
#else
    #define SCREEN_X_SIZE (240u)
    #define SCREEN_Y_SIZE (320u)
    #define SCREEN_HIST_MARK (48u)
#endif

extern uint32_t targetTime;                  /* for next read timeout */
extern int32_t logData[SCREEN_X_SIZE];
extern uint16_t logColorData[SCREEN_X_SIZE];
extern int32_t minCO2;
extern int32_t maxCO2;

extern MHZ co2;
extern Adafruit_ILI9341 display;

extern WiFiClient wifi_client;
extern MQTTClient mqtt;
// extern Adafruit_MQTT_Client mqtt;
// extern Adafruit_MQTT_Publish mqtt_co2_level;
// extern Adafruit_MQTT_Publish mqtt_co2_dicovery;

extern const char* filename;
extern File f;