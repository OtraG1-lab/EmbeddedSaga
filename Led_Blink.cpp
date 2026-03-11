#include <iostream>
#include <fstream>
#include <unistd.h>

int main()
{
    while(true)
    {
        std::ofstream led("/sys/class/leds/led0/brightness");
        led << "1";
        led.close();

        sleep(1);

        std::ofstream led2("/sys/class/leds/led0/brightness");
        led2 << "0";
        led2.close();

        sleep(1);
    }

    return 0;
}
