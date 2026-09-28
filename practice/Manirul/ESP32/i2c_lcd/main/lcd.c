#include "lcd.h"
#include "i2c.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"


#define LCD_I2C_ADDRESS    0x27


/*
 * PCF8574T → LCD mapping
 *
 * P0 → RS
 * P1 → RW
 * P2 → EN
 * P3 → Backlight
 * P4 → D4
 * P5 → D5
 * P6 → D6
 * P7 → D7
 */

#define LCD_RS             0x01
#define LCD_RW             0x02
#define LCD_EN             0x04
#define LCD_BACKLIGHT      0x08


static void lcd_write_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data = 0;

    /* Put nibble on P4-P7 */
    data = (nibble & 0x0F) << 4;

    /* RS */
    if (rs){
        data |= LCD_RS;
    }

    /* Backlight ON */
    data |= LCD_BACKLIGHT;

    /* EN = 1 */
    i2c_write_byte(LCD_I2C_ADDRESS, data | LCD_EN);

    esp_rom_delay_us(100);

    /*EN = 0 */
    i2c_write_byte(LCD_I2C_ADDRESS, data & ~LCD_EN);

    esp_rom_delay_us(100);
}


static void lcd_write_byte(uint8_t data, uint8_t rs)
{
    /* High nibble */
    lcd_write_nibble(data >> 4, rs);

    /* Low nibble */
    lcd_write_nibble(data & 0x0F, rs);
}


void lcd_command(uint8_t command)
{
    lcd_write_byte(command, 0);

    vTaskDelay(pdMS_TO_TICKS(2));
}


void lcd_data(uint8_t data)
{
    lcd_write_byte(data, 1);

    vTaskDelay(pdMS_TO_TICKS(2));
}


esp_err_t lcd_init(void)
{
    /*
     * Wait for LCD power-up
     */
    vTaskDelay(pdMS_TO_TICKS(50));

    /*
     * RS = 0
     */
    lcd_write_nibble(0x03, 0);

    vTaskDelay(pdMS_TO_TICKS(5));

    lcd_write_nibble(0x03, 0);

    vTaskDelay(pdMS_TO_TICKS(5));

    lcd_write_nibble(0x03, 0);

    vTaskDelay(pdMS_TO_TICKS(5));

    /*
     * Switch to 4-bit mode
     */
    lcd_write_nibble(0x02, 0);

    vTaskDelay(pdMS_TO_TICKS(5));

    /*
     * Function set:
     * 4-bit
     * 2 lines
     * 5x8 font
     */
    lcd_command(0x28);

    /*
     * Display ON
     * Cursor OFF
     * Blink OFF
     */
    lcd_command(0x0C);

    /*
     * Entry mode
     */
    lcd_command(0x06);

    /*
     * Clear display
     */
    lcd_command(0x01);

    vTaskDelay(pdMS_TO_TICKS(5));

    return ESP_OK;
}


void lcd_clear(void)
{
    lcd_command(0x01);

    vTaskDelay(pdMS_TO_TICKS(2));
}


void lcd_set_cursor(uint8_t row, uint8_t column)
{
    uint8_t address;

    if (row == 0)
    {
        address = 0x00;
    }
    else
    {
        address = 0x40;
    }

    address += column;

    lcd_command(
        0x80 | address
    );
}


void lcd_print(const char *str)
{
    while (*str)
    {
        lcd_data(*str);
        str++;
    }
}