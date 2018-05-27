#ifndef ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H
#define ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H

namespace TempMonitor
{

// I2C
static constexpr int BAUD_RATE = 115200;
static constexpr int I2C_BUS = 0;
static constexpr int SCL_PIN = 14;
static constexpr int SDA_PIN = 12;
static constexpr int PCF_ADDRESS = 0x38;

// MQTT
static constexpr uint8_t MQTT_ID_LEN = 15;
static constexpr uint8_t MQTT_BUFFER_LEN = 100;
static constexpr int MQTT_COMMAND_TIMEOUT_MS = 5000;

static constexpr int MAC_ADDRESS_STRING_LEN = 13;

static constexpr int TEMPERATURE_STR_LEN = 10;
// MAC:TEMP + overhead for \0 etc.
static constexpr int MESSAGE_LEN = MAC_ADDRESS_STRING_LEN + TEMPERATURE_STR_LEN + 1;

static constexpr int QUEUE_SIZE = 5;

}

#endif //ESP_8266_TEMPERATURE_MONITOR_CONSTANTS_H
