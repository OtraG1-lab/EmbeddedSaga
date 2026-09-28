#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_GPIO       GPIO_NUM_2
#define BUTTON_GPIO    GPIO_NUM_0

void app_main(void)
{
    // LED
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Button
    gpio_reset_pin(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);

    gpio_pullup_en(BUTTON_GPIO);
    gpio_pulldown_dis(BUTTON_GPIO);

    while (1)
    {
        if (gpio_get_level(BUTTON_GPIO) == 0)
        {
            gpio_set_level(LED_GPIO, 1);
        }
        else
        {
            gpio_set_level(LED_GPIO, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}