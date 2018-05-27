#ifndef ESP_8266_TEMPERATURE_MONITOR_ROLE_H
#define ESP_8266_TEMPERATURE_MONITOR_ROLE_H

namespace TempMonitor
{

enum Device_role
{
    NRF_CLIENT,
    NRF_SERVER_AND_MQTT_CLIENT
}; // enum

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_ROLE_H
