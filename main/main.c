#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "esp_log.h"

static const char *TAG = "MAIN";

QueueHandle_t led_queue;
EventGroupHandle_t led_event_group;
#define LED_ON_BIT BIT0
#define LED_OFF_BIT BIT1

// led task
void led_task(void *pv){
    while(1){
        EventBits_t bits = xEventGroupWaitBits(
            led_event_group,
            LED_ON_BIT|LED_OFF_BIT,
            pdTRUE,
            pdFALSE, 
            portMAX_DELAY
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
void control_task(void *pv){
    while(1){
        ESP_LOGI("CTRL_TASK", "Send LED ON");
        xEventGroupSetBits(led_event_group, LED_ON_BIT);
        vTaskDelay(pdMS_TO_TICKS(2000)); 
        ESP_LOGI("CTRL_TASK", "Send LED ");
        xEventGroupSetBits(led_event_group, LED_OFF_BIT);
        vTaskDelay(pdMS_TO_TICKS(2000)); 
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "App started!");

    led_init(GPIO_NUM_20);

    led_queue = xQueueCreate(5, sizeof(int));
    led_event_group = xEventGroupCreate();

    xTaskCreate(led_task, "Led Task", 2048, NULL, 5, NULL);
    xTaskCreate(control_task, "Control Task", 2048, NULL, 4, NULL);
}

