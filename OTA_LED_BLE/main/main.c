#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#include "driver/gpio.h"

#define DEVICE_NAME "ESP32_LED"
#define SERVICE_UUID 0x00FF
#define CHAR_UUID    0xFF01
#define LED_GPIO     2

static uint8_t led_state = 0;
static uint16_t service_handle;
static uint16_t char_handle;

static const char *TAG = "BLE_LED";


// ---------------- GAP (Advertising) ----------------

static void gap_event_handler(esp_gap_ble_cb_event_t event,
                              esp_ble_gap_cb_param_t *param)
{
    if (event == ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT) {

        esp_ble_adv_params_t adv_params = {
            .adv_int_min = 0x20,
            .adv_int_max = 0x40,
            .adv_type = ADV_TYPE_IND,
            .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
            .channel_map = ADV_CHNL_ALL,
            .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
        };

        esp_ble_gap_start_advertising(&adv_params);
    }
}


// ---------------- GATT ----------------

static void gatts_event_handler(esp_gatts_cb_event_t event,
                                esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param)
{
    switch (event) {

    case ESP_GATTS_REG_EVT:

        ESP_LOGI(TAG, "GATT Registered");

        esp_ble_gap_set_device_name(DEVICE_NAME);

        esp_ble_adv_data_t adv_data = {
            .set_scan_rsp = false,
            .include_name = true,
            .include_txpower = true,
            .flag = ESP_BLE_ADV_FLAG_GEN_DISC |
                    ESP_BLE_ADV_FLAG_BREDR_NOT_SPT,
        };

        esp_ble_gap_config_adv_data(&adv_data);

        esp_ble_gatts_create_service(gatts_if,
                                     &(esp_gatt_srvc_id_t){
                                         .is_primary = true,
                                         .id.inst_id = 0x00,
                                         .id.uuid.len = ESP_UUID_LEN_16,
                                         .id.uuid.uuid.uuid16 = SERVICE_UUID,
                                     },
                                     4);
        break;


    case ESP_GATTS_CREATE_EVT:

        service_handle = param->create.service_handle;
        esp_ble_gatts_start_service(service_handle);

        esp_ble_gatts_add_char(
            service_handle,
            &(esp_bt_uuid_t){
                .len = ESP_UUID_LEN_16,
                .uuid.uuid16 = CHAR_UUID,
            },
            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            ESP_GATT_CHAR_PROP_BIT_READ |
            ESP_GATT_CHAR_PROP_BIT_WRITE,
            NULL,
            NULL);
        break;


    case ESP_GATTS_ADD_CHAR_EVT:
        char_handle = param->add_char.attr_handle;
        break;


    case ESP_GATTS_WRITE_EVT:

        if (!param->write.is_prep) {

            led_state = param->write.value[0];

            ESP_LOGI(TAG, "Write: %d", led_state);

            gpio_set_level(LED_GPIO, led_state ? 1 : 0);
        }
        break;


    case ESP_GATTS_READ_EVT: {

        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(rsp));

        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 1;
        rsp.attr_value.value[0] = led_state;

        esp_ble_gatts_send_response(gatts_if,
                                    param->read.conn_id,
                                    param->read.trans_id,
                                    ESP_GATT_OK,
                                    &rsp);
        break;
    }

    default:
        break;
    }
}


// ---------------- MAIN ----------------

void app_main(void)
{
    // NVS
    nvs_flash_init();

    // LED GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);

    // BT Init
    esp_bt_controller_config_t bt_cfg =
        BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_bluedroid_init();
    esp_bluedroid_enable();

    esp_ble_gap_register_callback(gap_event_handler);
    esp_ble_gatts_register_callback(gatts_event_handler);
    esp_ble_gatts_app_register(0);

    ESP_LOGI(TAG, "BLE LED Ready");
}