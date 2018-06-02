#ifndef ESP_8266_TEMPERATURE_MONITOR_MQTT_CLIENT_H
#define ESP_8266_TEMPERATURE_MONITOR_MQTT_CLIENT_H

#include <cstdint>
#include <string>
#include "constants.h"

extern "C" {
#include "paho_mqtt_c/MQTTESP8266.h"
#include "paho_mqtt_c/MQTTClient.h"
}

namespace TempMonitor
{

/**
 * MQTT client.
 */
class Mqtt_client
{

public:

	/**
	 * Constructor.
	 */
	Mqtt_client();

    /**
     * Initialises MQTT client.
     * @param client_id ID of the client.
     * @param client_id_len string length of ID.
     * @return <code>true</code> if the initialisation was successful, else returns <code>false</code>.
     */
    bool init(const char *const client_id, uint8_t client_id_len);

    /**
     * Connects to the MQTT broker.
     * @return <code>true</code> if the connection was successful, else returns <code>false</code>.
     */
    bool connect();

    /**
     * Publishes (sends) a message to MQTT broker.
     * @param msg message to send.
     * @param msg_len message length.
     * @return <code>true</code> if the publish operation was successful, else returns <code>false</code>.
     */
    bool publish(const char *const msg, const uint8_t msg_len);

    /**
     * Keeps the connection with the broker alive and blocks execution.
     * @param duration_ms time to block execution.
     * @return <code>true</code> if the yield was successful.
     */
	bool yield(const uint32_t duration_ms = MQTT_COMMAND_TIMEOUT_MS);

	/**
	 * Sends an MQTT disconnect packet to the broker.
	 */
    void disconnect();

private:

    char id[MQTT_ID_LEN];

    mqtt_network_t network;

    mqtt_client_t client;

    mqtt_packet_connect_data_t connection_data;

    uint8_t buffer[MQTT_BUFFER_LEN];

    uint8_t read_buffer[MQTT_BUFFER_LEN];

    bool connected;

}; // class

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_MQTT_CLIENT_H
