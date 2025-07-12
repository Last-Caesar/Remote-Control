#include "LoRa.h"
#include "main.h"
#include "string.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;

char trStr[33] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

extern char LoRa_IncomByte;
extern bool LoRa_IsStrComplete;
extern char LoRa_RsStr[32];

int LoRa::Init()
{
	HAL_Delay(1000);

	HAL_UART_Receive_IT(&huart1, (uint8_t*)&LoRa_IncomByte, 1);
	return 0;
}

int LoRa::Handler()
{
	if (LoRa_IsStrComplete)
	{
		LoRa_IsStrComplete = 0;
		if (!strncmp(LoRa_RsStr, "$,", 2))
		{
			//принят обычный пакет (без запроса ответа)
			this->Package_Decoder(LoRa_RsStr);
			this->typeResPackage = 0; 
		}
		else if (!strncmp(LoRa_RsStr, "$A,", 3))
		{
			// пакет типа A
			this->Package_Decoder(LoRa_RsStr);
			this->typeResPackage = 1;
		}
		else if (!strncmp(LoRa_RsStr, "$B,", 3))
		{
			// пакет типа B
			this->Package_Decoder(LoRa_RsStr);
			this->typeResPackage = 2;
		}
		else
		{
			//err
		}
	}
	return 0;
}

int LoRa::Transmit_Package_A(uint16_t batteryVoltage, double latitude, double longitude, double altitude, int nSatellites)
{
	sprintf(trStr, "$A,%4d,%2d.%5d,%3d.%5d,%4d.%2d,%2d\n", batteryVoltage,
		(int)latitude, (int)((latitude - (int)latitude) * 100000),
		(int)longitude, (int)((longitude - (int)longitude) * 100000),
		(int)altitude, (int)((altitude - (int)altitude) * 100),
		nSatellites);
	HAL_UART_Transmit_IT(&huart1, (uint8_t*)trStr, 32);
	return 0;
}

int LoRa::Transmit_Package_B(double speed, char* timeStr)
{
	sprintf(trStr, "$B,%3d.%2d,%s\n", (int)speed, (int)((speed - (int)speed) * 100), timeStr);
	HAL_UART_Transmit_IT(&huart1, (uint8_t*)trStr, 32);
	return 0;
}

int LoRa::Package_Decoder(char* str)
{
	char strChannel1[3], strChannel2[3], strChannel3[3], strChannel4[3], strPref[5]; //газ, рыскание, тангаж, крен, строка с параметрами передаваемыми пакетом
	sscanf(str, "$%*[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", strChannel1, strChannel2, strChannel3, strChannel4, strPref);
	this->channel1 = hex2int(strChannel1);
	this->channel2 = hex2int(strChannel2);
	this->channel3 = hex2int(strChannel3);
	this->channel4 = hex2int(strChannel4);
	// +разбор strPref
	return 0;
}

uint32_t hex2int(char* hex)
{
	uint32_t val = 0;
	while (*hex) {
		// get current character then increment
		uint8_t byte = *hex++;
		// transform hex character to the 4bit equivalent number, using the ascii table indexes
		if (byte >= '0' && byte <= '9') byte = byte - '0';
		else if (byte >= 'a' && byte <= 'f') byte = byte - 'a' + 10;
		else if (byte >= 'A' && byte <= 'F') byte = byte - 'A' + 10;
		// shift 4 to make space for new digit, and add the 4 bits of the new digit
		val = (val << 4) | (byte & 0xF);
	}
	return val;
}
