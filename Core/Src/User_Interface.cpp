#include "User_Interface.h"
#include "st7789v0.h"
#include <stdio.h>
#include "main.h"

int j = 0;

int U_I::Init()
{
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	ST7789_Init();
	ST7789_Fill_Color(BLACK);
	//ST7789_WriteString(20, 10, "Nick Wild", Font_11x18, WHITE, GREEN);
	return 0;
}

int U_I::Handler()
{
	
	return 0;
}

int U_I::print_adc_Channel(uint16_t* adcDataChannel)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		char strPrint[20];
		sprintf(strPrint, "%d: %4d\0", i, adcDataChannel[i]);
		ST7789_WriteString(5, 25 + i * 9, strPrint, Font_7x9, WHITE, BLACK);
	}

	j++;
	char strPrint[20];
	sprintf(strPrint, "%d\0", j);
	ST7789_WriteString(2, 2, strPrint, Font_7x9, WHITE, BLACK);
	return 0;
}

int U_I::print_buttons_press(uint8_t* lButtonsPress, uint8_t* rButtonsPress)
{
	for (int i = 0; i < 5; i++) {
		char strPrint[20];
		sprintf(strPrint, "%d: %d\0", i, lButtonsPress[i]);
		ST7789_WriteString(70, 2 + i * 9, strPrint, Font_7x9, WHITE, BLACK);
	}
	for (int i = 0; i < 5; i++) {
		char strPrint[20];
		sprintf(strPrint, "%d: %d\0", i, rButtonsPress[i]);
		ST7789_WriteString(70, 52 + i * 9, strPrint, Font_7x9, WHITE, BLACK);
	}
	return 0;
}

int U_I::print_buttons_hold(uint8_t* lButtonsHold, uint8_t* rButtonsHold)
{
	for (int i = 0; i < 5; i++) {
		char strPrint[20];
		sprintf(strPrint, "%d: %d\0", i, lButtonsHold[i]);
		ST7789_WriteString(120, 2 + i * 9, strPrint, Font_7x9, WHITE, BLACK);
	}
	for (int i = 0; i < 5; i++) {
		char strPrint[20];
		sprintf(strPrint, "%d: %d\0", i, rButtonsHold[i]);
		ST7789_WriteString(120, 52 + i * 9, strPrint, Font_7x9, WHITE, BLACK);
	}
	return 0;
}

int U_I::PrintScreen2()
{
	return 0;
}