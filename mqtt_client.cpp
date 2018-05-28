#include <cstdio>
#include <cstring>
#include "mqtt_client.h"
#include "secret_constants.h"

bool TempMonitor::Mqtt_client::init(const char *const client_id, uint8_t client_id_len)
{
    if (client_id_len >= MQTT_ID_LEN)
    {
        return false;
    }
    strncpy(id, client_id, client_id_len);
    client = mqtt_client_default;
    connection_data = mqtt_packet_connect_data_initializer;
    mqtt_network_new(&network);
    return true;
}

bool TempMonitor::Mqtt_client::connect()
{
    int ret = mqtt_network_connect(&network, MQTT_HOST, MQTT_PORT);
    if (ret != 0)
    {
        printf("error (network) connecting to MQTT: %d\n", ret);
        return false;
    }
    printf("network connected to MQTT\n");
    mqtt_client_new(&client, &network, MQTT_COMMAND_TIMEOUT_MS, buffer, MQTT_BUFFER_LEN, read_buffer, MQTT_BUFFER_LEN);

    connection_data.willFlag = 0;
    connection_data.MQTTVersion = 3;
    connection_data.clientID.cstring = id;
    connection_data.username.cstring = const_cast<char *>(MQTT_USER);
    connection_data.password.cstring = const_cast<char *>(MQTT_PASS);
    connection_data.keepAliveInterval = 10;
    connection_data.cleansession = 0;

    ret = mqtt_connect(&client, &connection_data);
    if (ret != 0)
    {
        printf("error connecting to MQTT, disconnecting: %d\n", ret);
        mqtt_network_disconnect(&network);
        return false;
    }
    printf("connected to MQTT\n");
	conected = true;
    return true;
}

bool TempMonitor::Mqtt_client::publish(const char *const msg, const uint8_t msg_len)
{
    if (msg_len >= MQTT_BUFFER_LEN)
    {
        printf("MQTT message too long for buffer size %d\n", MQTT_BUFFER_LEN);
        return false;
    }
    mqtt_message_t message;
    message.payload = const_cast<char *>(msg);
    message.payloadlen = msg_len;
    message.dup = 0;
    message.qos = MQTT_QOS1;
    message.retained = 0;
    int ret = mqtt_publish(&client, MQTT_TOPIC, &message);
    if (ret != MQTT_SUCCESS)
    {
        printf("error publishing MQTT message: %d\n", ret);
        return false;
    }
    return true;
}

bool TempMonitor::Mqtt_client::yield()
{
    printf("MQTT yield()\n");
	if(conected){
		return mqtt_yield(&client, MQTT_COMMAND_TIMEOUT_MS) == MQTT_SUCCESS;
	}
}

void TempMonitor::Mqtt_client::disconnect()
{
    printf("MQTT disconnect()\n");
	conected = false;
    mqtt_network_disconnect(&network);
}
