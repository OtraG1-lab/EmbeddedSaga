/* =========================================================
   ESP32 Wi-Fi + HTTP Server + Internal LED ON/OFF
   ---------------------------------------------------------
   Browser control:
     http://<ESP32_IP>/on
     http://<ESP32_IP>/off
   ---------------------------------------------------------
   Internal LED: GPIO 2 (most ESP32 dev boards)
   ========================================================= */

/* ===================== Includes ===================== */

#include <string.h>				//Used internally by Wi-Fi / HTTP (copying strings like SSID, password)
#include <stdbool.h>

/*👉 Gives access to: RTOS core, Task scheduling, System timing
Even if you don’t create tasks, ESP-IDF uses this.*/
#include "freertos/FreeRTOS.h"
/*Used for event synchronization You used it to say:
“Wi-Fi is connected — now allow HTTP server”*/
#include "freertos/event_groups.h"

/*Controls Wi-Fi driver: start Wi-Fi, connect to AP, set mode (STA/AP)*/
#include "esp_wifi.h"

/*ESP32’s event dispatcher: Wi-Fi events, IP events, System events, This is how ESP32 avoids polling.*/
#include "esp_event.h"


#include "esp_log.h"

/*Non-Volatile Storage
Wi-Fi requires NVS to store calibration data.
Without this → Wi-Fi will FAIL.*/
#include "nvs_flash.h"

/*Built-in HTTP server, URI handlers, Request parsing, Response sending*/
#include "esp_http_server.h"

/*Direct hardware Control : Configure pins, Set voltage level*/
#include "driver/gpio.h"

/* ===================== Wi-Fi Credentials ===================== */

#define WIFI_SSID "DEXTRIS 2,4 JIO"
#define WIFI_PASS "Dextris@123"

/* ===================== LED Configuration ===================== */

#define LED_GPIO GPIO_NUM_2     // Internal LED pin


#define LED_ON   1
#define LED_OFF  0

static bool led_state = false;  // false = OFF, true = ON

/* ===================== Wi-Fi Event Group ===================== */

#define WIFI_CONNECTED_BIT BIT0

static EventGroupHandle_t wifi_event_group;				//

static const char *TAG = "ESP32_HTTP";

/* =========================================================
   LED INITIALIZATION
   ========================================================= */
static void led_init(void)
{
    gpio_config_t io_conf = {							//Structure describing pin behavior 
        .pin_bit_mask = (1ULL << LED_GPIO),				//Select GPIO2
        .mode = GPIO_MODE_OUTPUT,						//Mode
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);					//Applies Configuration 

    gpio_set_level(LED_GPIO, LED_OFF); // Set Led Dafault state.
    led_state = false;									//deafault off
}

/* =========================================================
   HTTP HANDLER: ROOT "/"
   ========================================================= */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    const char *resp =
        "ESP32 HTTP LED Control\n"
        "Use /on or /off";

    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* =========================================================
   HTTP HANDLER: "/on"
   ========================================================= */
static esp_err_t led_on_handler(httpd_req_t *req)
{
    gpio_set_level(LED_GPIO, LED_ON);
    led_state = true;

    httpd_resp_send(req, "Internal LED: ON", HTTPD_RESP_USE_STRLEN);
    ESP_LOGI("HTTP", "LED turned ON");

    return ESP_OK;
}

/* =========================================================
   HTTP HANDLER: "/off"
   ========================================================= */
static esp_err_t led_off_handler(httpd_req_t *req)
{
    gpio_set_level(LED_GPIO, LED_OFF);
    led_state = false;

    httpd_resp_send(req, "Internal LED: OFF", HTTPD_RESP_USE_STRLEN);
    ESP_LOGI("HTTP", "LED turned OFF");

    return ESP_OK;
}

/* =========================================================
   START HTTP SERVER
   ========================================================= */
static httpd_handle_t server = NULL;

static void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI("HTTP", "Starting server on port %d", config.server_port);

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t root_uri = {
            .uri      = "/",
            .method   = HTTP_GET,
            .handler  = root_get_handler,
            .user_ctx = NULL
        };

        httpd_uri_t on_uri = {
            .uri      = "/on",
            .method   = HTTP_GET,
            .handler  = led_on_handler,
            .user_ctx = NULL
        };

        httpd_uri_t off_uri = {
            .uri      = "/off",
            .method   = HTTP_GET,
            .handler  = led_off_handler,
            .user_ctx = NULL
        };

        httpd_register_uri_handler(server, &root_uri);
        httpd_register_uri_handler(server, &on_uri);
        httpd_register_uri_handler(server, &off_uri);
    }
}

/* =========================================================
   WIFI EVENT HANDLER
   ========================================================= */
static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Retrying Wi-Fi...");
    }
    else if (event_base == IP_EVENT &&
             event_id == IP_EVENT_STA_GOT_IP)
    {
        ESP_LOGI(TAG, "Wi-Fi connected");

        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);

        // Start HTTP server ONLY after IP is obtained
        start_webserver();
    }
}

/* =========================================================
   WIFI INITIALIZATION
   ========================================================= */
static void wifi_init(void)
{
    wifi_event_group = xEventGroupCreate();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_register(WIFI_EVENT,
                               ESP_EVENT_ANY_ID,
                               &wifi_event_handler,
                               NULL);

    esp_event_handler_register(IP_EVENT,
                               IP_EVENT_STA_GOT_IP,
                               &wifi_event_handler,
                               NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

/* =========================================================
   MAIN ENTRY POINT
   ========================================================= */
void app_main(void)
{
    // Initialize NVS
    nvs_flash_init();

    // Initialize internal LED GPIO
    led_init();

    // Initialize Wi-Fi and HTTP server
    wifi_init();
}