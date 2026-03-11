#ifndef BUTTON_H
#define BUTTON_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// định nghĩa chân nút bấm 
#define BUTTON_GPIO_PIN 18

// khai báo queue để báo cho task led biết khi nào nút được nhấn
extern QueueHandle_t button_event_queue;

void button_init(int button_pin); // Khởi tạo GPIO cho nút bấm và thiết lập ngắt

#endif