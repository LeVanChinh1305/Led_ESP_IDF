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

static const char *TAG = "MAIN";

EventGroupHandle_t led_event_group;
#define LED_ON_BIT  BIT0
#define LED_OFF_BIT BIT1

void led_task(void *pv){
    int pin_num; // Biến tạm để nhận số chân từ Queue
    while(1){
        // --- NHÁNH 1: ĐỢI LỆNH TỪ WEB (Timeout 10ms thay vì MAX_DELAY) ---
        EventBits_t bits = xEventGroupWaitBits(
            led_event_group, 
            LED_ON_BIT | LED_OFF_BIT, 
            pdTRUE,             
            pdFALSE,            
            pdMS_TO_TICKS(10)   // Chỉ đợi 10ms rồi kiểm tra tiếp việc khác
        );

        if(bits & LED_ON_BIT){
            led_on();
            ESP_LOGI("LED_TASK", "Web: ĐANG BẬT");
        }
        if(bits & LED_OFF_BIT){
            led_off();
            ESP_LOGI("LED_TASK", "Web: ĐANG TẮT");
        }

        // --- NHÁNH 2: ĐỢI LỆNH TỪ NÚT BẤM (QUEUE) ---
        // button_event_queue phải khớp với tên trong button.h của bạn
        if (xQueueReceive(button_event_queue, &pin_num, pdMS_TO_TICKS(10))) {
            // Chống rung (Debounce) đơn giản
            vTaskDelay(pdMS_TO_TICKS(50));
            
            if (gpio_get_level(pin_num) == 0) { // Nếu vẫn đang nhấn (mức thấp)
                // Đảo trạng thái LED
                if (led_get_state() == 1) {
                    led_off();
                    ESP_LOGI("LED_TASK", "Button: Chuyển sang TẮT");
                } else {
                    led_on();
                    ESP_LOGI("LED_TASK", "Button: Chuyển sang BẬT");
                }
            }
        }
        
        // Tránh chiếm dụng CPU quá mức
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "He thong dang khoi dong...");

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
    led_event_group = xEventGroupCreate();
    led_init(GPIO_NUM_20);
    button_init(BUTTON_GPIO_PIN); 

    // 4. Tạo Task (Tăng stack lên 4096 cho an toàn vì có LOG và Queue)
    xTaskCreate(led_task, "Led Task", 4096, NULL, 5, NULL);

    // 5. Khởi động Web Server
    start_webserver();
}