idf_component_register(SRCS "webserver.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_http_server led        
                    PRIV_REQUIRES esp_wifi esp_event nvs_flash driver main
                    EMBED_FILES "../../web/webserver.html") # Trỏ tới file HTML
                    