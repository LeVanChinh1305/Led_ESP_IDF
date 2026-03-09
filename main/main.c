#include <stdio.h>
#include "freertos/FreeRTOS.h"     // Thư viện lõi của FreeRTOS
#include "freertos/task.h"         // Quản lý Task (xTaskCreate, vTaskDelay...)
#include "led.h"                   // Thư viện LED bạn tự viết
#include "esp_log.h"               // Log của ESP-IDF (ESP_LOGI)
#include "wifi.h"                  // Thư viện WiFi bạn tự viết
#include "nvs_flash.h"              // Thư viện quản lý flash (nvs_flash_init)

static const char *TAG = "MAIN";

QueueHandle_t led_queue; // hàng đợi truyền dữ liệu giữa các task
EventGroupHandle_t led_event_group; // nhóm sự kiện để đồng bộ hóa giữa các task (để bật/tắt led)
#define LED_ON_BIT BIT0 // định nghĩa bit để bật led
#define LED_OFF_BIT BIT1 // định nghĩa bit để tắt led

// led task
void led_task(void *pv){
    while(1){ // Loop forever chạy liên tục 
        EventBits_t bits = xEventGroupWaitBits( // Wait for LED control bits
            led_event_group, // Nhóm sự kiện để chờ
            LED_ON_BIT|LED_OFF_BIT, // Các bit mà task này quan tâm (bật hoặc tắt led)
            pdTRUE, // Clear bits on exit (sau khi nhận được sự kiện sẽ tự động xóa bit đó)
            pdFALSE, // Wait for any bit (không cần phải chờ cả 2 bit cùng lúc)
            portMAX_DELAY // Wait indefinitely (task sẽ chờ mãi cho đến khi có sự kiện xảy ra)
        );
        if(bits & LED_ON_BIT){
            led_on();
            ESP_LOGI("led task ", "led on");
        }
        if(bits & LED_OFF_BIT){
            led_off();
            ESP_LOGI("led task ","led off");
        }
    }
}

// control task 
void control_task(void *pv){// Simulate control commands
    while(1){
        ESP_LOGI("CTRL_TASK", "Send LED ON");// Send LED ON command
        xEventGroupSetBits(led_event_group, LED_ON_BIT); // Set bit để bật led
        vTaskDelay(pdMS_TO_TICKS(2000)); 
        ESP_LOGI("CTRL_TASK", "Send LED OFF");// Send LED OFF command
        xEventGroupSetBits(led_event_group, LED_OFF_BIT); // Set bit để tắt led
        vTaskDelay(pdMS_TO_TICKS(2000)); 
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "App started!");

    nvs_flash_init(); // Khởi tạo NVS (Non-Volatile Storage) để lưu trữ thông tin WiFi
    wifi_init_sta(); // Khởi tạo WiFi

    led_init(GPIO_NUM_20);// Initialize LED on GPIO 20 (đảm bảo rằng bạn đã kết nối LED với chân GPIO này)  

    led_queue = xQueueCreate(5, sizeof(int)); // Tạo hàng đợi với 5 phần tử, mỗi phần tử có kích thước bằng một int (dùng để truyền lệnh bật/tắt led)
    led_event_group = xEventGroupCreate(); // Tạo nhóm sự kiện để đồng bộ hóa giữa các task

    xTaskCreate(led_task, "Led Task", 2048, NULL, 5, NULL);
    xTaskCreate(control_task, "Control Task", 2048, NULL, 4, NULL);
}

