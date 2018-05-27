#include <cstring>
#include "temperature_message.h"

TempMonitor::Temperature_msg::Temperature_msg(char const *const msg_id, char const *const msg_temp)
{
    strncpy(id, msg_id, MAC_ADDRESS_STRING_LEN);
    strncpy(temp, msg_temp, MAC_ADDRESS_STRING_LEN);
}

bool TempMonitor::Temperature_msg::getMessage(char *buffer, const uint8_t buffer_len) const
{
    if (buffer_len < MAC_ADDRESS_STRING_LEN + TEMPERATURE_STR_LEN + 1)
    {
        return false;
    }
    strncpy(buffer, id, MAC_ADDRESS_STRING_LEN);
    strncat(buffer, ":", 2);
    strncat(buffer, temp, TEMPERATURE_STR_LEN);
    return true;
}
