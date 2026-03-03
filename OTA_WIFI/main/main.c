/*Wi-Fi hardware
   ↓
ESP Wi-Fi driver
   ↓
Event loop
   ↓
wifi_event_handler(arg, base, id, data)*/ 

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"   // ✅ REQUIRED for esp_netif APIs

#define WIFI_SSID "DEXTRIS 2,4 JIO"
#define WIFI_PASS "Dextris@123"

#define WIFI_CONNECTED_BIT BIT0
//used to only inform (signal)to application that wifi event has accurred
static EventGroupHandle_t wifi_event_group;			

//This function is called by ESP-IDF automatically whenever a Wi-Fi / IP event occurs.
static void wifi_event_handler(void *arg,			            //User defined pointer
                               esp_event_base_t event_base,		//Tells which subsystem generated the event
                               //Like WIfi event.ethernet event,IP event, MQTT event'
                               int32_t event_id,               //Specific event inside the category,Works together with event_base
                               void *event_data)				//Pointer to extra data related to the event
{	
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        ESP_LOGI("WIFI", "Retrying...");
    }
   else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
   {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;

    ESP_LOGI("WIFI", "Connected!");
    ESP_LOGI("WIFI", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));

    xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
   }
}

void wifi_init(void)
{
    // ✅ REQUIRED: NVS initialization for Wi-Fi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_event_group = xEventGroupCreate();

   /*esp_netif = ESP Network Interface layer
   It is the glue between Wi-Fi/Ethernet and the TCP/IP stack*/
    ESP_ERROR_CHECK(esp_netif_init());					//“Prepare ESP32 so it can use IP, TCP, UDP, HTTP, MQTT, etc.”
    
    
    /*All of these are events, and ESP-IDF needs one common place to process and dispatch them.
     That place is the default event loop.*/
     ESP_ERROR_CHECK(esp_event_loop_create_default());
     
     
     /*All of these are events, and ESP-IDF needs one common place to process and dispatch them.
       That place is the default event loop.
       
       1️⃣ Physical layer → Wi-Fi radio
       2️⃣ Network interface layer → IP, DHCP, routing
        This function connects those two together for STA (station) mode.*/
    esp_netif_create_default_wifi_sta();

	/**It is a structure (struct) used to configure the Wi-Fi driver
      Contains internal parameters like:buffer sizes, task priorities, memory allocation, RX/TX settings*/
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();     //A macro provided by ESP-IDF Fills the structure with recommended default values
    
    
    /*ESP-IDF must know: how much memory to allocate, how to create Wi-Fi tasks, how Wi-Fi interacts with FreeRTOS
    This structure provides that information.*/
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    //It registers your function (wifi_event_handler) so it will be called automatically whenever a Wi-Fi event occurs.
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,
                                               ESP_EVENT_ANY_ID,
                                               &wifi_event_handler,
                                               NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,
                                               IP_EVENT_STA_GOT_IP,
                                               &wifi_event_handler,
                                               NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// ✅ This is the entry point the linker wants
void app_main(void)
{
    wifi_init();
}