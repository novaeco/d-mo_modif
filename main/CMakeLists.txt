# main/CMakeLists.txt

# Sources (mettre à jour cette liste lors de l'ajout ou suppression de fichiers)
set(SRCS
    main.c
    ui/can.c
    ui/components/ui_comp_hook.c
    ui/create_user.c
    ui/images/ui_img_485_png.c
    ui/images/ui_img_can_png.c
    ui/images/ui_img_eye_png.c
    ui/images/ui_img_log_on_png.c
    ui/images/ui_img_ok_png.c
    ui/images/ui_img_pwm_png.c
    ui/images/ui_img_wifi_1_png.c
    ui/images/ui_img_wifi_2_png.c
    ui/images/ui_img_wifi_3_png.c
    ui/images/ui_img_wifi_4_png.c
    ui/images/ui_img_wifi_png.c
    ui/login.c
    ui/main_screen.c
    ui/pwm.c
    ui/rs485.c
    ui/screens/ui_Create_Uesr.c
    ui/screens/ui_Login.c
    ui/screens/ui_Main.c
    ui/screens/ui_Main_CAN.c
    ui/screens/ui_Main_PWM.c
    ui/screens/ui_Main_RS485.c
    ui/screens/ui_Main_WIFI.c
    ui/ui.c
    ui/ui_events.c
    ui/ui_helpers.c
    ui/wifi.c
    user/add_password/add_password.c
    user/can/can.c
    user/pwm/pwm.c
    user/rs485/rs485.c
    user/sd_card/sd_card.c
    user/wifi/wifi.c
    user/wifi/wifi_ap.c
    user/wifi/wifi_scan.c
    user/wifi/wifi_sta.c
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
    SRCS         ${SRCS}
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
