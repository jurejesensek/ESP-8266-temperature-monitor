#ifndef ESP_8266_TEMPERATURE_MONITOR_TEMPERATURE_MESSAGE_H
#define ESP_8266_TEMPERATURE_MONITOR_TEMPERATURE_MESSAGE_H

#include <cstdint>
#include "constants.h"

namespace TempMonitor
{

class Temperature_msg
{

public:

    char id[MAC_ADDRESS_STRING_LEN];
    char temp[TEMPERATURE_STR_LEN];

    Temperature_msg() = default;

    Temperature_msg(char const * const msg_id, char const * const msg_temp);

    bool getMessage(char * buffer, const uint8_t buffer_len) const;

}; // struct

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_TEMPERATURE_MESSAGE_H
