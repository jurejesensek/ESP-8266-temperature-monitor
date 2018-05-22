#ifndef ESP_8266_TEMPERATURE_MONITOR_WIFI_H
#define ESP_8266_TEMPERATURE_MONITOR_WIFI_H

#include "bmp280/bmp280.h"

namespace TempMonitor
{

/**
 * BMP280 temperature sensor.
 */
class Wifi
{

public:

    /**
     * Constructor.
     */
    Wifi();

    /**
     * Initialises the sensor.
     * @return <code>true</code> if the initialisation was successful, else it returns <code>false</code>.
     */
    bool init();

    /**
     */
    void wifi_task();

private:

    /** Is the sensor initialised. */
    bool valid;

    bmp280_t bmp280_dev;

}; // class

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_BMP280_H
