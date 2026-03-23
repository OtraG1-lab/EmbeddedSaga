
#include <stdio.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO     22
#define I2C_MASTER_SDA_IO     21
#define I2C_MASTER_NUM        I2C_NUM_0
#define I2C_MASTER_FREQ_HZ    100000
#define PCF8574_ADDR          0x27   // Adjust if your backpack uses a different address

// Bit mapping (depends on your backpack wiring)
#define LCD_RS 0x01
#define LCD_EN 0x04
#define LCD_BL 0x08   // Backlight
// D4–D7 are mapped to P4–P7 automatically when shifting nibble << 4

static const char *TAG = "LCD";

void i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    ESP_LOGI(TAG, "I2C Initialized");
}

void pcf8574_write(uint8_t data)
{
    i2c_master_write_to_device(I2C_MASTER_NUM, PCF8574_ADDR, &data, 1, pdMS_TO_TICKS(100));
}

void lcd_toggle_enable(uint8_t data)
{
    pcf8574_write(data | LCD_EN);
    esp_rom_delay_us(500);               // short pulse
    pcf8574_write(data & ~LCD_EN);
    esp_rom_delay_us(500);               // settle
}

void lcd_send_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data = (nibble << 4);    // put nibble on D4–D7
    if (rs) data |= LCD_RS;
    data |= LCD_BL;                  // backlight on
    pcf8574_write(data);
    lcd_toggle_enable(data);
}

void lcd_send_byte(uint8_t byte, uint8_t rs)
{
    lcd_send_nibble(byte >> 4, rs);  // high nibble
    lcd_send_nibble(byte & 0x0F, rs);// low nibble
}

void lcd_command(uint8_t cmd)
{
    lcd_send_byte(cmd, 0);
    vTaskDelay(pdMS_TO_TICKS(2));    // allow command to execute
}

void lcd_data(uint8_t data)
{
    lcd_send_byte(data, 1);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void lcd_init(void)
{
    vTaskDelay(pdMS_TO_TICKS(50));   // wait >40ms after power on

    lcd_send_nibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x02, 0);        // set 4-bit mode
    vTaskDelay(pdMS_TO_TICKS(5));

    lcd_command(0x28);               // function set: 4-bit, 2 line
    lcd_command(0x0C);               // display on, cursor off
    lcd_command(0x06);               // entry mode
    lcd_command(0x01);               // clear display
    vTaskDelay(pdMS_TO_TICKS(2));

    ESP_LOGI(TAG, "LCD Initialized");
}

void lcd_print(const char *str)
{
    while (*str) lcd_data(*str++);
}

void app_main(void)
{
    i2c_master_init();
    lcd_init();

    lcd_command(0x01);               // clear display
    lcd_command(0x80);               // move cursor to first line
    lcd_print("Hello Laxmi!");
    lcd_command(0xC0);
    lcd_print("Welcome");

    ESP_LOGI(TAG, "Message printed");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

