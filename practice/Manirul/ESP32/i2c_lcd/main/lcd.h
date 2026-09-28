#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include "esp_err.h"

esp_err_t lcd_init(void);

void lcd_command(uint8_t command);

void lcd_data(uint8_t data);

void lcd_clear(void);

void lcd_set_cursor(uint8_t row, uint8_t column);

void lcd_print(const char *str);

#endif