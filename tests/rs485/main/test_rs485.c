#include "driver/uart.h"
#include "esp_log.h"

static const char *TAG = "RS485_TEST";

void app_main(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_set_mode(UART_NUM_1, UART_MODE_RS485_HALF_DUPLEX));
    ESP_LOGI(TAG, "RS485 init test OK");
    uart_driver_delete(UART_NUM_1);
}
