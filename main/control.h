#ifndef CONTROL_H
#define CONTROL_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum {
    SRC_LIGHT,
    SRC_WEB,
    SRC_BUTTON
} control_source_t; 

typedef enum {
    LED_CMD_ON,
    LED_CMD_OFF,
    LED_CMD_TOGGLE
} led_cmd_t;

typedef struct {
    control_source_t source;
    led_cmd_t cmd;
} control_message_t;

extern QueueHandle_t control_queue;

#endif 