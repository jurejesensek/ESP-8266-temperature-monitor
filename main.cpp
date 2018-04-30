/* Simple example for I2C / BMP180 / Timer & Event Handling
 *
 * This sample code is in the public domain.
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "i2c/i2c.h"

#include "ota-tftp.h"
#include "rboot-api.h"

#include "RF24/nRF24L01.h"
#include "RF24/RF24.h"


#define PCF_ADDRESS 0x38
#define gpio 2


#define NCS	15
#define CS_NRF	0

#define BUS	0

RF24 radio(2, 0);

//#define readBytes(slave_addr, t data, len, buf) i2c_slave_read(slave_addr, t data, buf, len)
// readBytes(MPU9250_ADDRESS, MPU9250_ACCEL_XOUT_H, 6, &rawData[0]);        // Read the six raw data registers into data array

void delay(int del)
{
	   vTaskDelay(del / portTICK_PERIOD_MS); // sleep 100ms  // Delay a while to let the device stabilize
}


void readBytes(uint8_t slave_addr, uint8_t cmd, uint8_t len, uint8_t *data)
{
	i2c_slave_read(BUS, slave_addr, &cmd, data, len);
}

uint8_t readByte(uint8_t slave_addr, uint8_t cmd)
{
	uint8_t data;

	readBytes(slave_addr, cmd, 1, &data);
	return(data);

}



bool byte_read(uint8_t slave_addr, uint8_t *buf, uint32_t len)
{
    bool success = false;
    do {
//        i2c_start(BUS);
//        if (!i2c_write(BUS, slave_addr << 1)) {
//            break;
//        }
//        i2c_write(BUS, data);
//        i2c_stop(BUS);
        i2c_start(BUS);
        if (!i2c_write(BUS, slave_addr << 1 | 1)) { // Slave address + read
            break;
        }
        while(len) {
            *buf = i2c_read(BUS, len == 1);
            buf++;
            len--;
        }
        success = true;
    } while(0);
    i2c_stop(BUS);
    if (!success) {
        printf("I2C: write error\n");
    }
    return success;
}




#if 0
void writeByte(uint8_t slave_addr, uint8_t cmd, uint8_t data)
{
	uint8_t ddata[2];

	ddata[0] = cmd;
	ddata[1] = data;

	i2c_slave_write(BUS, slave_addr, NULL, ddata, 2);
//	delay(1);
}
#endif


void writeByte(uint8_t slave_addr, uint8_t data)
{
	uint8_t ddata[2];

	ddata[0] = data;

	i2c_slave_write(BUS, slave_addr, NULL, ddata, 1);
//	delay(1);
}

void disp_error(uint8_t err_code)
{
	uint8_t scl = 14, sda = 12;
	gpio_write(NCS, 1);
	gpio_write(CS_NRF, 1);
	i2c_init(BUS, scl, sda, I2C_FREQ_400K);

	while (1) {
		writeByte(PCF_ADDRESS, ~err_code);
		gpio_write(gpio, 0);
		vTaskDelay(500 / portTICK_PERIOD_MS);
		writeByte(PCF_ADDRESS, 0xFF);
		gpio_write(gpio, 1);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

void test_i2c()
{
uint8_t i;
uint8_t scl = 14, sda = 12;

	i = 0x76; // BMP280
	i2c_start(BUS);
	if (!i2c_write(BUS, (i << 1) & 0xfe)) {
		disp_error(0x03);
	} else {
		printf("\nADDR: %x", i);
		printf("\n");
	}
	i2c_stop(BUS);

	i = 0x68; // MPU-9250
	i2c_start(BUS);
	if (!i2c_write(BUS, (i << 1) & 0xfe)) {
		disp_error(0x05);
	} else {
		printf("\nADDR: %x", i);
		printf("\n");
	}
	i2c_stop(BUS);

	radio.begin();
	radio.setChannel(0x02a);
	if (radio.getChannel() != 0x02a) disp_error(0x0a);
	radio.setChannel(0x066);
	if (radio.getChannel() != 0x066) disp_error(0x0a);

	gpio_write(NCS, 1);
	gpio_write(CS_NRF, 1);
	i2c_init(BUS, scl, sda, I2C_FREQ_400K);
}



// Check for communiction events
void PCF_task(void *pvParameters)
{
uint8_t scl = 14, sda = 12;
uint8_t buff[20];


	printf("Init \n");
	vTaskDelay(500 / portTICK_PERIOD_MS); // sleep 100ms
	gpio_enable(NCS, GPIO_OUTPUT);
	gpio_write(NCS, 1);
	gpio_enable(CS_NRF, GPIO_OUTPUT);
	gpio_write(CS_NRF, 1);
//	GPIO.OUT_SET= BIT(scl);
	i2c_init(BUS, scl, sda, I2C_FREQ_400K);

//	setUp();



uint8_t cnt = 0;
uint8_t lled = 0xfe;
	for (int i=0; i<4; i++) {
		writeByte(PCF_ADDRESS, lled);
		gpio_write(gpio, 0);
		vTaskDelay(500 / portTICK_PERIOD_MS);
		writeByte(PCF_ADDRESS, 0xFF);
		gpio_write(gpio, 1);
		vTaskDelay(500 / portTICK_PERIOD_MS);
		lled <<= 1; lled |= 0x01;
	}


	test_i2c();

	lled = 0xff;
        printf("Heap: %i\n", (int) xPortGetFreeHeapSize());

	while (1) {
		vTaskDelay(10 / portTICK_PERIOD_MS); // sleep 100ms
//		writeByte(0x38, cnt++);


		byte_read(PCF_ADDRESS, &cnt, 1);
//		printf("read %x \n", cnt);
		if ((cnt & 0x40) == 0) lled &= 0xfe ; else lled |= 0x01;
		if ((cnt & 0x80) == 0) lled &= 0xfd ; else lled |= 0x02;
		if ((cnt & 0x10) == 0) lled &= 0xfb ; else lled |= 0x04;
		if ((cnt & 0x20) == 0) lled &= 0xf7 ; else lled |= 0x08;
		writeByte(PCF_ADDRESS, lled);



#if 1
		int i;
		printf("\nloop \n");
		for (i=0; i<127; i++) {
			i2c_start(BUS);
			if (!i2c_write(BUS, (i << 1) & 0xfe)) {
				printf("0");
			} else {
				printf("\nADDR: %x", i);
				printf("\n");
			}
			i2c_stop(BUS);
			if ((i%16) == 15) printf("\n");
			vTaskDelay(1 / portTICK_PERIOD_MS); // sleep 100ms
		}

		readBytes(0x68, 0x75, 1, &buff[0]);
		printf("\nWho am I: %x \n", buff[0]);

		delay(100);
#endif



	}




}

// Setup HW
void user_setup(void)
{
    // Set UART Parameter
    uart_set_baud(0, 115200);

    // Give the UART some time to settle
    sdk_os_delay_us(500);
}

extern "C" void user_init(void); // one way
void user_init(void)
{
	// Setup HW
    user_setup();


    sdk_wifi_set_opmode(NULL_MODE);	// STATION_MODE



    GPIO.ENABLE_OUT_SET = BIT(gpio);
    IOMUX_GPIO2 = IOMUX_GPIO2_FUNC_GPIO | IOMUX_PIN_OUTPUT_ENABLE;
    GPIO.OUT_SET= BIT(gpio);

    printf("Starting TFTP server...");
    ota_tftp_init_server(TFTP_PORT);

    printf("Heap: %i\n", (int) xPortGetFreeHeapSize());

    // Create user interface task
    xTaskCreate(PCF_task, "PCF_task", 512, 0, 2, NULL);

}
