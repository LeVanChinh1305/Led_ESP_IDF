#pragma once 
 // file header này chỉ được include một lần duy nhất trong quá trình biên dịch 
// đây là cách viét ngắn gọn cho cấu trúc sau : 
/*
#ifndef LED_H
#define LED_H
...
#endif
*/

#include "driver/gpio.h" 
// lấy thư viện gpio của esp-idf


// khai báo các hàm 
void led_init(gpio_num_t pin);// hàm khởi tạo led 
void led_on(void);// bật led 
void led_off(void);// tắt led
void led_toggle(void);// đảo trạng thái led
int led_get_state(void); // lấy trạng thái led
// -> tạo ra một led driver đơn giản với nhiệm vụ khia báo các hàm
// các hàm này được triển khai chi tiết trong .c  và được gọi trong main.c 