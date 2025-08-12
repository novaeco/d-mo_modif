/******************************************************************************
 * | File         :   rgb_lcd_port.c
 * | Author      :   Waveshare team (edited)
 * | Function    :   Hardware underlying interface
 * | Info        :   RGB LCD driver code for ST7262-based 1024x600 panel
 *----------------
 * |This version :   V1.2
 * | Date        :   2025-08-12
 * | Changes     :   - Add explicit HSYNC/VSYNC/DE polarity flags (typical ST7262)
 *                   - No functional renames; only minimal, necessary fixes
 *
 ******************************************************************************/

#include "rgb_lcd_port.h"
#include "lvgl_port.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "rgb_lcd_port";

// Handle for the RGB LCD panel
static esp_lcd_panel_handle_t panel_handle = NULL;

// VSYNC event callback function
IRAM_ATTR static bool rgb_lcd_on_vsync_event(esp_lcd_panel_handle_t panel,
                                             const esp_lcd_rgb_panel_event_data_t *edata,
                                             void *user_ctx)
{
    (void)panel;
    (void)edata;
    (void)user_ctx;
    return lvgl_port_notify_rgb_vsync();
}

/**
 * @brief Initialize the RGB LCD panel on the ESP32-S3
 *
 * This function configures and initializes an RGB LCD panel driver
 * using the ESP-IDF RGB LCD driver API. It sets up timing parameters,
 * GPIOs, data width, and framebuffer settings for the LCD panel.
 *
 * @return
 *    - esp_lcd_panel_handle_t: panel handle on success
 *    - NULL: Initialization failed.
 */
esp_lcd_panel_handle_t waveshare_esp32_s3_rgb_lcd_init(void)
{
    ESP_LOGI(TAG, "Install RGB LCD panel driver");

    /* Typical polarities for ST7262 1024x600 TTL panel:
     *  - DE: active HIGH
     *  - HSYNC/VSYNC: active HIGH (idle LOW)
     *  - PCLK: sample on rising edge (active_neg = 0)
     * If your glass requires the opposite, only change the flags below.
     */
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .timings = {
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .h_res = EXAMPLE_LCD_H_RES,
            .v_res = EXAMPLE_LCD_V_RES,
            .hsync_pulse_width = 162,
            .hsync_back_porch  = 152,
            .hsync_front_porch = 48,
            .vsync_pulse_width = 45,
            .vsync_back_porch  = 13,
            .vsync_front_porch = 3,
            .flags = {
                .pclk_active_neg = 0,   // sample on rising edge (fixable here if needed)
                .hsync_idle_low  = 1,   // HSYNC idle LOW → active HIGH
                .vsync_idle_low  = 1,   // VSYNC idle LOW → active HIGH
                .de_idle_high    = 1,   // DE active HIGH
                .pclk_idle_high  = 0,
            },
        },
        .data_width = EXAMPLE_RGB_DATA_WIDTH,
        .bits_per_pixel = EXAMPLE_RGB_BIT_PER_PIXEL,
        .num_fbs = EXAMPLE_LCD_RGB_BUFFER_NUMS,
        .bounce_buffer_size_px = EXAMPLE_RGB_BOUNCE_BUFFER_SIZE,
        .sram_trans_align = 4,
        .psram_trans_align = 64,
        .hsync_gpio_num = EXAMPLE_LCD_IO_RGB_HSYNC,
        .vsync_gpio_num = EXAMPLE_LCD_IO_RGB_VSYNC,
        .de_gpio_num    = EXAMPLE_LCD_IO_RGB_DE,
        .pclk_gpio_num  = EXAMPLE_LCD_IO_RGB_PCLK,
        .disp_gpio_num  = EXAMPLE_LCD_IO_RGB_DISP,
        .data_gpio_nums = {
            EXAMPLE_LCD_IO_RGB_DATA0,  EXAMPLE_LCD_IO_RGB_DATA1,
            EXAMPLE_LCD_IO_RGB_DATA2,  EXAMPLE_LCD_IO_RGB_DATA3,
            EXAMPLE_LCD_IO_RGB_DATA4,  EXAMPLE_LCD_IO_RGB_DATA5,
            EXAMPLE_LCD_IO_RGB_DATA6,  EXAMPLE_LCD_IO_RGB_DATA7,
            EXAMPLE_LCD_IO_RGB_DATA8,  EXAMPLE_LCD_IO_RGB_DATA9,
            EXAMPLE_LCD_IO_RGB_DATA10, EXAMPLE_LCD_IO_RGB_DATA11,
            EXAMPLE_LCD_IO_RGB_DATA12, EXAMPLE_LCD_IO_RGB_DATA13,
            EXAMPLE_LCD_IO_RGB_DATA14, EXAMPLE_LCD_IO_RGB_DATA15,
        },
        .flags = {
            .fb_in_psram = 1,
        },
    };

    if (esp_lcd_new_rgb_panel(&panel_config, &panel_handle) != ESP_OK) {
        ESP_LOGE(TAG, "esp_lcd_new_rgb_panel failed");
        return NULL;
    }

    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    if (esp_lcd_panel_init(panel_handle) != ESP_OK) {
        ESP_LOGE(TAG, "esp_lcd_panel_init failed");
        return NULL;
    }

    esp_lcd_rgb_panel_event_callbacks_t cbs = {
    #if EXAMPLE_RGB_BOUNCE_BUFFER_SIZE > 0
        .on_bounce_frame_finish = rgb_lcd_on_vsync_event,
    #else
        .on_vsync = rgb_lcd_on_vsync_event,
    #endif
    };
    (void)esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, NULL);

    return panel_handle;
}

/**
 * @brief Display a specific window of an image on the RGB LCD.
 *
 * @param Xstart Starting X coordinate (inclusive).
 * @param Ystart Starting Y coordinate (inclusive).
 * @param Xend   Ending X coordinate (exclusive, absolute).
 * @param Yend   Ending Y coordinate (exclusive, absolute).
 * @param Image  Pointer to the image data buffer for full LCD resolution.
 */
void wavesahre_rgb_lcd_display_window(int16_t Xstart, int16_t Ystart,
                                      int16_t Xend, int16_t Yend, uint8_t *Image)
{
    if (Xstart < 0) Xstart = 0;
    if (Ystart < 0) Ystart = 0;
    if (Xend > EXAMPLE_LCD_H_RES) Xend = EXAMPLE_LCD_H_RES;
    if (Yend > EXAMPLE_LCD_V_RES) Yend = EXAMPLE_LCD_V_RES;
    if (Xend <= Xstart || Yend <= Ystart) return;

    const int crop_width  = (int)(Xend - Xstart);
    const int crop_height = (int)(Yend - Ystart);

    // Allocate temporary crop buffer (2 bytes per pixel, RGB565)
    uint8_t *dst_data = (uint8_t *)malloc((size_t)crop_width * crop_height * 2);
    if (!dst_data) {
        ESP_LOGE(TAG, "Failed to allocate crop buffer (%d x %d)", crop_width, crop_height);
        return;
    }

    // Copy rows
    for (int y = 0; y < crop_height; y++) {
        const uint8_t *src_row = Image + ((size_t)(Ystart + y) * EXAMPLE_LCD_H_RES + Xstart) * 2;
        uint8_t *dst_row       = dst_data + (size_t)y * crop_width * 2;
        memcpy(dst_row, src_row, (size_t)crop_width * 2);
    }

    esp_lcd_panel_draw_bitmap(panel_handle, Xstart, Ystart, Xend, Yend, dst_data);
    free(dst_data);
}

/**
 * @brief Display a full-screen image on the RGB LCD.
 */
void wavesahre_rgb_lcd_display(uint8_t *Image)
{
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, Image);
}

void waveshare_get_frame_buffer(void **buf1, void **buf2)
{
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, buf1, buf2));
}

void waveahre_rgb_lcd_set_pclk(uint32_t freq_hz)
{
    esp_lcd_rgb_panel_set_pclk(panel_handle, freq_hz);
}

void waveshare_rgb_lcd_restart(void)
{
    esp_lcd_rgb_panel_restart(panel_handle);
}

/**
 * @brief Turn on the RGB LCD screen backlight.
 */
void wavesahre_rgb_lcd_bl_on(void)
{
    IO_EXTENSION_Output(IO_EXTENSION_IO_2, 1);
}

/**
 * @brief Turn off the RGB LCD screen backlight.
 */
void wavesahre_rgb_lcd_bl_off(void)
{
    IO_EXTENSION_Output(IO_EXTENSION_IO_2, 0);
}
