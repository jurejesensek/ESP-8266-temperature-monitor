#ifndef NRF_TRANSMITTER_H
#define NRF_TRANSMITTER_H

#include "nrf24.h"

namespace TempMonitor
{

/**
 * NRF transmitter.
 */
class NRF_transmitter
{

public:

    /**
     * Constructor.
     */
    NRF_transmitter();

    /**
     * Initialises NRF.
     * @return <code>true</code> if the initialisation was successful, else it returns <code>false</code>.
     */
    void init();

    /**
     * Writet to nrf
     * @return bool
     */
    void transmit_nrf24();

private:

    /** Is the sensor initialised. */
    bool valid;


}; // class

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_BMP280_H
