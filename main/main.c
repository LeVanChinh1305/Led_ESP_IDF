#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "led.h"
#include "esp_log.h"
#include "wifi.h"
#include "nvs_flash.h"
#include "webserver.h"
#include "button.h" // Đã có button_event_queue ở đây
#include "DHT11.h" // Đã có dht11_read ở đây
#include "light.h" // Đã có light_read ở đây
#include "control.h" // Đã có control_queue ở đây

QueueHandle_t control_queue;

static const char *TAG = "MAIN";

// Biến toàn cục để Webserver có thể truy cập (extern trong webserver.c)
float temperature = 0.0;
float humidity = 0.0;
float light = 0.0; 

void light_task(void *pv){
    ESP_LOGI(TAG, "Light Task started");
    light_sensor_init(); // Khởi tạo cảm biến ánh sáng (cấu hình ADC)
    while(1){
        light = convert_adc_to_light_percentage(); // Đọc giá trị ánh sáng từ cảm biến
        ESP_LOGI(TAG, "Light : %.2f%%", light);
        
        control_message_t msg;
        msg.source = SRC_LIGHT;
        if (light < 25.0) {
            msg.cmd = LED_CMD_ON;
            xQueueSend(control_queue, &msg, 0); // Gửi lệnh bật LED nếu ánh sáng yếu
        } else {
            msg.cmd = LED_CMD_OFF;
            xQueueSend(control_queue, &msg, 0); // Gửi lệnh tắt LED nếu ánh sáng đủ
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Đọc mỗi 2 giây
    }
}

void dht11_task(void *pv){
    ESP_LOGI(TAG, "DHT11 Task started");
    dht11_init(); // Khởi tạo DHT11 (cấu hình GPIO)
    while(1){
        float temp, hum;
        if (dht11_read(&temp, &hum) == ESP_OK) {
            temperature = temp;
            humidity = hum;
        } else {
            ESP_LOGE(TAG, "Failed to read from DHT11 sensor");
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Đọc mỗi 2 giây
    }
}

void button_task(void *pv){
    int pin;
    while(1){
        if(xQueueReceive(button_event_queue, &pin, portMAX_DELAY)) {
            // Chống rung (Debounce) đơn giản
            vTaskDelay(pdMS_TO_TICKS(50));
            
            if (gpio_get_level(pin) == 0) { // Nếu vẫn đang nhấn (mức thấp)
                control_message_t msg;
                msg.source = SRC_BUTTON;
                msg.cmd = LED_CMD_TOGGLE; // Gửi lệnh toggle để đảo trạng thái LED
                xQueueSend(control_queue, &msg, 0);
            }
        }
    }
}

void led_task(void *pv){
    control_message_t msg;
    while(1){
        if(xQueueReceive(control_queue, &msg, portMAX_DELAY)) {
            switch(msg.cmd) {
                case LED_CMD_ON:
                    led_on();
                    break;
                case LED_CMD_OFF:
                    led_off();
                    break;
                case LED_CMD_TOGGLE:
                    led_toggle();
                    break;
            }
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "He thong dang khoi dong...");

    control_queue = xQueueCreate(10, sizeof(control_message_t));

    if(control_queue == NULL){
        ESP_LOGE(TAG, "Queue create failed!");
        return;
    }

    // 1. Khởi tạo NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // 2. WiFi
    wifi_init_sta();

    // 3. Phần cứng (Lưu ý: Khởi tạo Event Group TRƯỚC khi khởi tạo Button/Web)
    led_init(GPIO_NUM_20);
    button_init(BUTTON_GPIO_PIN); 

    // 4. Tạo Task (Tăng stack lên 4096 cho an toàn vì có LOG và Queue)
    xTaskCreate(led_task, "Led Task", 4096, NULL, 5, NULL);
    xTaskCreate(dht11_task, "DHT11 Task", 4096, NULL, 4, NULL); // Tạo thêm task đọc DHT11
    xTaskCreate(light_task, "Light Task", 4096, NULL, 4, NULL); // Tạo thêm task đọc cảm biến ánh sáng
    xTaskCreate(button_task, "Button Task", 4096, NULL, 4, NULL); // Tạo thêm task xử lý nút bấm

    // 5. Khởi động Web Server
    start_webserver();
}