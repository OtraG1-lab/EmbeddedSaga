#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include<cstdint>
#define LCD_ADDR 0x27
#define LCD_BACKLIGHT 0x08

#define LCD_CHR 1
#define LCD_CMD 0
#define LCD_LINE1 0x80
#define LCD_LINE2 0xC0
#define ENABLE 0x04

void lcdToggleEnable(int fd, uint8_t data) {
    usleep(500);
    data |= ENABLE;
    write(fd, &data, 1);
    usleep(500);
    data &= ~ENABLE;
    write(fd, &data, 1);
    usleep(500);
}

void lcdByte(int fd, uint8_t bits, uint8_t mode) {
    uint8_t highNib = mode | (bits & 0xF0) | LCD_BACKLIGHT;
    uint8_t lowNib  = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;

    write(fd, &highNib, 1);
    lcdToggleEnable(fd, highNib);

    write(fd, &lowNib, 1);
    lcdToggleEnable(fd, lowNib);
}

void lcdInit(int fd) {
    lcdByte(fd, 0x33, LCD_CMD);
    lcdByte(fd, 0x32, LCD_CMD);
    lcdByte(fd, 0x06, LCD_CMD);
    lcdByte(fd, 0x0C, LCD_CMD);
    lcdByte(fd, 0x28, LCD_CMD);
    lcdByte(fd, 0x01, LCD_CMD);
    usleep(500);
}

// Print string automatically on 2 lines
void lcdMessage2Lines(int fd, const char* msg) {
    char line1[17] = {0};
    char line2[17] = {0};

    // Copy first 16 chars
    strncpy(line1, msg, 16);
    // Copy next 16 chars
    if(strlen(msg) > 16)
        strncpy(line2, msg + 16, 16);

    lcdByte(fd, LCD_LINE1, LCD_CMD);
    for(int i = 0; i < strlen(line1); i++)
        lcdByte(fd, line1[i], LCD_CHR);

    lcdByte(fd, LCD_LINE2, LCD_CMD);
    for(int i = 0; i < strlen(line2); i++)
        lcdByte(fd, line2[i], LCD_CHR);
}

int main() {
    const char* device = "/dev/i2c-1";
    int fd = open(device, O_RDWR);
    if(fd < 0) {
        std::cerr << "Failed to open I2C device\n";
        return 1;
    }

    if(ioctl(fd, I2C_SLAVE, LCD_ADDR) < 0) {
        std::cerr << "Failed to acquire bus access\n";
        return 1;
    }

    lcdInit(fd);

    lcdMessage2Lines(fd, "Hello Raspberry Pi - Automatic 2 Line Display!");

    close(fd);
    return 0;
}
