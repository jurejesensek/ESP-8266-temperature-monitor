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
#include "mqtt_client.h"
#include "temperature_message.h"
#include "device_role.h"
#include "button_reader.h"

TempMonitor::Bmp280_temp_sensor temp_sensor;
TempMonitor::Nrf_comm nrf_comm;
TempMonitor::Mqtt_client mqtt_client;
TempMonitor::Button_reader button_reader;

TaskHandle_t wifi_task_handle;
TaskHandle_t listen_nrf_task_handle;
TaskHandle_t send_nrf_task_handle;
TaskHandle_t read_temp_task_handle;


SemaphoreHandle_t wifi_alive;
SemaphoreHandle_t i2c_mutex;
QueueHandle_t publish_queue;

TempMonitor::Device_role device_role;

/**
 * Use MAC address for unique ID
 * @return char* to string containing MAC address.
 */
char const * const get_my_id()
{
	static char my_id[TempMonitor::MAC_ADDRESS_STRING_LEN];
	static bool my_id_done = false;
	if (my_id_done)
	{
		return my_id;
	}
	if (!sdk_wifi_get_macaddr(STATION_IF, reinterpret_cast<uint8_t *>(my_id)))
	{
		printf("Could not obtain MAC address\n");
		return nullptr;
	}
	for (int8_t i = 5; i >= 0; --i)
	{
		uint8_t x = static_cast<uint8_t>(my_id[i] & 0x0F);
		if (x > 9) x += 7;
		my_id[i * 2 + 1] = x + '0';
		x = static_cast<uint8_t>(my_id[i] >> 4);
		if (x > 9) x += 7;
		my_id[i * 2] = x + '0';
	}
	my_id[TempMonitor::MAC_ADDRESS_STRING_LEN - 1] = '\0';
	my_id_done = true;
	return my_id;
}

void read_temp_task(void *pvParameters)
{
    char msg[TempMonitor::TEMPERATURE_STR_LEN];
    while (true)
	{
		xSemaphoreTake(i2c_mutex, portMAX_DELAY);

		do
		{
			// todo better error messages
			if (!temp_sensor.initI2C())
			{
				printf("temp sensor init was unsuccessful\n");
				break;
			}
			float temp = temp_sensor.read();
			sprintf(msg, "%.03f", temp);
			printf("read temperature: %s\n", msg);
			TempMonitor::Temperature_msg temperature_msg{get_my_id(), msg};

			// don't wait
			xQueueSendToBack(publish_queue, &temperature_msg, 0);
		} while (false);

		xSemaphoreGive(i2c_mutex);

        vTaskDelay(pdMS_TO_TICKS(10000)); // sleep
	}
}

void send_nrf_task(void *pvParameters)
{
	while (true)
	{
		xSemaphoreTake(i2c_mutex, portMAX_DELAY);

		while (uxQueueMessagesWaiting(publish_queue) != 0)
        {
            TempMonitor::Temperature_msg msg;
            xQueueReceive(publish_queue, &msg, 0);
            char message[TempMonitor::MESSAGE_LEN];
            msg.getMessage(message, TempMonitor::MESSAGE_LEN);
            if (nrf_comm.send(message, static_cast<uint8_t>(strlen(message))))
            {
                printf("sent NRF message %s\n", message);
            }
            else
            {
                printf("NRF sending was not successful for %s\n", message);
            }
        }
        printf("finished with NRF sending\n");
		xSemaphoreGive(i2c_mutex);
		vTaskDelay(pdMS_TO_TICKS(10000)); // sleep
	}
}

void listen_nrf_task(void *pvParameters)
{
	char buffer[TempMonitor::MESSAGE_LEN];
	while (true)
	{
		do
		{
			if (!mqtt_client.init(MQTT_TEST_TOPIC, static_cast<uint8_t>(strlen(MQTT_TEST_TOPIC))))
			{
				printf("unsuccessful client init\n");
				break;
			}
			if (!mqtt_client.connect())
			{
				printf("unsuccessful client connect\n");
				// force wifi reconnect
				// sdk_wifi_station_disconnect();
				break;
			}

			xSemaphoreTake(i2c_mutex, portMAX_DELAY);

			// receive all messages
			while (nrf_comm.receive(buffer, sizeof(buffer)))
			{
				printf("----NRF received: %s\n", buffer);
				// publish received messages
				if (!mqtt_client.publish(buffer, static_cast<const uint8_t>(strlen(buffer))))
				{
					printf("unsuccessful client publish\n");
					break;
				}
			}
			xSemaphoreGive(i2c_mutex);
			printf("no more NRF messages waiting\n");

			while (uxQueueMessagesWaiting(publish_queue) != 0)
			{
				TempMonitor::Temperature_msg msg;
				xQueueReceive(publish_queue, &msg, 0);
				char message[TempMonitor::MESSAGE_LEN];
				msg.getMessage(message, TempMonitor::MESSAGE_LEN);
				if (!mqtt_client.publish(message, static_cast<const uint8_t>(strlen(message))))
				{
					printf("unsuccessful client publish: %s\n", message);
					break;
				}
				printf("published %s\n", message);
			}
			printf("published all local temperatures\n");

		mqtt_client.disconnect();
            /*if(!mqtt_client.yield())
            {
                mqtt_client.disconnect();
            }*/

        } while (false);

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
		//sdk_wifi_station_disconnect();
		//retries = 30;
		//sdk_wifi_set_opmode(STATION_MODE);
		//sdk_wifi_station_set_config(&config);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}

void switch_role(TempMonitor::Device_role target_role)
{
    switch (target_role)
    {
    case TempMonitor::NRF_CLIENT:
        if (device_role == TempMonitor::NRF_CLIENT)
        {
            printf("already in NRF client mode\n");
        }
        else
        {
            device_role = TempMonitor::NRF_CLIENT;
            vTaskSuspend(listen_nrf_task_handle);
            vTaskSuspend(wifi_task_handle);
            vTaskResume(send_nrf_task_handle);
        }
        break;
    case TempMonitor::NRF_SERVER_AND_MQTT_CLIENT:
        if (device_role == TempMonitor::NRF_SERVER_AND_MQTT_CLIENT)
        {
            printf("already in MQTT client mode\n");
        }
        else
        {
            device_role = TempMonitor::NRF_SERVER_AND_MQTT_CLIENT;
            vTaskSuspend(send_nrf_task_handle);
            vTaskResume(wifi_task_handle);
            vTaskResume(listen_nrf_task_handle);
        }
        break;

    default:
        printf("invalid mode\n");
    }
}

void button_poll_task(void *pvParameter)
{
	while (true)
	{
		xSemaphoreTake(i2c_mutex, portMAX_DELAY);
		button_reader.init();
		if (!button_reader.read())
		{
			printf("unsuccessful button I2C read\n");
		}
		xSemaphoreGive(i2c_mutex);

		if (button_reader.button1_pressed())
		{
			printf("button1 - switch to NRF sender mode\n");
			switch_role(TempMonitor::NRF_CLIENT);
		}
		else if (button_reader.button4_pressed())
		{
			printf("button4 - switch to MQTT client mode\n");
			switch_role(TempMonitor::NRF_SERVER_AND_MQTT_CLIENT);
		}

		// check again after 200 ms
		vTaskDelay(pdMS_TO_TICKS(200));
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
	wifi_alive = xSemaphoreCreateBinary();
	i2c_mutex = xSemaphoreCreateMutex();
	publish_queue = xQueueCreate(TempMonitor::QUEUE_SIZE, sizeof(TempMonitor::Temperature_msg));
	nrf_comm.init();
	temp_sensor.init();
}

extern "C" void user_init(void); // one way
void user_init(void)
{
	device_role = TempMonitor::NRF_CLIENT;
	// Setup HW
	user_setup_hw();

	other_setup();

    xTaskCreate(button_poll_task, "button_poll_task", 256, nullptr, 2, nullptr);
	xTaskCreate(read_temp_task, "read_temp_task", 512, nullptr, 2, &read_temp_task_handle);
	xTaskCreate(send_nrf_task, "send_nrf_task", 512, nullptr, 2, &send_nrf_task_handle);

	xTaskCreate(wifi_task, "wifi_task", 265, nullptr, 2, &wifi_task_handle);
	vTaskSuspend(wifi_task_handle);

	xTaskCreate(listen_nrf_task, "listen_nrf_task", 512, nullptr, 2, &listen_nrf_task_handle);
	vTaskSuspend(listen_nrf_task_handle);
}
