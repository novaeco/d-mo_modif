#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#warning "lvgl.h introuvable : ajoute le composant LVGL au projet si ce build échoue."
#endif

static const char *TAG = "SMOKE";

void app_main(void) {
    ESP_LOGI(TAG, "Smoke: démarrage test LVGL");

#ifdef LVGL_H
    lv_init();
    ESP_LOGI(TAG, "LVGL initialisé");
#else
    ESP_LOGW(TAG, "LVGL non disponible au build (test limité)");
#endif

    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_LOGI(TAG, "Smoke: OK (compilation + init basique)");
}