#ifndef MQTT_H
#define MQTT_H

namespace TempMonitor
{

/**
 * NRF transmitter.
 */
class MQTT
{

public:

    /**
     * Constructor.
     */
    MQTT();

    /**
     * Initialises MQTT.
     * @return <code>true</code> if the initialisation was successful, else it returns <code>false</code>.
     */
    void init(QueueHandle_t * quee);

    /**
     * Writet to nrf
     * @return bool
     */
    void mqtt_task(void *pvParameters);
	
	const char* get_my_id();

private:

    /** Is the sensor initialised. */
    bool valid;
	
	QueueHandle_t publish_queue;


}; // class

} // namespace

#endif //MQTT_H
