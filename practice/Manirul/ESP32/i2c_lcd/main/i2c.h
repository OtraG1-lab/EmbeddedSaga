#ifndef I2C_H
#define I2C_H

#include "esp_err.h"
#include <stdint.h>

esp_err_t i2c_master_init(void);

esp_err_t i2c_write_byte(uint8_t device_address,
                         uint8_t data);

#endif