#include "config.h"
#include "functions.h"



/** Local Functions */
/* *********************************
 *              backupData
 * ********************************* */
static void backupData(void)
{
    /* Backup date */
    f = SPIFFS.open(filename, "w");
    if (f) {
      /* File found read back the values */
      uint32_t bytesOut = f.write( (char *)&logData[0], sizeof(logData) );
      if(bytesOut != sizeof(logData))
      {
         /* Error */
      }
      f.close();
    }  
}


/* APIs */
/* *********************************
 *              getColor
 * ********************************* */
uint16_t getColor(int32_t co2Value)
{
  #define LOLIM (400)  // ILI9341_GREEN
  #define HILIM (1500) // ILI9341_YELLOW
  
  uint16_t retColor = 0;

  if (co2Value <= LOLIM)
    retColor = ILI9341_GREEN;
  else if(co2Value >= HILIM)
    retColor = ILI9341_RED;
  else
  {
    uint16_t red = (uint16_t)(((co2Value - LOLIM) * 32) / (HILIM-LOLIM));
    uint16_t green = (uint16_t)((31u - red) * 2);
    retColor = (((co2Value - LOLIM) * 32) / (HILIM-LOLIM));
    retColor = (red << 11) | (green << 5);
  }
  return(retColor);
}

/* *********************************
 *              addData
 * ********************************* */
void addData(int32_t co2Value)
{
  #define RA_SIZE (16u)
  static uint8_t indexRA = 0;
  static int32_t sumRA = 0;
  static int32_t runningAverage[RA_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  int32_t h;

  sumRA -= runningAverage[indexRA];
  runningAverage[indexRA] = co2Value;
  sumRA += runningAverage[indexRA];
  indexRA += 1u;
  indexRA %= RA_SIZE;

  if(0u == indexRA)
  {
    minCO2 = 1999;
    maxCO2 = 0;
    for(uint32_t idx = 1; idx < SCREEN_X_SIZE; idx++)
    {
      int32_t tLogData = logData[idx-1];
      if(minCO2 > tLogData) minCO2 = tLogData;
      if(maxCO2 < tLogData) maxCO2 = tLogData;

      /* shift the logged data */
      logData[idx-1] = logData[idx];
      logColorData[idx-1] = logColorData[idx];

      /* Limit the value to 1600 ppm for the graphic */
      if(co2Value <= 1600)
        h = SCREEN_Y_SIZE - (int32_t)(((int32_t)logData[idx-1] - 300.) / 8.125f);
      else
        h = SCREEN_Y_SIZE - 160;

      /* Draw the lines */
        /* Draw history markers */
      if (0 == (idx-1) % SCREEN_HIST_MARK)
        display.drawLine(idx-1, 70,            idx-1, h, ILI9341_BLUE );  
      else
        display.drawLine(idx-1, 80,            idx-1, h, BG_COLOR);
      display.drawLine(idx-1, SCREEN_Y_SIZE, idx-1, h, logColorData[idx-1]);
    }
    /* add the new data */
    logData[SCREEN_X_SIZE - 1] = sumRA / RA_SIZE;
    logColorData[SCREEN_X_SIZE - 1] = getColor(sumRA / RA_SIZE);

    backupData();
    //Serial.printf("\nLogCO2:%d", sumRA / RA_SIZE);
  }
  /* Check min and max against the actual value after it was checked against the on screen data */
  if(minCO2 > co2Value) minCO2 = co2Value;
  if(maxCO2 < co2Value) maxCO2 = co2Value;
}


/* *********************************
 *              MQTT_send_config
 * ********************************* */
const char* mqtt_dicovery_payload = "{\"unit_of_measurement\":\"ppm\",\"icon\":\"mdi:gauge\",\"name\":\"CO2 Level\",\"state_topic\":\"cotwo/sensor/cotwo_level/state\",\"unique_id\":\"cotwosensor_level\",\"device\":{\"identifiers\":\"60019464c610\",\"name\":\"cotwo\",\"sw_version\":\"01\",\"model\":\"DIY_D1_MINI\",\"manufacturer\":\"dsgmedia\"}}";

boolean MQTT_send_config(void) {
  int8_t ret;

  mqtt.begin(MQTT_SERVER, MQTT_SERVERPORT, wifi_client);

  if (true != mqtt.connected()) {
    //Serial.print("Connecting to MQTT... ");
    uint8_t retries_left = 3;
    while ( retries_left && !mqtt.connected() ) { // connect will return 0 for connected
      //Serial.println(mqtt.connectErrorString(ret));
      //Serial.println("Retrying MQTT connection in 5 seconds...");
      mqtt.disconnect();
      delay(5000);  // wait 5 seconds
      mqtt.connect(SENSORNAME, MQTT_USERNAME, MQTT_KEY);
      retries_left--;
      if(retries_left == 0) {
        return(false);
      }
    }
  } else {
    // Connected
    // Stop if already connected do not connect
    //Serial.println("MQTT Connected!");
  }

  // Send payload if connected and not already returned
  if (! mqtt.publish(DICOVERY_CONFIG, &(mqtt_dicovery_payload[0]), true, 0)) {
    //Serial.println(F("Send Config - Failed"));
    return(false);
  } else {
    //Serial.println(F("OK!"));
    return(true);
  }
}



/* *********************************
 *              MQTT_connect_and_send_c02
 * ********************************* */
boolean MQTT_connect_and_send_c02(int32_t co2Value) {
  int8_t ret;

  if (true != mqtt.connected()) {
    //Serial.print("Connecting to MQTT... ");
    uint8_t retries_left = 3;
    while (retries_left && !mqtt.connected()) { // connect will return 0 for connected
      //Serial.println(mqtt.connectErrorString(ret));
      //Serial.println("Retrying MQTT connection in 5 seconds...");
      mqtt.disconnect();
      delay(5000);  // wait 5 seconds
      mqtt.connect(SENSORNAME, MQTT_USERNAME, MQTT_KEY);
      retries_left--;
      if(retries_left == 0) {
        return(false);
      }
    }
  } else {
    // Connected
    // Stop if already connected do not connect
    //Serial.println("MQTT Connected!");
  }

  // Send payload if connected and not already returned
  if (! mqtt.publish(COTWO_STATE_TOPIC, String(co2Value))) {
    //Serial.println(F("Failed"));
    return(false);
  } else {
    //Serial.println(F("OK!"));
    return(true);
  }
  //Serial.println("MQTT Connected!");
}
