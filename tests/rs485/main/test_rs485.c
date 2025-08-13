#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>

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
    ESP_ERROR_CHECK(uart_set_loop_back(UART_NUM_1, true));

    const char *test_str = "rs485 loop";
    uint8_t rx_buf[32] = {0};
    ESP_ERROR_CHECK(uart_write_bytes(UART_NUM_1, test_str, strlen(test_str)));
    ESP_ERROR_CHECK(uart_wait_tx_done(UART_NUM_1, pdMS_TO_TICKS(100)));
    int len = uart_read_bytes(UART_NUM_1, rx_buf, sizeof(rx_buf), pdMS_TO_TICKS(100));

    if (len == (int)strlen(test_str) && memcmp(rx_buf, test_str, len) == 0) {
        ESP_LOGI(TAG, "RS485 loopback OK");
    } else {
        ESP_LOGE(TAG, "RS485 loopback failed");
    }

    uart_driver_delete(UART_NUM_1);
}
