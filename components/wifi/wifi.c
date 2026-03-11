#include "wifi.h"           // 
#include "esp_wifi.h"       // Thư viện WiFi của ESP-IDF
#include "esp_event.h"      // Thư viện quản lý sự kiện của ESP-IDF
#include "esp_log.h"        // Thư viện logging của ESP-IDF
#include "nvs_flash.h"      // Thư viện quản lý flash NVS của ESP-IDF
#include "esp_netif.h"      // Thư viện quản lý giao diện mạng của ESP-IDF

#define WIFI_SSID "SUA CHUA TRAN CHAU HA LONG"
#define WIFI_PASS "@12345678"

static const char *TAG = "wifi";

// Hàm này sẽ được hệ thống gọi tự động khi có các sự kiện mạng xảy ra.
/* @param arg: Dữ liệu người dùng truyền vào (thường là NULL).
 * @param event_base: Loại sự kiện (Ví dụ: WIFI_EVENT hoặc IP_EVENT).
 * @param event_id: ID cụ thể của sự kiện đó (Ví dụ: START, CONNECTED, GOT_IP).
 * @param event_data: Dữ liệu đi kèm sự kiện (Ví dụ: cấu hình IP, lý do mất mạng).
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){

    // 1. Xử lý sự kiện WiFi bắt đầu (STA_START)
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    // 2. Xử lý sự kiện WiFi bị ngắt kết nối (STA_DISCONNECTED)
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected from WiFi, trying to reconnect...");
        esp_wifi_connect();
    // 3. Xử lý sự kiện khi WiFi đã kết nối và nhận được IP (STA_GOT_IP)
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data; // Ép kiểu dữ liệu event_data sang cấu trúc chứa thông tin IP
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));// In ra địa chỉ IP đã nhận được
    }
}

// Hàm khởi tạo WiFi ở chế độ station (STA)
void wifi_init_sta(void) {
    ESP_LOGI(TAG, "Initializing WiFi in station mode...");
    // Khởi tạo TCP/IP stack va vong lặp sự kiện mặc định
    esp_netif_init(); // Khởi tạo TCP/IP stack
    esp_event_loop_create_default(); // Tạo vòng lặp sự kiện mặc định

    // Tạo giao diện mạng mặc định cho WiFi station
    esp_netif_create_default_wifi_sta();
    
    //dang ky ham wifi_event_handler de xu ly cac su kien wifi va ip
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();// Lấy cấu hình khởi tạo WiFi mặc định
    esp_wifi_init(&cfg); // Khởi tạo WiFi với cấu hình mặc định
    
    // Đăng ký hàm xử lý sự kiện cho các sự kiện WiFi và IP
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_register( // Đăng ký cho tất cả các sự kiện WiFi (bất kể ID nào) để xử lý kết nối và ngắt kết nối
        WIFI_EVENT, 
        ESP_EVENT_ANY_ID, 
        &wifi_event_handler, 
        NULL, 
        &instance_any_id);

    // Đăng ký hàm xử lý sự kiện cho sự kiện khi nhận được IP
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(
        IP_EVENT, 
        IP_EVENT_STA_GOT_IP, // Chỉ đăng ký cho sự kiện khi nhận được IP, không cần xử lý tất cả các sự kiện IP
        &wifi_event_handler, 
        NULL, 
        &instance_got_ip);
    
    // 4. Cấu hình WiFi với SSID và mật khẩu, sau đó khởi động WiFi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    
    // Thiết lập chế độ WiFi là station, cấu hình WiFi và khởi động WiFi
    esp_wifi_set_mode(WIFI_MODE_STA); // Thiết lập chế độ WiFi là station
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config); // Cấu hình WiFi với SSID và mật khẩu
    esp_wifi_start();// Khởi động WiFi
    ESP_LOGI(TAG, "WiFi initialization completed.");
}