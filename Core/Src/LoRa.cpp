#include "LoRa.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern UART_HandleTypeDef huart1;

char trStr[34] = "$ABC\naaaaaaaaaaaaaaaaaaaaaaaaaaaa";
char rsStr[32];

uint32_t transmitTimer = 0;
uint32_t rsTimer;
char incomByte;
uint8_t rsStrPoint = 0;
bool isStrComplete = 0;
bool isStrBegin = 0;

int LoRa::Init()
{
	//HAL_UART_Transmit_IT(&huart1, (uint8_t*)trStr, 33);
	rsTimer = HAL_GetTick();
	HAL_UART_Receive_IT(&huart1, (uint8_t*)&incomByte, 1);
	return 0;
}

int LoRa::Handler()
{
	if (isStrComplete) {
		isStrComplete = 0;
		if (!strncmp(rsStr, "$ABC\n", 4))
		{
			char strPrint[20];
			sprintf(strPrint, "resTime: %d\0", HAL_GetTick() - rsTimer);
			//ST7789_WriteString(25, 15, strPrint, Font_11x18, WHITE, GREEN);
		}
		else
		{
			//err
		}
	}
	return 0;
}

int LoRa::Transmit()
{
	return 0;
}

int LoRa::Reserve()
{
	return 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	if (huart == &huart1)
	{
		if (incomByte == '$') {
			isStrBegin = 1;
			rsStrPoint = 0;
		}
		else if (incomByte == '\n') {
			isStrBegin = 0;
			isStrComplete = 1;
			rsStr[rsStrPoint] = incomByte;
			rsStr[rsStrPoint + 1] = '\0';
		}

		if (isStrBegin == 1) {
			rsStr[rsStrPoint] = incomByte;
			rsStrPoint++;
		}

		if (rsStrPoint >= 32) {
			rsStrPoint = 0;
		}

		HAL_UART_Receive_IT(&huart1, (uint8_t*)&incomByte, 1);
	}
}