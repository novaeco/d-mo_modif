#include "driver/twai.h"
#include "esp_log.h"

static const char *TAG = "CAN_TEST";

void app_main(void) {
    const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    const twai_general_config_t g_config =
        TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NO_ACK);
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(TAG, "TWAI driver installed");
        twai_driver_uninstall();
    } else {
        ESP_LOGE(TAG, "TWAI install failed");
    }
}
