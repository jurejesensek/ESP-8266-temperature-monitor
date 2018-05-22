#ifndef ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H
#define ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H
#define MQTT_PORT	1883
#define MQTT_TOPIC	"dev/test"

namespace TempSensor
{

constexpr int BAUD_RATE = 115200;
constexpr int I2C_BUS = 0;
constexpr int SCL = 14;
constexpr int SDA = 12;

}

#endif //ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H
