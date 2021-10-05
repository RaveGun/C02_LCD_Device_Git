#include "config.h"
#include "functions.h"
#include "ota.h"

/* *********************************
 *              SETUP
 * ********************************* */
void setup(void) {
  boolean mqtt_connect_result = false;
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

#if defined(ENABLE_OTA)
  setup_OTA();
  delay(200);
  mqtt_connect_result = MQTT_send_config();
  delay(200);
#else
  WiFi.forceSleepBegin();
#endif

  /* Init the log with the CO2 baseline of 400 ppm */
  for(uint16_t i = 0; i < SCREEN_X_SIZE; i++)
  {
    logData[i] = /*i*5*/ INIT_VALUE_CO2;
    logColorData[i] = getColor(logData[i]);
  }

  /* Initialize File System */
  if(SPIFFS.begin())
  {

    /* SPIFF is active serch log file */
    f = SPIFFS.open(filename, "r");
    if (f) {
      /* File found read back the values */
      f.readBytes( (char *)&logData[0], sizeof(logData) );
      f.close();
    }
    else
    {
       /* Maybe next time */
    }
  }

  /* Setup background led light */
  analogWriteFreq(300);
  analogWrite(LED_PIN, 64);

  /* Clear screen and wait for preheating to be done */
  display.begin();
  display.fillScreen(BG_COLOR);

#ifdef LANDSCAPE
  display.setRotation(1);
#else
  display.setRotation(2);
#endif

  display.setTextSize(2);

  /* Preheating shall be done here */
  display.setTextColor(ILI9341_YELLOW);
  display.setCursor(100,110);
  display.printf("Preheating !");

  /*
  if (co2.isPreHeating()) {
    while (co2.isPreHeating()) {
      delay(5000);
    }
  }
  */
  display.fillScreen(BG_COLOR);
  
  /* Draw static scree portion */
  display.setTextSize(1);
  display.setTextColor(ILI9341_LIGHTGREY);

  display.setCursor(10,5);
  display.printf("CO2");
 
  display.setCursor(SCREEN_X_SIZE-30,5);
  display.printf("%cC", 247);

  display.drawLine(0,70, SCREEN_X_SIZE,70, ILI9341_BLUE );

  analogWrite(LED_PIN, 767);
  targetTime = millis() + UPDATE_INTERVAL;
}
