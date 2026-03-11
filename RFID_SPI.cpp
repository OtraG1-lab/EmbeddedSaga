#include <iostream>
#include <fcntl.h>
#include <cstdint>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>
int main() {

    const char *device = "/dev/spidev0.0";
    int fd = open(device, O_RDWR);

    uint8_t mode = SPI_MODE_0;
    uint32_t speed = 500000;

    ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    uint8_t tx[] = {0x00};
    uint8_t rx[] = {0x00};

    struct spi_ioc_transfer tr;

    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = 1;
    tr.speed_hz = speed;

    ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    std::cout << "SPI Communication Done" << std::endl;

    close(fd);
}
