#include <stdio.h>
#include <string.h>
#include "webserver.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "led.h"
#include "control.h"

static const char *TAG = "WEB_SERVER";

extern const uint8_t web_html_start[] asm("_binary_webserver_html_start");
extern const uint8_t web_html_end[]   asm("_binary_webserver_html_end");

static esp_err_t root_get_handler(httpd_req_t *req) {
    const size_t web_html_size = (web_html_end - web_html_start);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)web_html_start, web_html_size);
    return ESP_OK;
}

static esp_err_t led_on_handler(httpd_req_t *req) {

    control_message_t msg = {
        .source = SRC_WEB,
        .cmd = LED_CMD_ON
    };

    xQueueSend(control_queue, &msg, 0);

    ESP_LOGI(TAG, "Web Request: LED ON");

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t led_off_handler(httpd_req_t *req) {

    control_message_t msg = {
        .source = SRC_WEB,
        .cmd = LED_CMD_OFF
    };

    xQueueSend(control_queue, &msg, 0);

    ESP_LOGI(TAG, "Web Request: LED OFF");

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t get_status_handler(httpd_req_t *req){
    int led_state = led_get_state();
    const char* status_str = (led_state == 1) ? "ON" : "OFF";

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, status_str, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t dht11_sensor_data_get_handler(httpd_req_t *req) {

    extern float temperature, humidity;

    char json_str[64];

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