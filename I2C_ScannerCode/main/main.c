//I2c Scanner code :
#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO 22					//GPIO 22 as Serial clock Line
#define I2C_MASTER_SDA_IO 21					//GPIO 21 as Serial data line
#define I2C_MASTER_NUM I2C_NUM_0				//to use first i2C hardware
#define I2C_MASTER_FREQ_HZ 100000				//Set frquency is 100Khz standard

static const char *TAG = "I2C_SCAN";

void i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,					//Define I2C  as master means ESP32 will control the  bus
        .sda_io_num = I2C_MASTER_SDA_IO,			//SDA No
        .scl_io_num = I2C_MASTER_SCL_IO,			//SCL no
        .sda_pullup_en = GPIO_PULLUP_ENABLE,		//to set SDA line in idle state that is high 
        .scl_pullup_en = GPIO_PULLUP_ENABLE,		//to set SCL line in idle state that is high 
        .master.clk_speed = I2C_MASTER_FREQ_HZ		//
    };

/* ESPIDF API that applies your configuration settings to the chosen I²C controller (here I2C_NUM_0).
- The driver checks that the pins you chose are valid GPIOs.
- It sets the controller into master mode.
- It assigns SDA = GPIO21, SCL = GPIO22.
- It enables internal pull‑ups if requested.
- It sets the clock speed to 100 kHz.
- It stores these settings so the driver knows how to generate signals on the bus.
*/
    i2c_param_config(I2C_MASTER_NUM, &conf);
    		
 /*-  installs the I²C driver for the selected controller (I2C_NUM_0 here)
  - Which I²C controller to install the driver for (I2C_NUM_0 or I2C_NUM_1).
- conf.mode
- The mode you configured earlier (I2C_MODE_MASTER in your case).
- Could also be I2C_MODE_SLAVE if you wanted ESP32 to act as a slave.
- 0 (rx_buf_len)
- Length of the RX buffer (used in slave mode).
- Since you’re in master mode, you don’t need it → set to 0.
- 0 (tx_buf_len)
- Length of the TX buffer (used in slave mode).
- Again, not needed in master mode → set to 0.
- 0 (intr_alloc_flags)
- Flags for interrupt allocation (priority, CPU affinity, etc.).
- Default = 0, meaning “use default settings.”
 */
   
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void app_main()
{
    i2c_master_init();

    ESP_LOGI(TAG, "Scanning I2C bus...");

    for (int addr = 1; addr < 127; addr++)			//As 8bit I2C can connect 128 devices 
    {
		
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "Device found at address: 0x%02X", addr);
        }
    }

    ESP_LOGI(TAG, "Scan complete");
}
