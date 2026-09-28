#include "i2c.h"
#include "lcd.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "Main Executing";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting I2C LCD");

    /*
     * Initialize I2C
     */
    ESP_ERROR_CHECK(i2c_master_init());

    /*
     * Initialize LCD
     */
    ESP_ERROR_CHECK(lcd_init());

    lcd_set_cursor(0, 0);  //First line

    lcd_print("ESP32 I2C LCD");

    lcd_set_cursor(1, 0);  // Second line

    lcd_print("Hello World!");

    ESP_LOGI(TAG, "LCD message displayed");

    // lcd_set_cursor(0, 0);
    // lcd_print("A"); //Check for print 'A' only

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}