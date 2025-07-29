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

uint32_t timerLostSignal_1000ms = 0;
bool lostSignal_1000ms_Lock = 0;
uint32_t timerLostSignal_1700ms = 0;
bool lostSignal_1700ms_Lock = 0;

uint8_t ledPwm = 400;

int app()
{
	LoRa lora;
	ADC adc;
	GPS gps;
	PWM pwm;
	CurrentData currentData;

	lora.Init();
	adc.Init();
	gps.Init();
	pwm.Init();

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
				pwm.Start();

			}
			// редактирование pwm
			ledPwm = lora.channel1;
			int temp = map(lora.channel1, 0, 255, 1000, 2000);
			TIM1->CCR1 = temp;
			TIM1->CCR2 = map(lora.channel2, 0, 255, 700, 2700);
			TIM1->CCR3 = map(lora.channel3, 0, 255, 700, 2700);
			TIM1->CCR4 = map(lora.channel4, 0, 255, 700, 2700);
			TIM3->CCR1 = map(lora.channel4, 0, 255, 2700, 700);
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
			//выполнять постоянно при потере сигнала

			if (isSignalLost_Lock == 1) {
				isSignalLost_Lock = 0;
				//выполнить один раз при потере сигнала
				timerLostSignal_1000ms = HAL_GetTick();
				lostSignal_1000ms_Lock = 0;
			}


			if (HAL_GetTick() - timerAutoPackage > 240) { 
				timerAutoPackage = HAL_GetTick();
				//выполнять постоянно при потере сигнала с периодом 240мс
				if (prevAutoPackage == 0) {
					lora.Transmit_Package_A(adc.batteryVoltage, gps.latitude, gps.longitude, gps.nSatellite);
					prevAutoPackage = 1;
				}
				else if (prevAutoPackage == 1) {
					lora.Transmit_Package_B(gps.speed, gps.altitude, gps.timeStr);
					prevAutoPackage = 0;
				}
			}

			if (HAL_GetTick() - timerLostSignal_1000ms >= 1000 && lostSignal_1000ms_Lock == 0) {
				lostSignal_1000ms_Lock = 1;
				//выполнить один раз через 1000мс после потери сигнала
				pwm.Set_Null();
				timerLostSignal_1700ms = HAL_GetTick();
				lostSignal_1700ms_Lock = 0;
			}

			if (HAL_GetTick() - timerLostSignal_1700ms >= 700 && lostSignal_1700ms_Lock == 0) {
				lostSignal_1700ms_Lock = 1;
				//выполнить один раз через 1700мс после потери сигнала
				pwm.Stop();
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

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double map(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}