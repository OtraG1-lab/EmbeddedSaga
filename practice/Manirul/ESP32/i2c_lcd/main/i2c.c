#include "i2c.h"

#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"

#define I2C_MASTER_SDA_IO     21
#define I2C_MASTER_SCL_IO     22
#define I2C_MASTER_NUM        I2C_NUM_0
#define I2C_MASTER_FREQ_HZ    100000


esp_err_t i2c_master_init(void)
{
    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,

        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,

        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,

        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };

    esp_err_t ret;

    ret = i2c_param_config(
        I2C_MASTER_NUM,
        &config
    );

    if (ret != ESP_OK)
    {
        return ret;
    }

    return i2c_driver_install(
        I2C_MASTER_NUM,
        config.mode,
        0,
        0,
        0
    );
}


esp_err_t i2c_write_byte(
    uint8_t device_address,
    uint8_t data)
{
    return i2c_master_write_to_device(
        I2C_MASTER_NUM,
        device_address,
        &data,
        1,
        pdMS_TO_TICKS(100)
    );
}