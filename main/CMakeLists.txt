# main/CMakeLists.txt

# Sources
file(GLOB_RECURSE srcs
    "*.c"
    "ui/*.c"
    "ui/components/*.c"
    "ui/screens/*.c"
    "ui/images/*.c"
    "user/add_password/*.c"
    "user/wifi/*.c"
    "user/rs485/*.c"
    "user/can/*.c"
    "user/pwm/*.c"
    "user/sd_card/*.c"
)

# Include dirs
set(include_dirs
    "."
    "ui"
    "ui/components"
    "ui/screens"
    "ui/images"
    "user/add_password"
    "user/wifi"
    "user/rs485"
    "user/can"
    "user/pwm"
    "user/sd_card"
)

idf_component_register(
    SRCS         ${srcs}
    INCLUDE_DIRS ${include_dirs}
    REQUIRES
        lvgl__lvgl
        lvgl_port
        rgb_lcd_port
        touch
        image
        fonts
        gpio
        i2c
        io_extension
        sd
        twai
        usart
        gui_paint
        LovyanGFX
        esp_timer
        esp_lcd
        esp_wifi
        esp_netif
        esp_event
        nvs_flash
    PRIV_REQUIRES
        mbedtls
)

# Optionnel : réduire le bruit des warnings
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format)

# Optionnel : idem pour LVGL
idf_component_get_property(lvgl_lib lvgl__lvgl COMPONENT_LIB)
target_compile_options(${lvgl_lib} PRIVATE -Wno-format)
