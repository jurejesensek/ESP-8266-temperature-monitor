#ifndef ESP_8266_TEMPERATURE_MONITOR_ROLE_H
#define ESP_8266_TEMPERATURE_MONITOR_ROLE_H

namespace TempMonitor
{

/**
 * In what role is the device currently in.
 */
enum class Device_role
{
    /**
     * Reads temperature and sends it via NRF.
     */
    NRF_CLIENT,

    /**
     * Reads temperature, retrieves it from other devices (via NRF) and forwards all over MQTT.
     */
    NRF_SERVER_AND_MQTT_CLIENT

}; // enum

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_ROLE_H
