
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
#define MQTT_BROKER_URI "mqtt://192.168.29.53:1883"   // replace with your broker IP

static const char *TAG = "MQTT_EXAMPLE";
static esp_mqtt_client_handle_t client = NULL;

// ===================== MQTT Event Handler =====================
static void mqtt_event_handler_cb(void *handler_args,
                                  esp_event_base_t base,
                                  int32_t event_id,
                                  void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            esp_mqtt_client_subscribe(client, "test/topic", 0);
            esp_mqtt_client_subscribe(client, "sensor/topic", 0);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Received: topic=%.*s, data=%.*s",
                     event->topic_len, event->topic,
                     event->data_len, event->data);
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
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
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
        .broker.address.uri = MQTT_BROKER_URI,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler_cb, NULL);
    esp_mqtt_client_start(client);
}

// ===================== Task 1: Publisher =====================
void publish_task(void* pvParameters) {
    int count = 0;
    while (1) {
        char msg[50];
        sprintf(msg, "Hello ESP32 #%d", count++);
        esp_mqtt_client_publish(client, "test/topic", msg, 0, 1, 0);
        ESP_LOGI(TAG, "Published message: %s", msg);
        vTaskDelay(pdMS_TO_TICKS(5000)); // every 5 seconds
    }
}

// ===================== Task 2: Sensor =====================
void sensor_task(void* pvParameters) {
    int sensor_value = 0;
    while (1) {
        sensor_value++;
        char msg[50];
        sprintf(msg, "Sensor value: %d", sensor_value);
        esp_mqtt_client_publish(client, "sensor/topic", msg, 0, 1, 0);
        ESP_LOGI(TAG, "Published sensor data: %s", msg);
        vTaskDelay(pdMS_TO_TICKS(3000)); // every 3 seconds
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
    mqtt_app_start();

    // Create two tasks
    xTaskCreate(publish_task, "publish_task", 4096, NULL, 5, NULL);
    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
}
