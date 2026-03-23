
#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define RC522_MISO 19
#define RC522_MOSI 23
#define RC522_SCLK 18
#define RC522_CS   5
#define RC522_RST  22

static const char *TAG = "RC522";
spi_device_handle_t rc522;

// --- SPI Init ---
void spi_init_rc522(void)
{
    spi_bus_config_t buscfg = {
        .miso_io_num = RC522_MISO,
        .mosi_io_num = RC522_MOSI,
        .sclk_io_num = RC522_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1*1000*1000, // 1 MHz
        .mode = 0,                     // SPI mode 0
        .spics_io_num = RC522_CS,
        .queue_size = 1
    };

    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, 1));
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &rc522));

    ESP_LOGI(TAG, "SPI initialized for RC522");
}

// --- Write Register ---
void rc522_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t data[2];
    data[0] = (reg << 1) & 0x7E; // MSB=0 for write
    data[1] = val;

    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = data
    };
    ESP_ERROR_CHECK(spi_device_transmit(rc522, &t));
}

// --- Read Register ---
uint8_t rc522_read_reg(uint8_t reg)
{
    uint8_t tx[2], rx[2];
    tx[0] = ((reg << 1) & 0x7E) | 0x80; // MSB=1 for read
    tx[1] = 0x00;

    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = tx,
        .rx_buffer = rx
    };
    ESP_ERROR_CHECK(spi_device_transmit(rc522, &t));

    return rx[1];
}

// --- Initialization ---
void rc522_init(void)
{
    rc522_write_reg(0x01, 0x0F); // CommandReg = Reset

    rc522_write_reg(0x2A, 0x8D); // TModeReg
    rc522_write_reg(0x2B, 0x3E); // TPrescalerReg
    rc522_write_reg(0x2C, 30);   // TReloadRegL
    rc522_write_reg(0x2D, 0);    // TReloadRegH

    rc522_write_reg(0x11, 0x3D); // ModeReg
    rc522_write_reg(0x26, 0x48); // RFCfgReg

    uint8_t val = rc522_read_reg(0x14); // TxControlReg
    if (!(val & 0x03)) {
        rc522_write_reg(0x14, val | 0x03); // Turn on antenna
    }

    ESP_LOGI(TAG, "RC522 initialized and antenna enabled");
}

// --- Request (REQA) ---

bool rc522_request(void) {
    rc522_write_reg(0x0A, 0x80); // Clear FIFO
    rc522_write_reg(0x0D, 0x07); // BitFramingReg
    rc522_write_reg(0x09, 0x26); // FIFODataReg = REQA
    rc522_write_reg(0x01, 0x0C); // CommandReg = Transceive
    rc522_write_reg(0x0D, 0x87); // Start transmission

    // Wait for response or timeout
    int counter = 0;
    uint8_t irq;
    do {
        irq = rc522_read_reg(0x04); // ComIrqReg
        counter++;
    } while (!(irq & 0x30) && counter < 2000);

    uint8_t error = rc522_read_reg(0x06); // ErrorReg
    uint8_t length = rc522_read_reg(0x0A); // FIFOLevelReg

    ESP_LOGI(TAG, "REQA check: IRQ=0x%02X, Error=0x%02X, FIFOlen=%d", irq, error, length);

    if ((irq & 0x20) && length >= 2 && !(error & 0x13)) {
        return true;
    }
    return false;
}



void rc522_get_uid(void) {
    rc522_write_reg(0x0A, 0x80); // Clear FIFO
    rc522_write_reg(0x09, 0x93); // Anti-collision command
    rc522_write_reg(0x09, 0x20);
    rc522_write_reg(0x01, 0x0C); // Transceive
    rc522_write_reg(0x0D, 0x80);

    uint8_t irq;
    do {
        irq = rc522_read_reg(0x04); // ComIrqReg
    } while (!(irq & 0x30));

    uint8_t error = rc522_read_reg(0x06);
    if (error & 0x13) {
        ESP_LOGI(TAG, "UID read error.");
        return;
    }

    uint8_t length = rc522_read_reg(0x0A);
    if (length >= 4) {
        uint8_t uid[4];
        for (int i = 0; i < 4; i++) uid[i] = rc522_read_reg(0x09);
        ESP_LOGI(TAG, "Card UID: %02X %02X %02X %02X", uid[0], uid[1], uid[2], uid[3]);
    } else {
        ESP_LOGI(TAG, "No valid UID received.");
    }
}


// --- Main ---
void app_main(void)
{
    spi_init_rc522();

    uint8_t version = rc522_read_reg(0x37);
    ESP_LOGI(TAG, "RC522 Version: 0x%02X", version);

    if (version == 0x91 || version == 0x92) {
        ESP_LOGI(TAG, "RC522 detected successfully!");
        rc522_init();

        while (1) {
            if (rc522_request()) {
                ESP_LOGI(TAG, "Card detected!");
                rc522_get_uid(); // Print UID
            } else {
                ESP_LOGI(TAG, "No card present.");
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    } else {
        ESP_LOGW(TAG, "Unexpected version, check wiring!");
    }
}