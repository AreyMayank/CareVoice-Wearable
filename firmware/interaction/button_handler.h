#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <cstdint>

enum class ButtonEvent {
    NONE,
    SHORT_PRESS,
    LONG_PRESS
};

class ButtonHandler {
public:
    ButtonHandler(uint8_t pin = 20, uint32_t debounce_ms = 50);

    void begin();
    ButtonEvent update(uint32_t current_time_ms);
    void simulatePress(ButtonEvent event);

private:
    uint8_t gpio_pin;
    uint32_t debounce_delay;
    uint32_t last_debounce_time;
    bool last_pin_state;
    bool current_pin_state;
    bool simulated;
    ButtonEvent simulated_event;
};

#endif // BUTTON_HANDLER_H
