#include <string.h>

#include "rs485.h"
#include "ui.h"

static const char* TAG = "RS485"; // Tag used for ESP log output

TaskHandle_t rs485_TaskHandle;

#define RS485_TEMP_BUF_SIZE 256 // Maximum temporary buffer size

// Allocate a buffer to store incoming UART data
static char data[BUF_SIZE] = {0}; // Buffer for receiving data

/**
 * @brief Callback function to update the text area with the received data.
 *
 * This function is called periodically by a timer to update the text area
 * with the latest data stored in the global buffer. If the RS485_Clear flag is set,
 * it clears the buffer before updating the text area.
 *
 * @param timer Pointer to the lv_timer_t structure.
 * @return None
 */
void rs485_update_textarea_cb(lv_timer_t* timer) {
  // If the RS485_Clear flag is set, clear the data buffer
  if (RS485_Clear) {
    memset(data, 0, sizeof(data)); // Clear the data buffer
    RS485_Clear = false;           // Reset the flag
  }

  // Update the text area with the received data
  lv_textarea_set_text(ui_RS485_Read_Area, data);
}

/**
 * @brief RS485 communication task.
 *
 * This function is responsible for initializing UART communication for RS485,
 * reading incoming data, and updating the UI with the received data. The data is
 * appended to a global buffer and displayed in a text area on the screen.
 *
 * It also periodically checks if the RS485_Clear flag is set, in which case it
 * clears the data buffer and updates
 * the UI. Incoming messages larger than
 * RS485_TEMP_BUF_SIZE - 1 bytes are truncated to fit the temporary buffer.
 *
 * @param arg Pointer to the argument passed to the task (unused).
 * @return None
 */
void rs485_task(void* arg) {
  // Initialize UART communication for RS485 with specified TX, RX pins and baud rate
  DEV_UART_Init(ECHO_TEST_TXD, ECHO_TEST_RXD, RS485_BaudRate);

  while (1) {
    // Get the number of bytes available in the UART receive buffer
    int len = UART_Get_Date_Len();
    if (len > 0) {
      char temp_buf[RS485_TEMP_BUF_SIZE] = {0}; // Temporary buffer for received data
      if (len >= RS485_TEMP_BUF_SIZE) {
        ESP_LOGW(TAG, "UART message too long (%d bytes), truncating to %d", len, RS485_TEMP_BUF_SIZE - 1);
        len = RS485_TEMP_BUF_SIZE - 1;
      }

      UART_Read_Byte((uint8_t*)temp_buf, len);
      temp_buf[len] = '\0'; // Null-terminate the received data

      if (strlcat(data, temp_buf, sizeof(data)) >= sizeof(data)) {
        ESP_LOGW(TAG, "RS485 buffer truncated");
      }

      ESP_LOGI(TAG, "UART_Read_Byte:%s", data);

      lv_timer_t* t = lv_timer_create(rs485_update_textarea_cb, 100, NULL);
      lv_timer_set_repeat_count(t, 1);
    }

    // If the RS485_Clear flag is set, clear the data buffer and update the UI
    if (RS485_Clear) {
      lv_timer_t* t = lv_timer_create(rs485_update_textarea_cb, 100, NULL); // Update the UI every 100ms
      lv_timer_set_repeat_count(t, 1);
    }

    // Delay for 10ms before checking again
    vTaskDelay(10);
  }
}
