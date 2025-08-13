#include "esp_log.h"        // ESP32 logging library for debugging and error logging
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ui.h"
#include "pwm.h"

static const char *TAG = "PWM";  // Tag used for ESP log output
TaskHandle_t pwm_TaskHandle;     // Handle for the PWM task
char bat_v[20];                  // Buffer to store formatted battery voltage string
static lv_timer_t *bat_timer;    // Timer to refresh battery voltage label

/**
 * @brief Callback function to update the UI with battery voltage information.
 * 
 * This function is called by an LVGL timer to update the UI label with the formatted battery voltage string.
 * 
 * @param timer Pointer to the LVGL timer object
 */
static void bat_cb(lv_timer_t * timer)
{
    lv_label_set_text(ui_BAT_Label0, bat_v); // Update the UI label with the battery voltage string
    lv_timer_pause(timer);                   // Run once then pause until reset
}

/**
 * @brief Task function to periodically measure and update the battery voltage.
 * 
 * This function continuously measures the battery voltage using an ADC, averages the readings,
 * formats the voltage string, and updates the UI label.
 * 
 * @param arg Pointer to the task parameter (not used in this implementation)
 */
void pwm_task(void *arg)
{
    float value = 0; // Variable to store the ADC reading
    int i;

    while (1)
    {
        // Take 10 ADC readings and average them to reduce noise
        for (i = 0; i < 10; i++)
        {
            value += IO_EXTENSION_Adc_Input(); // Read the ADC input
            vTaskDelay(20); // Delay between readings
        }
        value /= 10.0; // Compute average

        // Convert ADC value to voltage (assuming 10-bit ADC, 3.3V reference, and 3:1 voltage divider)
        value *= 3 * 3.3 / 1023.0;

        // Clamp voltage to max 4.2V for safe display
        if (value > 4.2) {
            value = 4.2;
        }

        // Format battery voltage string for display
        sprintf(bat_v, "BAT:%0.2fV", value);
        // Trigger the LVGL timer to update the UI label without recreating it
        lv_timer_reset(bat_timer);
        lv_timer_resume(bat_timer);

        value = 0; // Reset the value for the next cycle

        vTaskDelay(100); // Delay before the next measurement cycle
    }

    // Clean up timer if the task is ever terminated
    lv_timer_del(bat_timer);
}

/**
 * @brief Initialize the PWM module and start the battery voltage monitoring task.
 * 
 * This function initializes the PWM module with a 1 kHz frequency, sets the initial duty cycle,
 * and starts a FreeRTOS task to periodically measure and update the battery voltage.
 */
void pwm_init()
{
    ESP_LOGI(TAG, "Init 1000Hz PWM "); // Log the PWM initialization

    // Initialize the GPIO pin for PWM with a 1 kHz frequency
    DEV_GPIO_PWM(LED_GPIO_PIN, 1000); // Set up the PWM on the specified GPIO pin

    // Create a single LVGL timer for battery updates and keep it paused until needed
    bat_timer = lv_timer_create(bat_cb, 100, NULL);
    lv_timer_pause(bat_timer);

    // Start the battery voltage monitoring task
    xTaskCreate(pwm_task, "pwm_task", 3 * 1024, NULL, 3, &pwm_TaskHandle);

    // Set the initial PWM duty cycle to 50% (assuming active-low configuration)
    IO_EXTENSION_Pwm_Output(50); // Set the PWM duty cycle
}
