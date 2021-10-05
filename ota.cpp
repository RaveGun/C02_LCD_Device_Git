#include "config.h"
#include "functions.h"
#include "ota.h"

#if defined(ENABLE_OTA)
    const char* ssid = STASSID;
    const char* password = STAPSK;

    void setup_OTA(void) {

        /* Configure OTA */
        ArduinoOTA.onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH) {
                type = "sketch";
            } else { // U_SPIFFS
                type = "filesystem";
            }

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            //Serial.println("Start updating " + type);
        });
        ArduinoOTA.onEnd([]() {
            //Serial.println("\nEnd");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            progress = progress;
            total = total;
            //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            error = error;
            //Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) {
            //Serial.println("Auth Failed");
            } else if (error == OTA_BEGIN_ERROR) {
            //Serial.println("Begin Failed");
            } else if (error == OTA_CONNECT_ERROR) {
            //Serial.println("Connect Failed");
            } else if (error == OTA_RECEIVE_ERROR) {
            //Serial.println("Receive Failed");
            } else if (error == OTA_END_ERROR) {
            //Serial.println("End Failed");
            }
        });

        ArduinoOTA.setHostname(SENSORNAME);
        ArduinoOTA.begin();  

        WiFi.begin(ssid, password);
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            //Serial.println("Connection Failed! Rebooting...");
            delay(5000);
            ESP.restart();
        } 
        //Serial.println("Ready");
        //Serial.print("IP address: ");
        //Serial.println(WiFi.localIP());
    }
#endif
