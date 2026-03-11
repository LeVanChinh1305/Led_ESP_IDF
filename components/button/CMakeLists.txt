idf_component_register(
    SRCS "button.c"
    INCLUDE_DIRS "include"
    REQUIRES "driver" # Cần driver để dùng gpio.h
)