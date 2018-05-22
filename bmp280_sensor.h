#ifndef ESP_8266_TEMPERATURE_MONITOR_BMP280_H
#define ESP_8266_TEMPERATURE_MONITOR_BMP280_H

#include "bmp280/bmp280.h"

namespace TempMonitor
{

/**
 * BMP280 temperature sensor.
 */
class Bmp280_temp_sensor
{

public:

    /**
     * Constructor.
     */
    Bmp280_temp_sensor();

    /**
     * Initialises the sensor.
     * @return <code>true</code> if the initialisation was successful, else it returns <code>false</code>.
     */
    bool init();

    /**
     * Reads the temperature value from the sensor and returns it.
     * @return temperature as measured by the sensor, or <code>NaN</code> it the sensor is not properly initialised
     * (by calling <code>init()</code>, or there was someting wrong with the read.
     */
    float read();

private:

    /** Is the sensor initialised. */
    bool valid;

    bmp280_t bmp280_dev;

}; // class

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_BMP280_H
