#include "same70.h"
#include "hal_gpio.h"

HAL_GPIO_PIN(LED,	   C, 8)
HAL_GPIO_PIN(BUTTON,   A, 11)

void LED_init(void)
{
	HAL_GPIO_LED_out();
	HAL_GPIO_LED_clr();
}

int LED_set(int st) 
{
	switch (st) {
		case 0  : HAL_GPIO_LED_clr(); break;
		case 1  : HAL_GPIO_LED_set(); break;
		default : HAL_GPIO_LED_toggle(); break;
	}
	return 0;
}

void GPIO_init(void)
{
	HAL_GPIO_BUTTON_in();
	HAL_GPIO_BUTTON_pullup();
}

int BUTTON_read(void)
{
	return HAL_GPIO_BUTTON_read();
}
