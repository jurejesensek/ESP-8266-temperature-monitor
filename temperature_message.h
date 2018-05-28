#ifndef ESP_8266_TEMPERATURE_MONITOR_TEMPERATURE_MESSAGE_H
#define ESP_8266_TEMPERATURE_MONITOR_TEMPERATURE_MESSAGE_H

#include <cstdint>
#include "constants.h"

namespace TempMonitor
{

/**
 * Stores temperature messages sent over NRF.
 */
class Temperature_msg
{

public:

    /**
     * Device ID.
     */
    char id[MAC_ADDRESS_STRING_LEN];

    /**
     * Temperature.
     */
    char temp[TEMPERATURE_STR_LEN];

    /**
     * Default constructor.
     */
    Temperature_msg() = default;

    /**
     * Constructor.
     * @param msg_id device ID (MAC address).
     * @param msg_temp recorded temperature.
     */
    Temperature_msg(char const * const msg_id, char const * const msg_temp);

    /**
     * Constructs a string from the stored fields.
     * Returns it via supplied buffer.
     * @param buffer buffer to store final string.
     * @param buffer_len buffer length.
     * @return <code>true</code> if the construction was successful (the buffer is of adequate size).
     */
    bool getMessage(char * buffer, const uint8_t buffer_len) const;

}; // struct

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_TEMPERATURE_MESSAGE_H
