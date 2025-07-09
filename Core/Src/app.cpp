#include "main.h"
#include "stdio.h"
#include "app.h"
#include "string.h"
#include "Input_Output.h"
#include "User_Interface.h"
#include "LoRa.h"

uint32_t timerLed = 0;

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

	printf("Hi, Nick!\n");


	while (1)
	{
		adc.Handler(); //обработчик, готовящий данные с АЦП, обновляет поля класса
		lora.Handler(); //обработчик прерываний uart и событий в классе

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

		if (HAL_GetTick() - timerLed >= 400) {
			timerLed = HAL_GetTick();
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			//HAL_UART_Transmit_IT(&huart1, (uint8_t*)trStr, 32);
			//rsTimer = HAL_GetTick();
		}


	}
	return 0;
}






