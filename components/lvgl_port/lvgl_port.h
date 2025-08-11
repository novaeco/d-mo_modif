#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_types.h"
#include "touch.h"
#include "lvgl.h"

/* ----------------------------------------------
 *     Paramétrage carte Waveshare 7B 1024x600
 * ---------------------------------------------- */
#ifndef LVGL_PORT_H_RES
#  define LVGL_PORT_H_RES             (1024)
#endif
#ifndef LVGL_PORT_V_RES
#  define LVGL_PORT_V_RES             (600)
#endif

/* Tick LVGL */
#ifndef LVGL_PORT_TICK_PERIOD_MS
#  define LVGL_PORT_TICK_PERIOD_MS    (2)
#endif

/* Tâche LVGL */
#ifndef LVGL_PORT_TASK_STACK_SIZE
#  define LVGL_PORT_TASK_STACK_SIZE   (10 * 1024)
#endif
#ifndef LVGL_PORT_TASK_PRIORITY
#  define LVGL_PORT_TASK_PRIORITY     (3)
#endif
#ifndef LVGL_PORT_TASK_CORE
#  define LVGL_PORT_TASK_CORE         (-1)    /* libre / no affinity */
#endif
#ifndef LVGL_PORT_TASK_MIN_DELAY_MS
#  define LVGL_PORT_TASK_MIN_DELAY_MS (4)
#endif
#ifndef LVGL_PORT_TASK_MAX_DELAY_MS
#  define LVGL_PORT_TASK_MAX_DELAY_MS (33)
#endif

/* ----------------------------------------------
 *          Modes de rendu LVGL 9
 * ---------------------------------------------- */
/* Anti-tearing : attendre VSYNC avant flush_ready */
#ifndef LVGL_PORT_AVOID_TEAR_ENABLE
#  define LVGL_PORT_AVOID_TEAR_ENABLE (1)
#endif

/* Choix du mode de rendu LVGL:
 *  - DIRECT  : LVGL dessine et on “pousse” directement (fb partiel)
 *  - FULL    : double/triple buffering plein écran
 *  - PARTIAL : bandes (moins de RAM)
 *
 * Note: pour un RGB panel, FULL (double buffer) est souvent le meilleur compromis.
 */
#ifndef LVGL_PORT_RENDER_MODE_FULL
#  define LVGL_PORT_RENDER_MODE_FULL  (1)
#endif
#ifndef LVGL_PORT_RENDER_MODE_DIRECT
#  define LVGL_PORT_RENDER_MODE_DIRECT (0)
#endif
#ifndef LVGL_PORT_RENDER_MODE_PARTIAL
#  define LVGL_PORT_RENDER_MODE_PARTIAL (0)
#endif

/* Double / Triple buffering (avec FULL) */
#ifndef LVGL_PORT_NUM_BUFFERS
#  define LVGL_PORT_NUM_BUFFERS       (2)   /* mettre 3 pour triple buffer */
#endif

/* Allocation buffers (PSRAM conseillé) */
#ifndef LVGL_PORT_USE_PSRAM
#  define LVGL_PORT_USE_PSRAM         (1)
#endif

#if LVGL_PORT_USE_PSRAM
#  define LVGL_PORT_MALLOC_CAPS  (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)
#else
#  define LVGL_PORT_MALLOC_CAPS  (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#endif

/* Hauteur du “draw buffer” si PARTIAL */
#ifndef LVGL_PORT_BUFFER_HEIGHT
#  define LVGL_PORT_BUFFER_HEIGHT     (120)
#endif

/* Rotation (0/90/180/270). On fait la rotation logique côté LVGL et on remappe le tactile. */
#ifndef LVGL_PORT_ROTATION_DEGREE
#  define LVGL_PORT_ROTATION_DEGREE   (0)
#endif

/* ----------------------------------------------
 *     Hooks “board” facultatifs (CH422G/backlight)
 *  - Déclarés weak ici ; vous pouvez les définir
 *    dans un autre fichier pour piloter réellement
 *    le backlight, VCOM, etc.
 * ---------------------------------------------- */
__attribute__((weak)) void board_lcd_power_enable(bool on);
__attribute__((weak)) void board_lcd_backlight_set(bool on);
__attribute__((weak)) void board_lcd_backlight_set_level(uint8_t percent); /* 0..100 */

/* API publique */
#ifdef __cplusplus
extern "C" {
#endif

esp_err_t lvgl_port_init(esp_lcd_panel_handle_t lcd_handle, esp_lcd_touch_handle_t tp_handle);

bool lvgl_port_lock(int timeout_ms);
void lvgl_port_unlock(void);

/* À appeler depuis l’ISR VSYNC du panneau si vous câbler l’event RGB */
bool lvgl_port_notify_rgb_vsync(void);

#ifdef __cplusplus
}
#endif
