#ifndef ESP_8266_TEMPERATURE_MONITOR_BUTTON_READER_H
#define ESP_8266_TEMPERATURE_MONITOR_BUTTON_READER_H

#include <cstdint>

namespace TempMonitor
{

/**
 * Reads button presses.
 */
class Button_reader
{

public:

    /**
     * Initialises the interface.
     */
    void init();

    /**
     * Reads the state of buttons and stores the state into internal buffer.
     * @return <code>true</code> if the read was successful, else return <code>false</code>.
     */
    bool read();

    /**
     * @return <code>true</code> if the 1st button was pressed.
     */
    bool button1_pressed() const;

    /**
     * @return <code>true</code> if the 2nd button was pressed.
     */
    bool button2_pressed() const;

    /**
     * @return <code>true</code> if the 3rd button was pressed.
     */
    bool button3_pressed() const;

    /**
     * @return <code>true</code> if the 4th button was pressed.
     */
    bool button4_pressed() const;

private:

    /**
     * Internal buffer for storing button state.
     */
    uint8_t data;

    // button masks.
    static constexpr uint8_t BUTTON1 = 0x20;    // 0b ??0? ????
    static constexpr uint8_t BUTTON2 = 0x10;    // 0b ???0 ????
    static constexpr uint8_t BUTTON3 = 0x80;    // 0b 0??? ????
    static constexpr uint8_t BUTTON4 = 0x40;    // 0b ?0?? ????

}; // class

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_BUTTON_READER_H
