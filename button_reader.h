#ifndef ESP_8266_TEMPERATURE_MONITOR_BUTTON_READER_H
#define ESP_8266_TEMPERATURE_MONITOR_BUTTON_READER_H

#include <cstdint>

namespace TempMonitor
{

class Button_reader
{

public:

    void init();

    bool read();

    bool button1_pressed() const;

    bool button2_pressed() const;

    bool button3_pressed() const;

    bool button4_pressed() const;

private:

    uint8_t data;

    static constexpr uint8_t BUTTON1 = 0x20;    // 0b ??0? ????
    static constexpr uint8_t BUTTON2 = 0x10;    // 0b ???0 ????
    static constexpr uint8_t BUTTON3 = 0x80;    // 0b 0??? ????
    static constexpr uint8_t BUTTON4 = 0x40;    // 0b ?0?? ????

}; // class

} // namespace

#endif //ESP_8266_TEMPERATURE_MONITOR_BUTTON_READER_H
