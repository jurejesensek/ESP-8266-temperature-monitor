#include <cmath>    // for NAN
#include "bmp280_sensor.h"
#include "constants.h"
#include "i2c/i2c.h"

bool TempMonitor::Bmp280_temp_sensor::init()
{
    initI2C();

    bmp280_params_t params;
    bmp280_init_default_params(&params);
    params.mode = BMP280_MODE_FORCED;
    bmp280_dev.i2c_dev.bus = TempMonitor::I2C_BUS;
    bmp280_dev.i2c_dev.addr = BMP280_I2C_ADDRESS_0;

    return bmp280_init(&bmp280_dev, &params);
}

float TempMonitor::Bmp280_temp_sensor::read()
{
    float temperature, pressure;
    bmp280_force_measurement(&bmp280_dev);

    // wait for measurement to complete
    while (bmp280_is_measuring(&bmp280_dev))
    {};

    if (!bmp280_read_float(&bmp280_dev, &temperature, &pressure, nullptr))
    {
        // read was not successful
        return NAN;
    }
    return temperature;
}

bool TempMonitor::Bmp280_temp_sensor::initI2C()
{
    return i2c_init(TempMonitor::I2C_BUS, TempMonitor::SCL_PIN, TempMonitor::SDA_PIN, I2C_FREQ_100K) == 0;
}
