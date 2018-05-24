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
    Bmp280_temp_sensor() = default;

    /**
     * Initialises the sensor.
     * Calling <code>initI2C()</code> is not required.
     * @return <code>true</code> if the initialisation was successful, else it returns <code>false</code>.
     */
    bool init();

    /**
     * Initialises I2C.
     * Useful when using multiple devices on I2C.
     * @return <code>true</code> if the initialisation was successful, else it returns <code>false</code>.
     */
    bool initI2C();

    /**
     * Reads the temperature value from the sensor and returns it.
     * @return temperature as measured by the sensor, or <code>NaN</code> it the sensor is not properly initialised
     * (by calling <code>init()</code>, or there was someting wrong with the read.
     */
    float read();

private:

    bmp280_t bmp280_dev;

}; // class

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_BMP280_H
