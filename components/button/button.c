#include "button.h"
#include "esp_log.h"

static const char *TAG = "BUTTON_COMPONENT";

QueueHandle_t button_event_queue = NULL;
static bool isr_service_installed = false; // Biến tĩnh để kiểm tra dịch vụ ngắt

static void IRAM_ATTR button_isr_handler(void* arg) {
    int button_pin = (int) arg; 
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Gửi số chân vào queue
    xQueueSendFromISR(button_event_queue, &button_pin, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

void button_init(int button_pin) {
    if (button_event_queue == NULL) {
        button_event_queue = xQueueCreate(10, sizeof(int));
    }

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,         // Ngắt khi nút được nhấn (giả sử nút nối đất)
        .mode = GPIO_MODE_INPUT,                // Chân nút là ngõ vào
        .pin_bit_mask = (1ULL << button_pin),   // Chỉ cấu hình chân nút
        .pull_up_en = GPIO_PULLUP_ENABLE,       // Kích hoạt điện trở kéo lên để đảm bảo chân ở mức cao khi nút không được nhấn
        .pull_down_en = GPIO_PULLDOWN_DISABLE   // Không cần điện trở kéo xuống
    };
    gpio_config(&io_conf);

    // Chỉ cài đặt dịch vụ ngắt nếu chưa có
    if (!isr_service_installed) {
        esp_err_t ret = gpio_install_isr_service(0);
        if (ret == ESP_OK) {
            isr_service_installed = true;
        } else {
            ESP_LOGE(TAG, "Khong the cai dat ISR service!");
        }
    }

    gpio_isr_handler_add(button_pin, button_isr_handler, (void*) button_pin);
    ESP_LOGI(TAG, "Da khoi tao nut bam tai GPIO %d", button_pin);
}