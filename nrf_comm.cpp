#include "FreeRTOS.h"
#include "esp/uart.h"
#include "nrf_comm.h"
#include "constants.h"

void TempMonitor::Nrf_comm::init()
{
    uart_set_baud(0, BAUD_RATE);
    gpio_enable(SCL_PIN, GPIO_OUTPUT);
    gpio_enable(CS_NRF, GPIO_OUTPUT);

    // radio config
    radio.begin();
    radio.setChannel(CHANNEL);
}

bool TempMonitor::Nrf_comm::send(char * data, uint8_t len)
{
    radio.stopListening();
    radio.openWritingPipe(ADDRESS);
    return radio.write(data, len);
    // todo powerDown()
}

bool TempMonitor::Nrf_comm::receive(char *buffer, uint8_t len)
{
    radio.openReadingPipe(1, ADDRESS);
    radio.startListening();
    if (radio.available())
    {
        radio.read(buffer, len);
        return true;
    }
    printf("receive(): no available data\n");
    return false;
}
