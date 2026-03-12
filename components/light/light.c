#include "light.h"
#include "esp_adc/adc_oneshot.h" // Thư viện ADC OneShot cho ESP32
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern EventGroupHandle_t led_event_group; // Sử dụng Event Group đã tạo trong main.c
#define LED_ON_BIT  BIT0
#define LED_OFF_BIT BIT1

static adc_oneshot_unit_handle_t adc_handle; // Handle cho ADC OneShot
const char *TAG = "LIGHT_SENSOR";

void light_sensor_init(void){
    // Cấu hình unit ADC OneShot
    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = ADC_UNIT_1, // Sử dụng ADC1
        .ulp_mode = ADC_ULP_MODE_DISABLE, // Không sử dụng chế độ ULP
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &adc_handle)); // Khởi tạo ADC OneShot
    // Cấu hình channel 
    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_11, // Độ suy hao 11dB (tương đương với dải điện áp 0-3.6V)
        .bitwidth = ADC_BITWIDTH_DEFAULT, // Độ phân giải mặc định (12-bit)
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, LIGHT_SENSOR_CHANNEL, &channel_config)); // Cấu hình channel
    ESP_LOGI(TAG, "Light sensor initialized");
}
float read_light_sensor(void){
    int adc_value = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, LIGHT_SENSOR_CHANNEL, &adc_value)); // Đọc giá trị ADC
    ESP_LOGI(TAG, "ADC Value: %d", adc_value);
    return adc_value;
}
float convert_adc_to_light_percentage(void){
    float adc_value = read_light_sensor(); // Đọc giá trị ADC
    // Giả sử giá trị ADC tối đa là 4095 (12-bit), chuyển đổi sang phần trăm
    float percentage = (adc_value * 100) / 4095;
    if(percentage > 100) percentage = 100; // Giới hạn phần trăm tối đa là 100%
    if(percentage < 0) percentage = 0; // Giới hạn phần trăm tối thiểu là 0%
    if (led_event_group != NULL) {
        if (percentage < 30.0) {
            xEventGroupSetBits(led_event_group, LED_ON_BIT); // Trời tối -> Bật
        } else if (percentage > 70.0) {
            xEventGroupSetBits(led_event_group, LED_OFF_BIT); // Trời sáng -> Tắt
        }
    }
    ESP_LOGI(TAG, "Light Percentage: %.2f%%", percentage);
    return percentage;
}