#include "main.h"
#include "stdio.h"
#include "app.h"
#include "string.h"
#include "Input_Output.h"
#include "User_Interface.h"
#include "LoRa.h"

#include "st7789v0.h"

uint32_t timerLed = 0;
uint32_t timerTransmitPackage = 0;
uint32_t timerReqPack = 0;

uint8_t counterReqPack = 0;
bool reqPack_Lock = 0;
uint8_t typePrevPack = 2;

int countTrInfoPackA = 0;
int countTrInfoPackB = 0;

bool lockEvent_Lock = 0;
bool trIsLock = 1;

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
		adc.Handler(); //обработчик, готов€щий данные с ј÷ѕ, обновл€ет пол€ класса
		lora.Handler(); //обработчик прерываний uart и событий в классе

		if (HAL_GetTick() - timerLed >= 400) {
			timerLed = HAL_GetTick();
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			//HAL_UART_Transmit_IT(&huart1, (uint8_t*)trStr, 32);
			//rsTimer = HAL_GetTick();
		}

		if (HAL_GetTick() - timerTransmitPackage >= 55 && reqPack_Lock == 0 && trIsLock == 0) {
			timerTransmitPackage = HAL_GetTick();
			counterReqPack++;
			uint8_t channel1 = map(adc.adcDataChannel[6], 0, 4095, 0, 255); // газ 
			uint8_t channel2 = map(adc.adcDataChannel[2], 0, 4095, 0, 255); // рыскание
			uint8_t channel3 = map(adc.adcDataChannel[5], 0, 4095, 0, 255); // тангаж
			uint8_t channel4 = map(adc.adcDataChannel[3], 0, 4095, 0, 255); // крен
			if (counterReqPack >= 20) {
				counterReqPack = 0;
				reqPack_Lock = 1;
				timerReqPack = HAL_GetTick();
				if (typePrevPack == 2) {
					lora.Transmit_Package(1, channel1, channel2, channel3, channel4, 1);

					char strPrint[20];
					sprintf(strPrint, "%3d\0", countTrInfoPackA++);
					ST7789_WriteString(120, 129, strPrint, Font_7x9, WHITE, BLACK);

					typePrevPack = 1;
				}
				else if (typePrevPack == 1) {
					lora.Transmit_Package(2, channel1, channel2, channel3, channel4, 1);

					char strPrint[20];
					sprintf(strPrint, "%3d\0", countTrInfoPackB++);
					ST7789_WriteString(150, 129, strPrint, Font_7x9, WHITE, BLACK);

					typePrevPack = 2;
				}
			}
			else
				lora.Transmit_Package(0, channel1, channel2, channel3, channel4, 1); 
		}

		//if (counterReqPack >= 10) {    //выполн€ть один раз на каждые 10 обычных пакетов
		//	counterReqPack = 0;
		//	reqPack_Lock = 1;
		//	timerReqPack = HAL_GetTick();
		//	if (typePrevPack == 2) {
		//		uint8_t channel1 = map(adc.adcDataChannel[6], 0, 4095, 0, 255); // газ 
		//		uint8_t channel2 = map(adc.adcDataChannel[2], 0, 4095, 0, 255); // рыскание
		//		uint8_t channel3 = map(adc.adcDataChannel[5], 0, 4095, 0, 255); // тангаж
		//		uint8_t channel4 = map(adc.adcDataChannel[3], 0, 4095, 0, 255); // крен
		//		lora.Transmit_Package(1, channel1, channel2, channel3, channel4, 1);
		//		typePrevPack = 1;
		//	} else if (typePrevPack == 1) {
		//		uint8_t channel1 = map(adc.adcDataChannel[6], 0, 4095, 0, 255); // газ 
		//		uint8_t channel2 = map(adc.adcDataChannel[2], 0, 4095, 0, 255); // рыскание
		//		uint8_t channel3 = map(adc.adcDataChannel[5], 0, 4095, 0, 255); // тангаж
		//		uint8_t channel4 = map(adc.adcDataChannel[3], 0, 4095, 0, 255); // крен
		//		lora.Transmit_Package(2, channel1, channel2, channel3, channel4, 1);
		//		typePrevPack = 2;
		//	}
		//}

		if (lora.packageIsAviable) {
			//прин€ли пакет с данными
			lora.packageIsAviable = 0;
			reqPack_Lock = 0;
			gui.Print_res_stat(1);
			uint32_t timeReqPack = HAL_GetTick() - timerReqPack;
			//здесь должен быть запрс rssi
			if (lora.typeResPack == 1) {
				gui.print_pack_A(lora.nSatellite, lora.latitude, lora.longitude, lora.batteryVoltage);
				
				if (timeReqPack < 130) {
					char strPrint[20];
					sprintf(strPrint, "%3d \0", timeReqPack);
					ST7789_WriteString(120, 120, strPrint, Font_7x9, WHITE, BLACK);
				}
			}
			else if (lora.typeResPack == 2) {
				gui.print_pack_B(lora.altitude, lora.speed, lora.timeStr);

				if (timeReqPack < 130) {
					char strPrint[20];
					sprintf(strPrint, "%3d \0", timeReqPack);
					ST7789_WriteString(150, 120, strPrint, Font_7x9, WHITE, BLACK);
				}
			}
		}

		if (HAL_GetTick() - timerReqPack >= 130 && reqPack_Lock == 1) {   
			//если сюда попали, то св€зь потер€на
			reqPack_Lock = 0;
			//вывести сообщение о потере св€зи
			gui.Print_res_stat(0);
		}

		if (adc.isAdcComplete == 1) {
			buttons.Handler(adc.adcDataChannel[7], adc.adcDataChannel[1]); //обработчик кнопок, обновл€ет пол€ класса
			
			gui.print_adc_Channel(adc.adcDataChannel);
			if (buttons.eventButtons == 1) {
				gui.print_buttons_press(buttons.lButtonsPress, buttons.rButtonsPress);
				buttons.eventButtons = 0;
			}
			gui.print_buttons_hold(buttons.lButtonsHoldCounter, buttons.rButtonsHoldCounter, buttons.lButtonsIsHold, buttons.rButtonsIsHold);
			//вывести напр€жение акб
			if (buttons.lButtonsIsHold[3] == 1 && buttons.rButtonsIsHold[3] == 1 && lockEvent_Lock == 0) { //выполн€етс€ каждый раз когда зажаты обе кнопки
				lockEvent_Lock = 1;
				if (trIsLock) {
					gui.Print_tr_stat(0);
					trIsLock = 0;
				}
				else {
					gui.Print_tr_stat(1);
					trIsLock = 1;
				}
			}

			if ((buttons.lButtonsIsHold[3] == 0 || buttons.rButtonsIsHold[3] == 0) && lockEvent_Lock == 1) { //выполн€етс€ когда одна из кнопок отпущена
				lockEvent_Lock = 0;
			}

			adc.isAdcComplete = 0;
		}

		


	}
	return 0;
}

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}




