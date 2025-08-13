#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

static const char *TAG = "WIFI_TEST";

#define WIFI_AP_STARTED_BIT   BIT0
#define WIFI_STA_CONNECTED_BIT BIT1
#define WIFI_STA_GOT_IP_BIT    BIT2

static EventGroupHandle_t wifi_event_group;

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_AP_START) {
            xEventGroupSetBits(wifi_event_group, WIFI_AP_STARTED_BIT);
        } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
            xEventGroupSetBits(wifi_event_group, WIFI_STA_CONNECTED_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_STA_GOT_IP_BIT);
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_event_group = xEventGroupCreate();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_event_post(WIFI_EVENT, WIFI_EVENT_AP_START, NULL, 0, portMAX_DELAY));
    ESP_ERROR_CHECK(esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, NULL, 0, portMAX_DELAY));
    ESP_ERROR_CHECK(esp_event_post(IP_EVENT, IP_EVENT_STA_GOT_IP, NULL, 0, portMAX_DELAY));

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_AP_STARTED_BIT | WIFI_STA_CONNECTED_BIT | WIFI_STA_GOT_IP_BIT,
                                           pdTRUE, pdTRUE, pdMS_TO_TICKS(1000));

    if ((bits & (WIFI_AP_STARTED_BIT | WIFI_STA_CONNECTED_BIT | WIFI_STA_GOT_IP_BIT)) ==
        (WIFI_AP_STARTED_BIT | WIFI_STA_CONNECTED_BIT | WIFI_STA_GOT_IP_BIT)) {
        ESP_LOGI(TAG, "Wi-Fi AP/STA simulation succeeded");
    } else {
        ESP_LOGE(TAG, "Wi-Fi AP/STA simulation failed");
    }

    ESP_ERROR_CHECK(esp_wifi_stop());
}
