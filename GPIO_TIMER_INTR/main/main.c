// ESP32 hardware timer generates an interrupt every 1 second, and the ISR toggles the LED.
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/timer.h"

#define LED_PIN GPIO_NUM_4
#define TIMER_DIVIDER 80      // 80 MHz / 80 = 1 MHz (1 tick = 1 µs)
#define TIMER_INTERVAL_SEC 1  // 1 second

volatile bool led_state = false;

/* Timer ISR */
void IRAM_ATTR timer_isr_handler(void *arg)
{
    led_state = !led_state;
    gpio_set_level(LED_PIN, led_state);

    // Clear interrupt
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);

    // Re-enable alarm
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
}

void app_main(void)
{
    // 1. Configure LED pin
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // 2. Timer configuration structure
    timer_config_t timer_conf = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true,
    };

    // 3. Initialize timer
    timer_init(TIMER_GROUP_0, TIMER_0, &timer_conf);

    // 4. Set timer counter value
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);

    // 5. Set alarm value (1 second)
    timer_set_alarm_value(
        TIMER_GROUP_0,
        TIMER_0,
        TIMER_INTERVAL_SEC * 1000000
    );

    // 6. Enable interrupt
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);

    // 7. Register ISR
    timer_isr_register(
        TIMER_GROUP_0,
        TIMER_0,
        timer_isr_handler,
        NULL,
        ESP_INTR_FLAG_IRAM,
        NULL
    );

    // 8. Start timer
    timer_start(TIMER_GROUP_0, TIMER_0);
}