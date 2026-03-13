#ifndef LIGHT_H
#define LIGHT_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "hal/adc_types.h" // Thư viện ADC cho ESP32

// đinh nghĩa chân ADC cho cảm biến ánh sáng (ví dụ: ADC1_CHANNEL_0 là GPIO36 trên ESP32)
#define LIGHT_SENSOR_CHANNEL ADC_CHANNEL_1

// hàm khởi tạo cảm biến ánh sáng
void light_sensor_init(void);

// đọc giá trị từ cảm biến ánh sáng
float read_light_sensor(void);

// chuyển đổi giá trị ADC sang mức độ sáng (0-100%)
float convert_adc_to_light_percentage(void);

#endif