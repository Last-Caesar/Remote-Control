#include "LoRa.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern UART_HandleTypeDef huart1;

char rsStr[32];

uint32_t transmitTimer = 0;
uint32_t rsTimer;
char incomByte;
uint8_t rsStrPoint = 0;
bool isStrComplete = 0;
bool isStrBegin = 0;

char trStr[33] = "$,cc,ba,aa,cc,a\naaaaaaaaaaaaaaaa";

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
		if (!strncmp(rsStr, "$A,", 3))
		{
			// принят пакет типа A
			Package_Decoder_A(rsStr);
		}
		else if (!strncmp(rsStr, "$B,", 3))
		{
			// принят пакет типа B
			Package_Decoder_B(rsStr);
		}
		else
		{
			//err
		}
	}
	return 0;
}

int LoRa::Transmit_Package(uint8_t packetType, uint8_t channel1, uint8_t channel2, uint8_t channel3, uint8_t channel4, uint16_t param)
{
	//char trStr[32] = "$ab,a\naaaaaaaaaaaaaaaaaaaaaaaaa";

	if (packetType == 0) {
		sprintf(trStr, "$,%x,%x,%x,%x,%x\n", channel1, channel2, channel3, channel4, param);
	}
	else if (packetType == 1)
	{
		sprintf(trStr, "$a,%x,%x,%x,%x,%x\n", channel1, channel2, channel3, channel4, param);
	}
	else if (packetType == 2)
	{
		sprintf(trStr, "$b,%x,%x,%x,%x,%x\n", channel1, channel2, channel3, channel4, param);
	}
	HAL_UART_Transmit_IT(&huart1, (uint8_t*)trStr, 32);
	//HAL_UART_Transmit_IT(&huart1, (uint8_t*)tStr, 32);
	//HAL_UART_Transmit_IT(&huart1, (uint8_t*)"$,cc,ba,aa,cc,a\naaaaaaaaaaaaaaa", 32);

	return 0;
}

int LoRa::Package_Decoder_A(char* str)
{
	char batteryVoltageStr[15] = { 0 }; //напряжение акб
	char latitudeStr[15] = { 0 }; //широта
	char longitudeStr[15] = { 0 }; //долгота

	sscanf(str, "$%*[^,],%[^,],%[^,],%[^,],%d\n", batteryVoltageStr, latitudeStr, longitudeStr, &(this->nSatellite));
	this->batteryVoltage = atof(batteryVoltageStr);
	this->latitude = atof(latitudeStr);
	this->longitude = atof(longitudeStr);
	return 0;
}

int LoRa::Package_Decoder_B(char* str)
{
	char speed[15] = { 0 }; // скорость
	char altitude[15] = { 0 }; // высота
	char timeStr[15] = { 0 }; // время

	sscanf(str, "$%*[^,],%[^,],%[^,],%s\n", speed, altitude, this->timeStr);
	this->speed = atof(speed);
	this->altitude = atof(altitude);
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