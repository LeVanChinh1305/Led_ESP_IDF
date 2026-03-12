#include "DHT11.h"
#include "esp_log.h" // Thư viện để sử dụng chức năng logging của ESP-IDF
#include "dht.h" // Thư viện DHT11 của Adafruit (đã được thêm vào dự án)

void dht11_init(void) {
    // Chỉ cần đảm bảo chân 4 không bị kéo xuống (Pull-down) 
    // và kích hoạt Pull-up nội bộ nếu module DHT11 của Chính không có trở dán sẵn.
    gpio_set_pull_mode(DHT11_GPIO_PIN, GPIO_PULLUP_ONLY);
}
esp_err_t dht11_read(float *temperature, float *humidity) {
    // Tài liệu bản 1.2.0 dùng hàm này là chuẩn nhất
    esp_err_t ret = dht_read_float_data(DHT_TYPE_DHT11, DHT11_GPIO_PIN, humidity, temperature);
    
    if (ret == ESP_OK) {
        // Log để kiểm tra trên Terminal của ESP-IDF
        ESP_LOGI("DHT", "Temp: %.1f, Hum: %.1f", *temperature, *humidity);
        return ESP_OK;
    } else {
        // Các mã lỗi từ thư viện esp-idf-lib
        switch (ret) {
            case ESP_ERR_TIMEOUT:
                ESP_LOGE("DHT", "Loi Timeout - Kiem tra day cam chan 4!");
                break;
            case ESP_ERR_INVALID_CRC:
                ESP_LOGE("DHT", "Loi Checksum - Tin hieu bi nhieu!");
                break;
            default:
                ESP_LOGE("DHT", "Loi khong xac dinh: %d", ret);
        }
        return ret;
    }
}