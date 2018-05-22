#include "string.h"
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "i2c/i2c.h"
#include "secret_constants.h"
#include "ota-tftp.h"
#include "RF24/nRF24L01.h"
#include "RF24/RF24.h"

#define PCF_ADDRESS	0x38
#define BUS_I2C		0
#define SCL 		14
#define SDA 		12

#define button1		0x20	// 0b ??0? ????
#define led1 		0xfe	// 0b ???? ???0

#define CE_NRF		3
#define CS_NRF		0
#define channel		33

const uint8_t address[] = { 0x01, 0x23, 0x45, 0x67, 0x89 };

static RF24 radio(CE_NRF, CS_NRF);

// write byte to PCF on I2C bus
static inline void write_byte_pcf(uint8_t data) {

	// disable radio
	gpio_write(CS_NRF, 1);
	// reinitialize i2c
	i2c_init(BUS_I2C, SCL, SDA, I2C_FREQ_100K);
	// write data byte
	i2c_slave_write(BUS_I2C, PCF_ADDRESS, NULL, &data, 1);
}

// read byte from PCF on I2C bus
static inline uint8_t read_byte_pcf() {
	uint8_t data;

	// disable radio
	gpio_write(CS_NRF, 1);
	// reinitialize i2c
	i2c_init(BUS_I2C, SCL, SDA, I2C_FREQ_100K);
	// read data byte
	i2c_slave_read(BUS_I2C, PCF_ADDRESS, NULL, &data, 1);

	return data;
}

static inline void setup_nrf() {

	uart_set_baud(0, 115200);
	gpio_enable(SCL, GPIO_OUTPUT);
	gpio_enable(CS_NRF, GPIO_OUTPUT);

	// WiFi configuration
	struct sdk_station_config config = { WIFI_SSID, WIFI_PASS };
	sdk_wifi_station_set_auto_connect(1);
	sdk_wifi_set_opmode(STATION_MODE);
	sdk_wifi_station_set_config(&config);
	sdk_wifi_station_connect();

	// OTA configuration
	ota_tftp_init_server(TFTP_PORT);
}
