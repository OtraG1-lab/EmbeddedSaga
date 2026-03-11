#include <iostream>
#include <fcntl.h>
#include <cstdint>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <thread>
#include <chrono>

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

void lcdDisplaySegment(int fd, const char* text, int start, int line) {
    char buffer[17] = {0};
    strncpy(buffer, text + start, 16);

    lcdByte(fd, line, LCD_CMD);
    for (int i = 0; i < strlen(buffer); i++)
        lcdByte(fd, buffer[i], LCD_CHR);
}

// Scroll string across 2 lines
void lcdScrollText(int fd, const char* msg) {
    size_t len = strlen(msg);
    size_t maxLen = 32; // max display chars (16x2)

    // Pad with spaces at the end for smooth scroll
    char padded[128] = {0};
    strcpy(padded, msg);
    strcat(padded, "                "); // 16 spaces padding

    size_t scrollLen = strlen(padded);

    for(size_t pos = 0; pos < scrollLen - 15; pos++) {
        lcdDisplaySegment(fd, padded, pos, LCD_LINE1);
        lcdDisplaySegment(fd, padded, pos + 16, LCD_LINE2);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
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

    // Test string longer than 32 chars
    const char* message = "Hello Raspberry Pi! This text will scroll automatically across the 16x2 LCD display.";
    
    while(true) {
        lcdScrollText(fd, message);
    }

    close(fd);
    return 0;
}
