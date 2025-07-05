#include "User_Interface.h"
#include "st7789v0.h"
#include "main.h"

int U_I_Init()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	ST7789_Init();
	ST7789_Fill_Color(BLACK);
	//ST7789_WriteString(20, 10, "Nick Wild", Font_11x18, WHITE, GREEN);
	return 0;
}

int U_I_Handler()
{
	return 0;
}