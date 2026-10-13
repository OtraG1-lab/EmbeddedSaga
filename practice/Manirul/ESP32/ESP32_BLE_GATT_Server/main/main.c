#include <stdio.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"

#define TAG "BLE_SERVER"
#define DEVICE_NAME "ESP32_BLE_TEST"

/*This variable is to print actual value on Monitor*/
static uint16_t char_value_handle;
static uint16_t cccd_handle;

static bool notifications_enabled = false;

/* BLE UUIDs */
static const uint16_t primary_service_uuid =
    ESP_GATT_UUID_PRI_SERVICE;

static const uint16_t character_declaration_uuid =
    ESP_GATT_UUID_CHAR_DECLARE;

static const uint16_t service_uuid = 0x00FF;
static const uint16_t char_uuid = 0xFF01;

/* Characteristic value and properties */
static uint8_t char_value = 0;

static const uint8_t char_properties =
    ESP_GATT_CHAR_PROP_BIT_READ |
    ESP_GATT_CHAR_PROP_BIT_WRITE |
    ESP_GATT_CHAR_PROP_BIT_NOTIFY;

/* Attribute indexes */
enum
{
    IDX_SVC,
    IDX_CHAR,
    IDX_CHAR_VAL,
    IDX_CHAR_CCCD,
    ATTR_COUNT
};

static const uint16_t cccd_uuid =
    ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

static uint16_t cccd_value = 0x0000;

/* GATT attribute table */
static esp_gatts_attr_db_t gatt_db[ATTR_COUNT] = {
    /* Primary service declaration */
    [IDX_SVC] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16,
         (uint8_t *)&primary_service_uuid,
         ESP_GATT_PERM_READ,
         sizeof(uint16_t),
         sizeof(service_uuid),
         (uint8_t *)&service_uuid}},

    /* Characteristic declaration */
    [IDX_CHAR] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, sizeof(uint8_t), sizeof(char_properties), (uint8_t *)&char_properties}},

    /* Characteristic value */
    [IDX_CHAR_VAL] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&char_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, sizeof(char_value), sizeof(char_value), &char_value}},

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CCCD] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&cccd_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, sizeof(cccd_value), sizeof(cccd_value), (uint8_t *)&cccd_value}}

};

/* GAP event handler */
static void gap_event_handler(
    esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
    {
        ESP_LOGI(TAG, "Advertising data configured");

        esp_ble_adv_params_t adv_params = {
            .adv_int_min = 0x20,
            .adv_int_max = 0x40,
            .adv_type = ADV_TYPE_IND,
            .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
            .channel_map = ADV_CHNL_ALL,
            .adv_filter_policy =
                ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
        };

        esp_err_t ret =
            esp_ble_gap_start_advertising(&adv_params);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Start advertising failed: %s",
                     esp_err_to_name(ret));
        }
        break;
    }

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGI(TAG, "Advertising started successfully");
        }
        else
        {
            ESP_LOGE(TAG, "Advertising start failed");
        }
        break;

    default:
        ESP_LOGI(TAG, "GAP event: %d", event);
        break;
    }
}

/* GATT event handler */

static void gatts_event_handler(
    esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GATTS_REG_EVT:
    {
        ESP_LOGI(TAG, "GATT application registered");

        esp_err_t ret =
            esp_ble_gap_set_device_name(DEVICE_NAME);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Set device name failed: %s",
                     esp_err_to_name(ret));
            break;
        }

        esp_ble_adv_data_t adv_data = {
            .set_scan_rsp = false,
            .include_name = true,
            .include_txpower = true,
            .flag = ESP_BLE_ADV_FLAG_GEN_DISC |
                    ESP_BLE_ADV_FLAG_BREDR_NOT_SPT,
        };

        ret = esp_ble_gap_config_adv_data(&adv_data);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Advertising configuration failed: %s",
                     esp_err_to_name(ret));
        }

        ret = esp_ble_gatts_create_attr_tab(
            gatt_db,
            gatts_if,
            ATTR_COUNT,
            0);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Attribute table creation failed: %s",
                     esp_err_to_name(ret));
        }

        break;
    }

    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
    {
        if (param->add_attr_tab.status == ESP_GATT_OK &&
            param->add_attr_tab.num_handle == ATTR_COUNT)
        {
            ESP_LOGI(TAG, "GATT attribute table created");

            /* Save the characteristic value handle */
            char_value_handle =
                param->add_attr_tab.handles[IDX_CHAR_VAL];

            cccd_handle =
                param->add_attr_tab.handles[IDX_CHAR_CCCD];

            ESP_LOGI(TAG,
                     "Characteristic value handle: %u",
                     (unsigned int)char_value_handle);

            /* Start the primary service */
            esp_err_t ret = esp_ble_gatts_start_service(
                param->add_attr_tab.handles[IDX_SVC]);

            if (ret == ESP_OK)
            {
                ESP_LOGI(TAG, "GATT service start requested");
            }
            else
            {
                ESP_LOGE(TAG, "Start service failed: %s",
                         esp_err_to_name(ret));
            }
        }
        else
        {
            ESP_LOGE(TAG,
                     "Attribute table creation failed, status=%d, handles=%d",
                     param->add_attr_tab.status,
                     param->add_attr_tab.num_handle);
        }

        break;
    }

    case ESP_GATTS_START_EVT:
        if (param->start.status == ESP_GATT_OK)
        {
            ESP_LOGI(TAG, "GATT service started successfully");
        }
        else
        {
            ESP_LOGE(TAG, "GATT service start failed, status=%d",
                     param->start.status);
        }
        break;

    case ESP_GATTS_READ_EVT:
    {
        ESP_LOGI(TAG, "Characteristic read event");

        if (param->read.handle == char_value_handle)
        {
            ESP_LOGI(TAG,
                     "Current value: decimal=%u, hex=0x%02X",
                     (unsigned int)char_value,
                     (unsigned int)char_value);
        }

        break;
    }

    case ESP_GATTS_WRITE_EVT:
    {
        /* Case 1: Phone writes the characteristic value */
        if (param->write.handle == char_value_handle)
        {
            if (param->write.len == 1)
            {
                char_value = param->write.value[0];

                ESP_LOGI(TAG, "Notifications enabled flag: %d",
                         notifications_enabled);

                if (notifications_enabled)
                {
                    esp_err_t ret = esp_ble_gatts_send_indicate(
                        gatts_if,
                        param->write.conn_id,
                        char_value_handle,
                        sizeof(char_value),
                        &char_value,
                        false);

                    if (ret != ESP_OK)
                    {
                        ESP_LOGE(TAG, "Notification send failed: %s",
                                 esp_err_to_name(ret));
                    }
                    else
                    {
                        ESP_LOGI(TAG, "Notification requested");
                    }
                }

                ESP_LOGI(TAG,
                         "Write received: decimal=%u, hex=0x%02X",
                         (unsigned int)char_value,
                         (unsigned int)char_value);
            }
        }

        /* Case 2: Phone enables/disables notifications */
        else if (param->write.handle == cccd_handle)
        {
            if (param->write.len == 2)
            {
                uint16_t cccd =
                    param->write.value[0] |
                    ((uint16_t)param->write.value[1] << 8);

                notifications_enabled = (cccd & 0x0001) != 0;

                ESP_LOGI(TAG, "Notifications %s",
                         notifications_enabled ? "enabled" : "disabled");
            }
        }

        break;
    }

    case ESP_GATTS_CONNECT_EVT:
    {
        ESP_LOGI(TAG, "Phone connected");

        ESP_LOGI(TAG, "Connection ID: %d",
                 param->connect.conn_id);

        break;
    }

    case ESP_GATTS_DISCONNECT_EVT:
    {
        ESP_LOGI(TAG, "Phone disconnected");

        /* Restart advertising so the phone can reconnect */
        esp_err_t ret = esp_ble_gap_start_advertising(
            &(esp_ble_adv_params_t){
                .adv_int_min = 0x20,
                .adv_int_max = 0x40,
                .adv_type = ADV_TYPE_IND,
                .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
                .channel_map = ADV_CHNL_ALL,
                .adv_filter_policy =
                    ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
            });

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Restart advertising failed: %s",
                     esp_err_to_name(ret));
        }
        else
        {
            ESP_LOGI(TAG, "Restart advertising requested");
        }

        break;
    }

    default:
        ESP_LOGI(TAG, "GATT event received: %d", event);
        break;
    }
}

void app_main(void)
{
    /* Step 1: Initialize NVS */
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS initialized");

    /* Step 2: Initialize Bluetooth controller */
    esp_bt_controller_config_t bt_cfg =
        BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));

    /* Step 3: Enable BLE mode */
    ESP_ERROR_CHECK(
        esp_bt_controller_enable(ESP_BT_MODE_BLE));

    ESP_LOGI(TAG, "Bluetooth controller enabled");

    /* Step 4: Initialize Bluedroid */
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_LOGI(TAG, "Bluedroid host enabled");

    /* Step 5: Register callbacks */
    ESP_ERROR_CHECK(
        esp_ble_gap_register_callback(gap_event_handler));

    ESP_ERROR_CHECK(
        esp_ble_gatts_register_callback(gatts_event_handler));

    ESP_LOGI(TAG, "GAP and GATT callbacks registered");

    /* Step 6: Register the GATT application */
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(0));

    ESP_LOGI(TAG, "GATT application registration requested");
}
