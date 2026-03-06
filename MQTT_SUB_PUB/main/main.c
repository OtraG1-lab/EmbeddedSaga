
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_client.h"

#define WIFI_SSID "DEXTRIS 2,4 JIO"
#define WIFI_PASS "Dextris@123"
#define MQTT_BROKER_URI "mqtt://192.168.29.53:1883"

static const char *TAG = "MQTT_TEST";
static esp_mqtt_client_handle_t client = NULL;

// ---------------- MQTT Event Handler ----------------
static void mqtt_event_handler_cb(void *handler_args,
                                  esp_event_base_t base,
                                  int32_t event_id,
                                  void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            esp_mqtt_client_subscribe(client, "test/topic", 0);
            esp_mqtt_client_publish(client, "test/topic", "Hello from ESP32", 0, 0, 0);
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "Subscribed successfully");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Received: %.*s", event->data_len, event->data);
            break;
        default:
            break;
    }
}

// ---------------- MQTT Start Function ----------------
void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .session.protocol_ver = MQTT_PROTOCOL_V_3_1_1,   // force MQTT v3.1.1
        // To test MQTT v5.0, change to: MQTT_PROTOCOL_V_5
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_LOGI(TAG, "MQTT client configured with protocol version %s",
             (mqtt_cfg.session.protocol_ver == MQTT_PROTOCOL_V_3_1_1) ? "3.1.1" : "5.0");
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler_cb, NULL);
    esp_mqtt_client_start(client);
}

// ---------------- Wi-Fi Event Handler ----------------
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Wi-Fi Disconnected. Reconnecting...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
     {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));

        // ✅ Start MQTT only after IP is assigned
        mqtt_app_start();
    }
}

// ---------------- Wi-Fi Init ----------------
void wifi_init(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi initialization finished.");
}

// ---------------- Main ----------------
void app_main(void) 
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    // ❌ Removed vTaskDelay and mqtt_app_start() here
}
