idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES led button wifi DHT11 light webserver)
