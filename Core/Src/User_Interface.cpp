#include "User_Interface.h"
#include "st7789v0.h"
#include <stdio.h>
#include "main.h"

int j = 0;

int U_I::Init()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	ST7789_Init();
	ST7789_Fill_Color(BLACK);
	PrintScreen1();
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

int U_I::print_buttons_hold(uint8_t* lButtonsHold, uint8_t* rButtonsHold, bool* lButtonsIsHold, bool* rButtonsIsHold)
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
	for (int i = 0; i < 5; i++) {
		char strPrint[20];
		sprintf(strPrint, "%d\0", (int)lButtonsIsHold[i]);
		ST7789_WriteString(162, 2 + i * 9, strPrint, Font_7x9, WHITE, BLACK);
	}
	for (int i = 0; i < 5; i++) {
		char strPrint[20];
		sprintf(strPrint, "%d\0", (int)rButtonsIsHold[i]);
		ST7789_WriteString(162, 52 + i * 9, strPrint, Font_7x9, WHITE, BLACK);
	}
	return 0;
}

int U_I::PrintScreen1()
{
	ST7789_WriteString(0, 120, "nSat:", Font_7x9, WHITE, BLACK);
	ST7789_WriteString(0, 129, "lat:", Font_7x9, WHITE, BLACK);
	ST7789_WriteString(0, 138, "long:", Font_7x9, WHITE, BLACK);
	ST7789_WriteString(0, 147, "alt:", Font_7x9, WHITE, BLACK);
	ST7789_WriteString(0, 156, "speed:", Font_7x9, WHITE, BLACK);
	ST7789_WriteString(0, 165, "batVolt:", Font_7x9, WHITE, BLACK);
	ST7789_WriteString(0, 174, "time:", Font_7x9, WHITE, BLACK);
	ST7789_WriteString(0, 183, "resStat:", Font_7x9, WHITE, BLACK);
	ST7789_WriteString(0, 192, "trStat:", Font_7x9, WHITE, BLACK);
	return 0;
}

int U_I::Print_res_stat(bool stat)
{
	if (stat)
		ST7789_WriteString(56, 183, "ok          ", Font_7x9, WHITE, BLACK);
	else
		ST7789_WriteString(56, 183, "not response", Font_7x9, WHITE, BLACK);
	return 0;
}

int U_I::Print_tr_stat(bool stat)
{
	if (stat)
		ST7789_WriteString(49, 183, "ok  ", Font_7x9, WHITE, BLACK);
	else
		ST7789_WriteString(49, 183, "lock", Font_7x9, WHITE, BLACK);
	return 0;
}

int U_I::print_pack_A(int nSatellite, double latitude, double longitude, double batteryVoltage)
{
	char strPrint[20];
	sprintf(strPrint, "%d\0", nSatellite);
	ST7789_WriteString(35, 120, strPrint, Font_7x9, WHITE, BLACK);
	sprintf(strPrint, "%d.%d\0", (int)latitude, (int)((latitude - (int)latitude) * 100000) );
	ST7789_WriteString(28, 129, strPrint, Font_7x9, WHITE, BLACK);
	sprintf(strPrint, "%d.%d\0", (int)longitude, (int)((longitude - (int)longitude) * 100000));
	ST7789_WriteString(35, 138, strPrint, Font_7x9, WHITE, BLACK);
	sprintf(strPrint, "%d.%d\0", (int)batteryVoltage, (int)((batteryVoltage - (int)batteryVoltage) * 100));
	ST7789_WriteString(56, 165, strPrint, Font_7x9, WHITE, BLACK);
	return 0;
}

int U_I::print_pack_B(double altitude, double speed, char* timeStr)
{
	char strPrint[20];
	sprintf(strPrint, "%d.%d\0", (int)altitude, (int)((altitude - (int)altitude) * 100000));
	ST7789_WriteString(28, 147, strPrint, Font_7x9, WHITE, BLACK);
	sprintf(strPrint, "%d.%d\0", (int)speed, (int)((speed - (int)speed) * 100));
	ST7789_WriteString(42, 156, strPrint, Font_7x9, WHITE, BLACK);
	ST7789_WriteString(35, 174, timeStr, Font_7x9, WHITE, BLACK);
	return 0;
}

int U_I::PrintScreen2()
{
	return 0;
}