#include <i2c/i2c.h>
#include <esp8266.h>
#include "button_reader.h"
#include "constants.h"

void TempMonitor::Button_reader::init()
{
    i2c_init(I2C_BUS, SCL_PIN, SDA_PIN, I2C_FREQ_100K);
    gpio_enable(SCL_PIN, GPIO_OUTPUT);
}

bool TempMonitor::Button_reader::read()
{
    uint8_t reset = 0xFF;
    i2c_slave_write(I2C_BUS, PCF_ADDRESS, nullptr, &reset, 1);
    int ret = i2c_slave_read(TempMonitor::I2C_BUS, TempMonitor::PCF_ADDRESS, nullptr, &data, 1);
    if (ret != 0) {
        data = 0xFF;
        return false;
    }
    return true;
}

bool TempMonitor::Button_reader::button1_pressed() const
{
    return (data & BUTTON1) == 0;
}

bool TempMonitor::Button_reader::button2_pressed() const
{
    return (data & BUTTON2) == 0;
}

bool TempMonitor::Button_reader::button3_pressed() const
{
    return (data & BUTTON3) == 0;
}

bool TempMonitor::Button_reader::button4_pressed() const
{
    return (data & BUTTON4) == 0;
}
