#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "lvgl_port.h"

static const char *TAG = "lv_port";

/* ---------- Contexte global ---------- */
static SemaphoreHandle_t s_lvgl_mutex;
static TaskHandle_t      s_lvgl_task = NULL;
#if LVGL_PORT_AVOID_TEAR_ENABLE
static SemaphoreHandle_t s_vsync_sem;
#endif

/* ---------- Tick LVGL via esp_timer ---------- */
static void lv_tick_cb(void *arg) {
    (void)arg;
    lv_tick_inc(LVGL_PORT_TICK_PERIOD_MS);
}
static esp_err_t lv_tick_init(void) {
    const esp_timer_create_args_t args = {
        .callback = &lv_tick_cb,
        .name = "lv_tick"
    };
    esp_timer_handle_t t = NULL;
    ESP_RETURN_ON_ERROR(esp_timer_create(&args, &t), TAG, "esp_timer_create");
    return esp_timer_start_periodic(t, LVGL_PORT_TICK_PERIOD_MS * 1000);
}

/* ---------- Anti-tearing : VSYNC notify ---------- */
bool lvgl_port_notify_rgb_vsync(void)
{
#if LVGL_PORT_AVOID_TEAR_ENABLE
    BaseType_t woken = pdFALSE;
    if (s_vsync_sem) xSemaphoreGiveFromISR(s_vsync_sem, &woken);
    return (woken == pdTRUE);
#else
    return false;
#endif
}

/* ---------- Flush LVGL v9 ----------
 * Signature: void (*flush_cb)(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
 * - disp   : handle LVGL
 * - area   : rectangle à pousser
 * - px_map : pixels LVGL (format = color_format du display)
 */
static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel =
        (esp_lcd_panel_handle_t) lv_display_get_user_data(disp);

    const int x1 = area->x1;
    const int y1 = area->y1;
    const int x2 = area->x2;
    const int y2 = area->y2;

    /* Envoi zone -> panneau RGB */
    esp_lcd_panel_draw_bitmap(panel, x1, y1, x2 + 1, y2 + 1, (const void *)px_map);

#if LVGL_PORT_AVOID_TEAR_ENABLE
    /* Attendre le VSYNC pour éviter le tearing (timeout ~ 1 frame) */
    if (s_vsync_sem) {
        const TickType_t to = pdMS_TO_TICKS(20); /* ~50-60Hz */
        (void)xSemaphoreTake(s_vsync_sem, to);
    }
#endif
    lv_display_flush_ready(disp);
}

/* ---------- Touch (LVGL v9) ---------- */
static void touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    esp_lcd_touch_handle_t tp =
        (esp_lcd_touch_handle_t) lv_indev_get_user_data(indev);

    uint16_t x, y;
    uint8_t  cnt = 0;

    esp_lcd_touch_read_data(tp);
    bool pressed = esp_lcd_touch_get_coordinates(tp, &x, &y, NULL, &cnt, 1);

#if (LVGL_PORT_ROTATION_DEGREE == 90)
    uint16_t tx = x, ty = y;
    x = LVGL_PORT_V_RES - 1 - ty;
    y = tx;
#elif (LVGL_PORT_ROTATION_DEGREE == 180)
    x = LVGL_PORT_H_RES - 1 - x;
    y = LVGL_PORT_V_RES - 1 - y;
#elif (LVGL_PORT_ROTATION_DEGREE == 270)
    uint16_t tx2 = x, ty2 = y;
    x = ty2;
    y = LVGL_PORT_H_RES - 1 - tx2;
#endif

    if (pressed && cnt > 0) {
        data->point.x = x;
        data->point.y = y;
        data->state   = LV_INDEV_STATE_PRESSED;
    } else {
        data->state   = LV_INDEV_STATE_RELEASED;
    }
}

/* Try to allocate display buffers with graceful fallback:
   1) FULL double-buffer (2x full screen) in PSRAM
   2) PARTIAL band buffer (H_RES x 60)
   3) DIRECT small scratch buffer (H_RES x 20)
*/
static bool setup_display_buffers_with_fallback(lv_display_t *disp)
{
    const int32_t W = LVGL_PORT_H_RES;
    const int32_t H = LVGL_PORT_V_RES;

    /* --- Try FULL double buffer --- */
    {
        const size_t fb_bytes = (size_t)W * H * sizeof(lv_color_t);
        void *buf0 = heap_caps_malloc(fb_bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        void *buf1 = heap_caps_malloc(fb_bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (buf0 && buf1) {
            lv_display_set_buffers(disp, buf0, buf1, fb_bytes, LV_DISPLAY_RENDER_MODE_FULL);
            ESP_LOGI(TAG, "LVGL buffers: FULL x2 in PSRAM (%.2f MB)", (2.0 * fb_bytes) / (1024.0 * 1024.0));
            return true;
        }
        if (buf0) free(buf0);
        if (buf1) free(buf1);
        ESP_LOGW(TAG, "FULL x2 buffers alloc failed -> fallback");
    }

    /* --- Try PARTIAL band --- */
    {
        const int band_h = 60; /* lines */
        const size_t buf_bytes = (size_t)W * band_h * sizeof(lv_color_t);
        void *buf = heap_caps_malloc(buf_bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (buf) {
            lv_display_set_buffers(disp, buf, NULL, buf_bytes, LV_DISPLAY_RENDER_MODE_PARTIAL);
            ESP_LOGI(TAG, "LVGL buffers: PARTIAL %dx%d in PSRAM (~%.1f KB)",
                     W, band_h, buf_bytes / 1024.0);
            return true;
        }
        ESP_LOGW(TAG, "PARTIAL buffer alloc failed -> fallback");
    }

    /* --- Try DIRECT scratch --- */
    {
        const int scratch_h = 20; /* lines */
        const size_t buf_bytes = (size_t)W * scratch_h * sizeof(lv_color_t);
        void *buf = heap_caps_malloc(buf_bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (buf) {
            lv_display_set_buffers(disp, buf, NULL, buf_bytes, LV_DISPLAY_RENDER_MODE_DIRECT);
            ESP_LOGI(TAG, "LVGL buffers: DIRECT scratch %dx%d in PSRAM (~%.1f KB)",
                     W, scratch_h, buf_bytes / 1024.0);
            return true;
        }
        ESP_LOGE(TAG, "DIRECT scratch alloc failed");
    }

    return false;
}

/* ---------- Création display LVGL v9 ---------- */
static lv_display_t *display_create(esp_lcd_panel_handle_t panel)
{
    /* 1) Créer l’affichage LVGL (w,h) */
    lv_display_t *disp = lv_display_create(LVGL_PORT_H_RES, LVGL_PORT_V_RES);
    if (!disp) return NULL;

    /* 2) User data : on stocke le panel handle pour flush_cb */
    lv_display_set_user_data(disp, panel);

    /* 3) Format couleur (RGB565 pour esp_lcd RGB) */
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    /* 4) Rotation logique LVGL */
#if (LVGL_PORT_ROTATION_DEGREE == 90)
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);
#elif (LVGL_PORT_ROTATION_DEGREE == 180)
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_180);
#elif (LVGL_PORT_ROTATION_DEGREE == 270)
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);
#else
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
#endif

    /* 5) Buffers de rendu (avec fallback mémoire) */
    if (!setup_display_buffers_with_fallback(disp)) {
        ESP_LOGE(TAG, "malloc LVGL buffers failed");
        return NULL;
    }

    /* 6) Flush callback */
    lv_display_set_flush_cb(disp, flush_cb);

    return disp;
}

/* ---------- Création input LVGL v9 ---------- */
static lv_indev_t *indev_create(esp_lcd_touch_handle_t tp)
{
    if (!tp) return NULL;
    lv_indev_t *indev = lv_indev_create();
    if (!indev) return NULL;

    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_read_cb);
    lv_indev_set_user_data(indev, tp);
    return indev;
}

/* ---------- Tâche LVGL ---------- */
static void lvgl_task(void *arg)
{
    (void)arg;
    uint32_t delay_ms = LVGL_PORT_TASK_MAX_DELAY_MS;

    while (true) {
        if (lvgl_port_lock(-1)) {
            delay_ms = lv_timer_handler();
            lvgl_port_unlock();
        }
        if (delay_ms > LVGL_PORT_TASK_MAX_DELAY_MS) delay_ms = LVGL_PORT_TASK_MAX_DELAY_MS;
        if (delay_ms < LVGL_PORT_TASK_MIN_DELAY_MS) delay_ms = LVGL_PORT_TASK_MIN_DELAY_MS;
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

/* ---------- API publique ---------- */
esp_err_t lvgl_port_init(esp_lcd_panel_handle_t lcd_handle, esp_lcd_touch_handle_t tp_handle)
{
    ESP_RETURN_ON_FALSE(lcd_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "lcd_handle is NULL");

    /* Alims / backlight via hooks “weak” */
    board_lcd_power_enable(true);
    board_lcd_backlight_set(true);
    board_lcd_backlight_set_level(100);

    lv_init();
    ESP_RETURN_ON_ERROR(lv_tick_init(), TAG, "tick init");

#if LVGL_PORT_AVOID_TEAR_ENABLE
    s_vsync_sem = xSemaphoreCreateBinary();
    if (!s_vsync_sem) return ESP_ERR_NO_MEM;
#endif

    lv_display_t *disp = display_create(lcd_handle);
    if (!disp) return ESP_FAIL;

    if (tp_handle) {
        lv_indev_t *indev = indev_create(tp_handle);
        if (!indev) return ESP_FAIL;
        (void)indev;
        /* Optionnel: calibration logicielle si besoin (non incluse ici) */
    }

    s_lvgl_mutex = xSemaphoreCreateRecursiveMutex();
    if (!s_lvgl_mutex) return ESP_ERR_NO_MEM;

    const BaseType_t core = (LVGL_PORT_TASK_CORE < 0) ? tskNO_AFFINITY : LVGL_PORT_TASK_CORE;
    if (xTaskCreatePinnedToCore(lvgl_task, "lvgl", LVGL_PORT_TASK_STACK_SIZE, NULL,
                                LVGL_PORT_TASK_PRIORITY, &s_lvgl_task, core) != pdPASS) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "LVGL port ready: %dx%d, buffers set (fallback-safe), tear_protect=%s",
             LVGL_PORT_H_RES, LVGL_PORT_V_RES,
             LVGL_PORT_AVOID_TEAR_ENABLE ? "on" : "off");

    return ESP_OK;
}

bool lvgl_port_lock(int timeout_ms)
{
    configASSERT(s_lvgl_mutex && "lvgl_port_init() d'abord");
    TickType_t to = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTakeRecursive(s_lvgl_mutex, to) == pdTRUE;
}

void lvgl_port_unlock(void)
{
    configASSERT(s_lvgl_mutex && "lvgl_port_init() d'abord");
    xSemaphoreGiveRecursive(s_lvgl_mutex);
}

/* ---------- Hooks weak par défaut (NOP) ---------- */
void __attribute__((weak)) board_lcd_power_enable(bool on)      { (void)on; }
void __attribute__((weak)) board_lcd_backlight_set(bool on)     { (void)on; }
void __attribute__((weak)) board_lcd_backlight_set_level(uint8_t percent) { (void)percent; }
