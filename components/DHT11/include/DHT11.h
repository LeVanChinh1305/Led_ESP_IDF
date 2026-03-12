#ifndef DHT11_H
#define DHT11_H

#include "driver/gpio.h" // Thư viện để sử dụng GPIO của ESP-IDF
#include "esp_err.h" // Thư viện để sử dụng các mã lỗi chuẩn của ESP-IDF

// cấu hình chân 
#define DHT11_GPIO_PIN 4 // Chân dữ liệu của DHT11 được kết nối vào GPIO4
// khai báo các hàm giao tiếp với DHT11
void dht11_init(void); // Hàm khởi tạo DHT11 (cấu hình GPIO)
esp_err_t dht11_read(float *temperature, float *humidity); // Hàm đọc dữ liệu từ DHT11, trả về mã lỗi nếu có

#endif