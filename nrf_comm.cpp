#include "FreeRTOS.h"
#include "esp/uart.h"
#include "nrf_comm.h"
#include "constants.h"

TempMonitor::Nrf_comm::Nrf_comm()
{
    valid = false;
}

void TempMonitor::Nrf_comm::init()
{
    uart_set_baud(0, BAUD_RATE);
    gpio_enable(SCL, GPIO_OUTPUT);
    gpio_enable(CS_NRF, GPIO_OUTPUT);

    // radio config
    bool v = radio.begin();
    radio.setChannel(CHANNEL);
    valid = v;
}

void TempMonitor::Nrf_comm::disable()
{
    gpio_write(CS_NRF, true);
    valid = false;
}

bool TempMonitor::Nrf_comm::isValid() const
{
    return valid;
}

bool TempMonitor::Nrf_comm::send(char * data, uint8_t len)
{
    if (!valid)
    {
        printf("send(): device not valid\n");
        return false;
    }
    radio.openWritingPipe(ADDRESS);
    radio.stopListening();
    return radio.write(data, len);
    // todo powerDown()
}

bool TempMonitor::Nrf_comm::receive(char *buffer, uint8_t len)
{
    if (!valid)
    {
        printf("receive(): device not valid\n");
        return false;
    }
    radio.openReadingPipe(1, ADDRESS);
    radio.startListening();
    if (radio.available())
    {
        radio.read(buffer, len);
        return true;
    }
    printf("receive(): not successful\n");
    return false;
}
