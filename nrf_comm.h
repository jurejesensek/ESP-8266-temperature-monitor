#ifndef ESP_8266_TEMPERATURE_MONITOR_NRF_COMMUNICATION_H
#define ESP_8266_TEMPERATURE_MONITOR_NRF_COMMUNICATION_H

#include <cstdint>
#include "RF24/nRF24L01.h"
#include "RF24/RF24.h"

namespace TempMonitor
{

/**
 * Class used for NRF communication.
 */
class Nrf_comm
{

public:

    /**
     * Constructor.
     */
    Nrf_comm();

    /**
     * Prepares the module for communication.
     */
    void init();

    /**
     * Disables the module.
     * Useful when using multiple devices on I2C.
     */
    void disable();

    /**
     * Checks if the device is in a valid state.
     * @return <code>true</code> if the device is valid, else returns <code>false</code>.
     */
    bool isValid() const;

    /**
     * Sends data.
     * @param data buffer containing data to be sent.
     * @param len number of characters to be sent.
     * @return <code>true</code> if sending was successful, else returns <code>false</code>.
     */
    bool send(char * data, uint8_t len);

    /**
     * Listens and receives data (if there is data waiting).
     * @param buffer buffer where to store received data.
     * @param len length of buffer.
     * @return <code>true</code> if there were characters to be read and the read succeeded, else returns
     * <code>false</code>.
     */
    bool receive(char *buffer, uint8_t len);

private:

    /**
     * Controlling class.
     */
    RF24 radio{CE_NRF, CS_NRF};

    static constexpr int CE_NRF = 3;

    static constexpr int CS_NRF = 0;

    static constexpr int CHANNEL = 33;

    const uint8_t ADDRESS[5] = {0x01, 0x23, 0x45, 0x67, 0x89};

    bool valid;

}; // class

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_NRF_COMMUNICATION_H
