#include "led.h"
#include "driver/gpio.h"

static gpio_num_t led_pin;
static uint8_t led_state = 0;

void led_init(gpio_num_t pin){ // Initialize the LED pin
    led_pin = pin;
    gpio_reset_pin(led_pin);
    gpio_set_direction(led_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(led_pin, 0);
}

void led_on(void){ // Turn the LED on
    led_state = 1;
    gpio_set_level(led_pin, 1);
}

void led_off(void){ // Turn the LED off
    led_state = 0;
    gpio_set_level(led_pin, 0);
}

void led_toggle(void){ // Toggle the LED state
    led_state = !led_state;
    gpio_set_level(led_pin, led_state);
}
