#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <esp_http_server.h>

/**
 * @brief Khởi tạo và chạy Web Server
 * @return httpd_handle_t Handle của server hoặc NULL nếu thất bại
 */
httpd_handle_t start_webserver(void);

/**
 * @brief Dừng Web Server
 * @param server Handle của server cần dừng
 */
void stop_webserver(httpd_handle_t server);

#endif