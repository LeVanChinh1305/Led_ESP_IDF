#include <stdio.h>
#include <string.h>
#include "webserver.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "led.h"


// Kết nối với Event Group từ main.c
extern EventGroupHandle_t led_event_group; // extern để truy cập biến toàn cục đã được khai báo trong main.c
#define LED_ON_BIT  BIT0 
#define LED_OFF_BIT BIT1 

static const char *TAG = "WEB_SERVER";

/**
 * Các ký hiệu này được tạo ra bởi hệ thống build (CMake EMBED_FILES).
 * Tên biến được quy định theo định dạng: _binary_[tên_file]_[phần_mở_rộng]_[start/end]
 * Vì file của bạn là webserver.html, nên tên sẽ như sau:
 */
extern const uint8_t web_html_start[] asm("_binary_webserver_html_start");
extern const uint8_t web_html_end[]   asm("_binary_webserver_html_end");

/* Handler phục vụ trang chủ bằng cách gửi file HTML đã nhúng */
static esp_err_t root_get_handler(httpd_req_t *req) {
    const size_t web_html_size = (web_html_end - web_html_start); // Tính dung lượng file
    httpd_resp_set_type(req, "text/html"); // Báo cho trình duyệt đây là file HTML
    httpd_resp_send(req, (const char *)web_html_start, web_html_size); // Đẩy dữ liệu đi
    return ESP_OK;
}

/* Handler khi người dùng nhấn "BẬT ĐÈN" */
static esp_err_t led_on_handler(httpd_req_t *req) {
    if (led_event_group != NULL) {
        xEventGroupSetBits(led_event_group, LED_ON_BIT);
        ESP_LOGI(TAG, "Web Request: LED ON");
    }
    // Trả về phản hồi trống với mã 200 OK, không bắt tải lại trang
    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Handler khi người dùng nhấn "TẮT ĐÈN" */
static esp_err_t led_off_handler(httpd_req_t *req) {
    if (led_event_group != NULL) {
        xEventGroupSetBits(led_event_group, LED_OFF_BIT);
        ESP_LOGI(TAG, "Web Request: LED OFF");
    }
    // Trả về phản hồi trống với mã 200 OK
    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
/*Handler trả về trạng thái led hiện tại*/
static esp_err_t get_status_handler(httpd_req_t *req){
    // đọc trạng thái vật lý của chân led 
    int led_state = led_get_state(); // Hàm này cần được định nghĩa trong led.c để trả về 1 nếu bật, 0 nếu tắt
    const char* status_str = (led_state == 1) ? "ON" : "OFF";
    httpd_resp_set_type(req, "text/plain"); // Báo cho trình duyệt đây là văn bản thuần túy
    httpd_resp_send(req, status_str, HTTPD_RESP_USE_STRLEN); // Trả về trạng thái hiện tại
    return ESP_OK;
}
// Thêm handler này vào webserver.c
static esp_err_t dht11_sensor_data_get_handler(httpd_req_t *req) {
    extern float temperature, humidity; // Lấy từ biến toàn cục trong main.c
    char json_str[64];
    
    // Tạo cấu trúc JSON đơn giản
    sprintf(json_str, "{\"t\":%.1f,\"h\":%.1f}", temperature, humidity);
    
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json_str, HTTPD_RESP_USE_STRLEN);
}

/* Cấu hình các đường dẫn URI */
static const httpd_uri_t uri_root = { .uri = "/", .method = HTTP_GET, .handler = root_get_handler };
static const httpd_uri_t uri_on   = { .uri = "/led_on", .method = HTTP_GET, .handler = led_on_handler };
static const httpd_uri_t uri_off  = { .uri = "/led_off", .method = HTTP_GET, .handler = led_off_handler };
static const httpd_uri_t uri_sensor_dht11 = { .uri = "/sensor_data", .method = HTTP_GET, .handler = dht11_sensor_data_get_handler };
static const httpd_uri_t uri_status = { .uri = "/led_status", .method = HTTP_GET, .handler = get_status_handler };

/* Hàm khởi chạy Server */
httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    // Bắt đầu HTTP Server
    if (httpd_start(&server, &config) == ESP_OK) {
        // Đăng ký các hàm xử lý cho từng đường dẫn
        httpd_register_uri_handler(server, &uri_root);
        httpd_register_uri_handler(server, &uri_on);
        httpd_register_uri_handler(server, &uri_off);
        httpd_register_uri_handler(server, &uri_status);
        httpd_register_uri_handler(server, &uri_sensor_dht11);
        ESP_LOGI(TAG, "Web Server started successfully!");
        return server;
    }
    
    ESP_LOGE(TAG, "Failed to start Web Server");
    return NULL;
}