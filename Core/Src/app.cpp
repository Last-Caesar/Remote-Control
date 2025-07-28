#include "app.h"
#include "main.h"
#include "stdio.h"
#include "LoRa.h"
#include "Input_Output.h"
#include "gps.h"

uint32_t timerLed = 0;
uint32_t timerResPackage = 0;
uint32_t timerAutoPackage = 0;
uint8_t prevAutoPackage = 0;
bool isSignalLost_Lock = 0;

uint8_t ledPwm = 400;

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

		if (HAL_GetTick() - timerLed >= ledPwm)
		{
			timerLed = HAL_GetTick();
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}

		if (lora.packageIsAvailable) {
			lora.packageIsAvailable = 0;
			timerResPackage = HAL_GetTick();
			if (isSignalLost_Lock == 0) {
				isSignalLost_Lock = 1;
				//выпоныть один раз при появлении сигнала
				//pwm start
			}
			// редактирование pwm
			ledPwm = lora.channel1;
			//printf("%d\n", lora.channel1);
			if (lora.typeResPackage == 1) {
				//currentData.lock = 1;
				lora.Transmit_Package_A(adc.batteryVoltage, gps.latitude, gps.longitude, gps.nSatellite);
			}
			else if (lora.typeResPackage == 2) {
				//currentData.lock = 0;
				lora.Transmit_Package_B(gps.speed, gps.altitude, gps.timeStr);
			}
		}

		if (HAL_GetTick() - timerResPackage > 100) {
			if (HAL_GetTick() - timerAutoPackage > 240) { 
				timerAutoPackage = HAL_GetTick();
				//выполнять постоянно при потере сигнала
				if (prevAutoPackage == 0) {
					lora.Transmit_Package_A(adc.batteryVoltage, gps.latitude, gps.longitude, gps.nSatellite);
					prevAutoPackage = 1;
				}
				else if (prevAutoPackage == 1) {
					lora.Transmit_Package_B(gps.speed, gps.altitude, gps.timeStr);
					prevAutoPackage = 0;
				}
			}

			if (isSignalLost_Lock == 1) {
				isSignalLost_Lock = 0;
				//выполнить один раз при потере сигнала
				//pwm stop
			}
		}


		/*if (adc.isAdcComplete) {
			if (!currentData.lock) {
				for (int i = 0; i < 3; i++) {
					currentData.adcCurrentData[i] = adc.dataChannel[i];
				}
			}

			adc.isAdcComplete = 0;
		}*/

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


