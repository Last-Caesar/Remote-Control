#include "main.h"
#include "stdio.h"
#include "app.h"
#include "string.h"
#include "st7789v0.h"
#include "I_O.h"
#include "User_Interface.h"

extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi2_tx;
extern UART_HandleTypeDef huart1;

uint32_t rsTimer;
uint32_t timerLed = 0;
char incomByte;
uint8_t rsStrPoint = 0;
bool isStrComplete = 0;
bool isStrBegin = 0;


char trStr[34] = "$ABC\naaaaaaaaaaaaaaaaaaaaaaaaaaaa";
char rsStr[32];

int app()
{ 
	ADC adc;
	BUTTONS buttons;

	adc.Init();
	U_I_Init();

	printf("Hi, Nick!\n");

	int j = 0;

	HAL_Delay(1000);
	rsTimer = HAL_GetTick();
	//HAL_UART_Transmit_IT(&huart1, (uint8_t*)trStr, 33);

	HAL_UART_Receive_IT(&huart1, (uint8_t*)&incomByte, 1);
	while (1)
	{
		adc.Handler();

		if (adc.isAdcComplete == 1) {
			for (uint8_t i = 0; i < 8; i++)
			{
				char strPrint[20];
				sprintf(strPrint, "%d: %d    \0", i, adc.adcDataChannel[i]);
				ST7789_WriteString(5, 25 + i * 9, strPrint, Font_7x9, WHITE, BLACK);
			}
			buttons.what_button_pressed(adc.adcDataChannel[1], adc.adcDataChannel[2]);

			j++;
			char strPrint[20];
			sprintf(strPrint, "%d\0", j);
			ST7789_WriteString(2, 2, strPrint, Font_7x9, WHITE, BLACK);

			adc.isAdcComplete = 0;
		}

		if (HAL_GetTick() - timerLed >= 400) {
			timerLed = HAL_GetTick();
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			//HAL_UART_Transmit_IT(&huart1, (uint8_t*)trStr, 32);
			rsTimer = HAL_GetTick();
		}


		if (isStrComplete) {
			isStrComplete = 0;
			if (!strncmp(rsStr, "$ABC\n", 4))
			{
				char strPrint[20];
				sprintf(strPrint, "resTime: %d\0", HAL_GetTick() - rsTimer);
				ST7789_WriteString(25, 15, strPrint, Font_11x18, WHITE, GREEN);
			}
			else
			{
				//err
			}
		}

		

	}
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



