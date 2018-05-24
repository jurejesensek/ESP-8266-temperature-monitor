#include "nrf24.h"
#include "nrf_transmitter.h"

#define tx_size		6
static const char tx_buffer[tx_size] = "hello";

TempMonitor::NRF_transmitter::NRF_transmitter()
{
	
}

// transmit data
void TempMonitor::NRF_transmitter::transmit_nrf24() {
	printf("transmitting\n");

	// turn on led1
	write_byte_pcf(led1);

	radio.powerUp();
	radio.stopListening();
	radio.write(&tx_buffer, sizeof(tx_buffer));
	radio.powerDown();

	// turn off leds
	vTaskDelay(pdMS_TO_TICKS(300));
	write_byte_pcf(0xff);
}

extern "C" void TempMonitor::NRF_transmitter::init(void) {

	setup_nrf();
	radio.openWritingPipe(address);
	radio.powerDown();
	
}