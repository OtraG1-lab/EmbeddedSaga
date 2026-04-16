
// Esp in station mode or Access Point mode
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
//#define MQTT_BROKER_URI "mqtt://192.168.29.53:1883"

static const char *TAG = "MQTT_EXAMPLE";
static esp_mqtt_client_handle_t client = NULL;
static bool mqtt_connected = false;
static bool wifi_connected = false;

// ===================== MQTT Event Handler =====================
static void mqtt_event_handler_cb(void *handler_args,
                                  esp_event_base_t base,
                                  int32_t event_id,
                                  void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            mqtt_connected = true;
            esp_mqtt_client_subscribe(client, "test/topic", 0);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Received: topic=%.*s, data=%.*s",
                     event->topic_len, event->topic,
                     event->data_len, event->data);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT Disconnected");
            mqtt_connected = false;
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT Error: %d", event->error_handle->error_type);
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                ESP_LOGE(TAG, "TCP transport error: %d", event->error_handle->esp_transport_sock_errno);
            }
            break;
        default:
            break;
    }
}

// ===================== Wi-Fi Event Handler =====================
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Wi-Fi Disconnected. Reconnecting...");
        wifi_connected = false;
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        wifi_connected = true;
    }
}

// ===================== Initialize Wi-Fi =====================
void wifi_init(void) {
    ESP_LOGI(TAG, "Initializing Wi-Fi...");

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
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi initialization finished.");
}

// ===================== Initialize MQTT =====================
void mqtt_app_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.29.53:1883",
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler_cb, NULL);
    esp_mqtt_client_start(client);
}

// ===================== Publish Task =====================
void publish_task(void* pvParameters) {
    int count = 0;
    vTaskDelay(pdMS_TO_TICKS(2000)); // Wait for MQTT to be connected

    while (1) {
        if (mqtt_connected) {
            char msg[50];
            snprintf(msg, sizeof(msg), "Hello ESP32 #%d", count++);
            int msg_id = esp_mqtt_client_publish(client, "test/topic", msg, 0, 1, 0);
            if (msg_id > 0) {
                ESP_LOGI(TAG, "Published message: %s (id=%d)", msg, msg_id);
            } else {
                ESP_LOGE(TAG, "Failed to publish message");
            }
        } else {
            ESP_LOGW(TAG, "MQTT not connected, waiting...");
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); // Publish every 5 seconds
    }
}

// ===================== Main App =====================
void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init();

    ESP_LOGI(TAG, "Waiting for WiFi connection...");
    int timeout = 0;
    while (!wifi_connected && timeout < 30) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        timeout++;
    }

    if (wifi_connected) {
        ESP_LOGI(TAG, "WiFi connected, starting MQTT...");
        mqtt_app_start();
    } else {
        ESP_LOGE(TAG, "WiFi connection timeout!");
    }

    xTaskCreate(publish_task, "publish_task", 4096, NULL, 5, NULL);
}
