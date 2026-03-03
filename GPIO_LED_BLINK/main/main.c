//LED Pin ON_OFF

#include"driver/gpio.h"
//#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#define LED_PIN 2					//Internal LED PIn is connected to Pin GPIO2



void app_main(void)
{
	//Configuration  set gpio in OUTPUT Mode
	gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
	
	while(1)
	{
	gpio_set_level(LED_PIN, 1);
	vTaskDelay(pdMS_TO_TICKS(10000));					//Rtos internaly takes ticks and converts into sec(5 sec)
	gpio_set_level(LED_PIN, 0);
	vTaskDelay(pdMS_TO_TICKS(10000));								
	
	}
	
}
