idf_component_register(SRCS "led.c" INCLUDE_DIRS "." REQUIRES esp_driver_gpio)
# “Đây là 1 component, compile file led.c và cho phép include led.h”.
#| Bạn include gì      | Phải thêm vào REQUIRES |
#| ------------------- | ---------------------- |
#| `driver/gpio.h`     | `esp_driver_gpio`      |
#| `driver/uart.h`     | `esp_driver_uart`      |
#| `esp_wifi.h`        | `esp_wifi`             |
#| `esp_http_client.h` | `esp_http_client`      |
