# C02_LCD_Device_Git
A small CO2 sensor based on the MHZ19B device with 2.2in display
It implements:
- MQTT with autodiscovery for Home Assistant.
- OTA updates
- stores the latest data available on the display into the SPIFFS.

# Used libraries:
- MQTT by Joël Gähwiler - https://github.com/256dpi/arduino-mqtt
- MHZ by Tobias Schürg - https://github.com/tobiasschuerg/MH-Z-CO2-Sensors
- JSON by Arduino - https://github.com/arduino-libraries/Arduino_JSON
- GFX and ILI9341 from Adafruit

# Board settings for Arduino:
- WEMOS D1 Mini

# Hardware
The board schematic and layout is at EasyEDA:
https://easyeda.com/editor#id=d2530e7f5d1f4b67a9672a93810a599e|94576c1540c140daabbabc23da8f10ec
