#ifndef ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H
#define ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H

namespace TempMonitor
{

static constexpr int BAUD_RATE = 115200;
static constexpr int I2C_BUS = 0;
static constexpr int SCL = 14;
static constexpr int SDA = 12;

// todo change
static constexpr uint8_t NRF_BUFFER_LEN = 15;

static constexpr uint8_t MQTT_ID_LEN = 10;
static constexpr uint8_t MQTT_BUFFER_LEN = 100;

}

#endif //ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H
