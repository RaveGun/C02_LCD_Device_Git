extern uint16_t getColor(int32_t co2Value);
extern void addData(int32_t co2Value);
extern boolean MQTT_send_config(void);
extern boolean MQTT_connect_and_send_c02(int32_t co2Value);