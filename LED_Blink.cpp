#include <gpiod.h>
#include <iostream>
#include <unistd.h>

int main()
{
    const char *chip_path = "/dev/gpiochip0";
    unsigned int line_num = 47;   // internal ACT LED GPIO

    gpiod_chip *chip;
    gpiod_line_request *request;
    gpiod_line_settings *settings;
    gpiod_request_config *config;
    gpiod_line_config *line_cfg;

    chip = gpiod_chip_open(chip_path);

    settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);

    line_cfg = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(line_cfg, &line_num, 1, settings);

    config = gpiod_request_config_new();

    request = gpiod_chip_request_lines(chip, config, line_cfg);

    while (1)
    {
        gpiod_line_request_set_value(request, line_num, GPIOD_LINE_VALUE_ACTIVE);
        sleep(1);

        gpiod_line_request_set_value(request, line_num, GPIOD_LINE_VALUE_INACTIVE);
        sleep(1);
    }

    gpiod_chip_close(chip);

    return 0;
}
