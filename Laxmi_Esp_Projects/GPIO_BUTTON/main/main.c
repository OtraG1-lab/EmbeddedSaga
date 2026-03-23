#include "driver/adc.h"
#include "esp_log.h"

#define ADC_CHANNEL ADC1_CHANNEL_6 // GPIO34
static const char *TAG = "ADC_TEST";

void app_main(void) {
    // Configure ADC
    adc1_config_width(ADC_WIDTH_BIT_12);         // 12-bit ADC
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11); // 0-3.3V range

    while(1) {
        int raw = adc1_get_raw(ADC_CHANNEL);   // Read raw value
        float voltage = raw * (3.3 / 4095.0);  // Convert to voltage

        ESP_LOGI(TAG, "Raw ADC: %d | Voltage: %.3f V", raw, voltage);

        vTaskDelay(500 / portTICK_PERIOD_MS);  // Delay 0.5 sec
    }
}
