#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_PIN        2
#define LEDC_CHANNEL  LEDC_CHANNEL_0
#define LEDC_TIMER    LEDC_TIMER_0
#define LEDC_MODE     LEDC_HIGH_SPEED_MODE
#define LEDC_DUTY_RES LEDC_TIMER_8_BIT
#define LEDC_FREQ     5000

void app_main(void)
{
    /* 1️⃣ Configure PWM timer */
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,				//HighSpeed Mode
        .timer_num        = LEDC_TIMER,				//Timer number 0
        .duty_resolution  = LEDC_DUTY_RES,			//8 bit duty cycle resolution
        .freq_hz          = LEDC_FREQ,				//5K
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    /* 2️⃣ Configure PWM channel */ // this structure is inbuilt defined which header file is included above
    ledc_channel_config_t ledc_channel = {
        .gpio_num   = LED_PIN,			//2
        .speed_mode = LEDC_MODE,		//High speed Mode
        .channel    = LEDC_CHANNEL,		//0
        .timer_sel  = LEDC_TIMER,		//0
        .duty       = 0,				//0- 0%, 128-50%, 255-100%
        .hpoint     = 0					//hpoint = when PWM goes high withinn 1 cycle 0=PWM start high at begenning
    };
    ledc_channel_config(&ledc_channel);

    /* 3️⃣ Change duty cycle (fade LED) */
    while (1)
    {
        for (int duty = 0; duty <= 255; duty++)
        {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);					//stores value in memory
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);						//Writes on hardware Immediately.
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        for (int duty = 255; duty >= 0; duty--)
        {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}