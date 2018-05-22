#include <cstdio>
#include "bmp280/bmp280.h"
#include "espressif/esp_common.h"
#include "esp/uart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "ota-tftp.h"
#include "rboot-api.h"

#include "constants.h"
#include "bmp280_sensor.h"


TempMonitor::Bmp280_temp_sensor temp_sensor;

void readTemp(void *pvParameters)
{
	while (true)
	{
        float temp = temp_sensor.read();
        printf("%f\n", temp);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // sleep
	}
}

// Setup HW
void user_setup_hw()
{
    // Set UART Parameter
    uart_set_baud(0, TempSensor::BAUD_RATE);

    // Give the UART some time to settle
    sdk_os_delay_us(500);
}

void other_setup()
{
    temp_sensor.init();
}

extern "C" void user_init(void); // one way
void user_init(void)
{
	// Setup HW
	user_setup_hw();

	other_setup();

    // Create user interface task
    xTaskCreate(readTemp, "readTemp", 512, nullptr, 2, nullptr);

}
