#include "button_handler.h"

ButtonHandler::ButtonHandler(uint8_t pin, uint32_t debounce_ms)
    : gpio_pin(pin), debounce_delay(debounce_ms), last_debounce_time(0),
      last_pin_state(true), current_pin_state(true), simulated(false),
      simulated_event(ButtonEvent::NONE) {}

void ButtonHandler::begin() {
    // GPIO initialization placeholder
}

ButtonEvent ButtonHandler::update(uint32_t current_time_ms) {
    (void)current_time_ms;
    if (simulated) {
        ButtonEvent event = simulated_event;
        simulated_event = ButtonEvent::NONE;
        simulated = false;
        return event;
    }
    // Real GPIO pin reading would happen here with active low pull-up
    return ButtonEvent::NONE;
}

void ButtonHandler::simulatePress(ButtonEvent event) {
    simulated = true;
    simulated_event = event;
}
