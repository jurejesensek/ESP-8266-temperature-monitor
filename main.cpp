#include <cstdio>
#include "bmp280/bmp280.h"
#include "espressif/esp_common.h"
#include "esp/uart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "ota-tftp.h"
#include "rboot-api.h"

#include "bmp280_sensor.h"
#include "nrf_comm.h"
#include "constants.h"
#include "secret_constants.h"

#include <espressif/esp_sta.h>
#include <espressif/esp_wifi.h>
#include <semphr.h>
#include <cstring>
#include "wifi_config/ssid_config.h"

extern "C" {
#include "paho_mqtt_c/MQTTESP8266.h"
#include "paho_mqtt_c/MQTTClient.h"
}

TempMonitor::Bmp280_temp_sensor temp_sensor;
TempMonitor::Nrf_comm nrf_comm;

SemaphoreHandle_t wifi_alive;

bool sender = true;

void readTemp(void *pvParameters)
{
    char msg[TempMonitor::NRF_BUFFER_LEN];
    while (true)
	{
	    // todo better error messages
        if (!temp_sensor.initI2C())
        {
            printf("temp sensor init was unsuccessful\n");
        }
        float temp = temp_sensor.read();
        sprintf(msg, "%.03f", temp);
        printf("read temperature: %s\n", msg);
        nrf_comm.init();
        if (!nrf_comm.isValid())
        {
            printf("NRF re-init was not successful\n");
        }
		if (!nrf_comm.send(msg, static_cast<uint8_t>(strlen(msg))))
        {
            printf("NRF sending was not successful\n");
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // sleep
        nrf_comm.disable();
	}
}

void listenNrf(void *pvParameters)
{
	char buffer[TempMonitor::NRF_BUFFER_LEN];
	while (true)
	{
	    if (!nrf_comm.isValid())
        {
            nrf_comm.init();
        }
		if (nrf_comm.receive(buffer, sizeof(buffer)))
		{
			printf("NRF received: %s\n", buffer);
		}
        else
        {
            printf("NRF receive was not successful\n");
	    }
		vTaskDelay(pdMS_TO_TICKS(10000));
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

		while (sdk_wifi_station_get_connect_status() == STATION_GOT_IP)
		{
			xSemaphoreGive(wifi_alive);
			taskYIELD();
		}
		printf("WiFi: disconnected\n");
		sdk_wifi_station_disconnect();
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}

// Setup HW
void user_setup_hw()
{
    // Set UART Parameter
    uart_set_baud(0, TempMonitor::BAUD_RATE);

    // Give the UART some time to settle
    sdk_os_delay_us(500);
}

void other_setup()
{
	nrf_comm.init();
	nrf_comm.disable();
	temp_sensor.init();
}

extern "C" void user_init(void); // one way
void user_init(void)
{
	// Setup HW
	user_setup_hw();

	other_setup();

	vSemaphoreCreateBinary(wifi_alive);

    // Create user interface task

	xTaskCreate(&wifi_task, "wifi_task", 265, nullptr, 2, nullptr);
	//xTaskCreate(&mqtt_task, "mqtt_task", 1024, NULL, 4, NULL);
	if (sender)
	{
		xTaskCreate(readTemp, "readTemp", 512, nullptr, 2, nullptr);
	}
	else
	{
		xTaskCreate(listenNrf, "listenNrf", 512, nullptr, 2, nullptr);
	}

}
