#include "main.h"
#include "stdio.h"
#include "app.h"
#include "string.h"
#include "Input_Output.h"
#include "User_Interface.h"
#include "LoRa.h"

uint32_t timerLed = 0;
uint32_t timerTransmitPackage = 0;

int app()
{ 
	ADC adc;
	BUTTONS buttons;
	U_I gui;
	LoRa lora;

	adc.Init();
	buttons.Init();
	gui.Init();
	lora.Init();

	HAL_Delay(500);
	printf("Hi, Nick!\n");


	while (1)
	{
		adc.Handler(); //обработчик, готовящий данные с АЦП, обновляет поля класса
		lora.Handler(); //обработчик прерываний uart и событий в классе

		if (HAL_GetTick() - timerLed >= 400) {
			timerLed = HAL_GetTick();
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			//HAL_UART_Transmit_IT(&huart1, (uint8_t*)trStr, 32);
			//rsTimer = HAL_GetTick();
		}

		if (HAL_GetTick() - timerTransmitPackage >= 55) {
			timerTransmitPackage = HAL_GetTick();
			uint8_t channel1 = map(adc.adcDataChannel[6], 0, 4095, 0, 255); // газ 
			uint8_t channel2 = map(adc.adcDataChannel[2], 0, 4095, 0, 255); // рыскание
			uint8_t channel3 = map(adc.adcDataChannel[5], 0, 4095, 0, 255); // тангаж
			uint8_t channel4 = map(adc.adcDataChannel[3], 0, 4095, 0, 255); // крен
			lora.Transmit_Package(0, channel1, channel2, channel3, channel4, 1);
		}

		if (adc.isAdcComplete == 1) {
			buttons.Handler(adc.adcDataChannel[7], adc.adcDataChannel[1]); //обработчик кнопок, обновляет поля класса
			
			gui.print_adc_Channel(adc.adcDataChannel);
			if (buttons.eventButtons == 1) {
				gui.print_buttons_press(buttons.lButtonsPress, buttons.rButtonsPress);
				buttons.eventButtons = 0;
			}

			gui.print_buttons_hold(buttons.lButtonsHold, buttons.rButtonsHold);
			
			adc.isAdcComplete = 0;
		}

		


	}
	return 0;
}

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}




