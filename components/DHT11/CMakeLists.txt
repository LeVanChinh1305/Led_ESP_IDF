# Nếu bạn để thư viện dht gốc cùng cấp hoặc dùng thư viện từ esp-idf-lib
idf_component_register(SRCS "DHT11.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp-idf-lib__dht) // Priv_REQUIRES để chỉ định thư viện dht là phụ thuộc nội bộ, không cần phải cài đặt riêng
                    