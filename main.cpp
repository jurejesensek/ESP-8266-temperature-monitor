#include <cstdio>
#include "bmp280/bmp280.h"
#include "espressif/esp_common.h"
#include "esp/uart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "ota-tftp.h"
#include "rboot-api.h"

#include "bmp280_sensor.h"
#include "constants.h"

#include <espressif/esp_sta.h>
#include <espressif/esp_wifi.h>
#include <semphr.h>
#include "wifi_config/ssid_config.h"

#include "nrf24.h"
//#include "mqtt.h"


TempMonitor::Bmp280_temp_sensor temp_sensor;

SemaphoreHandle_t wifi_alive;

void readTemp(void *pvParameters)
{
	while (true)
	{
        float temp = temp_sensor.read();
        printf("temp: %f\n", temp);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // sleep
	}
}

void wifi_task(void *pvParameters)
{
	uint8_t status  = 0;
	uint8_t retries = 30;
	struct sdk_station_config config = {
			WIFI_SSID,
			WIFI_PASS,
	};

	printf("WiFi: connecting to WiFi\n");
	sdk_wifi_set_opmode(STATION_MODE);
	sdk_wifi_station_set_config(&config);

	while (true)
	{
		while ((status = sdk_wifi_station_get_connect_status()) != STATION_GOT_IP
				&& retries != 0)
		{
			printf("wifi status = %d\n", status);
			if (status == STATION_WRONG_PASSWORD)
			{
				printf("WiFi: wrong password\n");
				break;
			}
			else if (status == STATION_NO_AP_FOUND)
			{
				printf("WiFi: AP not found\n");
				break;
			}
			else if (status == STATION_CONNECT_FAIL)
			{
				printf("WiFi: connection failed\n");
				break;
			}
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			--retries;
		}
		if (status == STATION_GOT_IP)
		{
			printf("WiFi: Connected\n");
			xSemaphoreGive(wifi_alive);
			taskYIELD();
		}

		while ((status = sdk_wifi_station_get_connect_status()) == STATION_GOT_IP)
		{
			xSemaphoreGive(wifi_alive);
			taskYIELD();
		}
		printf("WiFi: disconnected\n");
		sdk_wifi_station_disconnect();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
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
	
	//setup_nrf();
	vSemaphoreCreateBinary(wifi_alive);
	//publish_queue = xQueueCreate(3, PUB_MSG_LEN);

    // Create user interface task
	
	xTaskCreate(&wifi_task, "wifi_task", 265, nullptr, 2, nullptr);
	//xTaskCreate(&mqtt_task, "mqtt_task", 1024, NULL, 4, NULL);
    xTaskCreate(readTemp, "readTemp", 265, nullptr, 2, nullptr);

}
