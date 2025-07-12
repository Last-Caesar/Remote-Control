#include "app.h"
#include "main.h"
#include "stdio.h"
#include "LoRa.h"
#include "Input_Output.h"
#include "gps.h"

extern UART_HandleTypeDef huart3;

uint32_t timerLed = 0;


int app()
{
	LoRa lora;
	ADC adc;
	GPS gps;
	CurrentData currentData;

	lora.Init();
	adc.Init();
	gps.Init();

	printf("Hi, Nick!\n");

	while (1)
	{
		
		lora.Handler();
		adc.Handler();
		gps.Handler();

		if (HAL_GetTick() - timerLed >= 400)
		{
			timerLed = HAL_GetTick();
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}

		if (lora.packageIsAvailable) {
			// редактирование pwm
			if (lora.typeResPackage == 1) {
				currentData.lock = 1;
				//lora.Transmit_Package_A();
			}
			else if (lora.typeResPackage == 2) {
				currentData.lock = 0;
				//lora.Transmit_Package_B();
			}
			lora.packageIsAvailable = 0;
		}

		if (adc.isAdcComplete) {
			if (!currentData.lock) {
				for (int i = 0; i < 3; i++) {
					currentData.adcCurrentData[i] = adc.dataChannel[i];
				}
			}

			adc.isAdcComplete = 0;
		}

		/*if (gps.isGpsComplete) {
			if (!currentData.lock) {
				for (int i = 0; i < 3; i++) {
					currentData.gpsCurrentData[i] = gps.DataChannel[i];
				}
			}

			gps.isGpsComplete = 0;
		}*/

		
	}
	return 0;
}


