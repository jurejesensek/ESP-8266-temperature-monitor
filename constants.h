#ifndef ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H
#define ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H

namespace TempSensor
{

static constexpr int MQTT_PORT	= 1883;
static constexpr char const * MQTT_TOPIC = "dev/test";
static constexpr int BAUD_RATE = 115200;
static constexpr int I2C_BUS = 0;
static constexpr int SCL = 14;
static constexpr int SDA = 12;

}

#endif //ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H
