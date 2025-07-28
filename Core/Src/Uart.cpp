#include "main.h"

extern UART_HandleTypeDef huart1; // LoRa
extern UART_HandleTypeDef huart3; // GPS

static uint8_t Gps_RsStrPoint = 0;
static bool Gps_IsStrBegin = 0;
char Gps_IncomByte;
bool Gps_IsStrComplete = 0;
char Gps_RsStr[83];

static uint8_t LoRa_RsStrPoint = 0;
static bool LoRa_IsStrBegin = 0;
char LoRa_IncomByte;
bool LoRa_IsStrComplete = 0;
char LoRa_RsStr[32];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	if (huart == &huart1)
	{
		if (LoRa_IncomByte == '$')
		{
			LoRa_IsStrBegin = 1;
			LoRa_RsStrPoint = 0;
		}
		else if (LoRa_IncomByte == '\n')
		{
			LoRa_IsStrBegin = 0;
			LoRa_IsStrComplete = 1;
			LoRa_RsStr[LoRa_RsStrPoint] = LoRa_IncomByte;
			LoRa_RsStr[LoRa_RsStrPoint + 1] = '\0';
		}

		if (LoRa_IsStrBegin == 1)
		{
			LoRa_RsStr[LoRa_RsStrPoint] = LoRa_IncomByte;
			LoRa_RsStrPoint++;
		}

		if (LoRa_RsStrPoint >= 32)
		{
			LoRa_RsStrPoint = 0;
		}

		HAL_UART_Receive_IT(&huart1, (uint8_t*)&LoRa_IncomByte, 1);
	}
	if (huart == &huart3)
	{
		if (Gps_IncomByte == '$')
		{
			Gps_IsStrBegin = 1;
			Gps_RsStrPoint = 0;
		}
		else if (Gps_IncomByte == '\n')
		{
			Gps_IsStrBegin = 0;
			Gps_IsStrComplete = 1;
			Gps_RsStr[Gps_RsStrPoint] = Gps_IncomByte;
			Gps_RsStr[Gps_RsStrPoint + 1] = '\0';
		}

		if (Gps_IsStrBegin == 1)
		{
			Gps_RsStr[Gps_RsStrPoint] = Gps_IncomByte;
			Gps_RsStrPoint++;
		}

		if (Gps_RsStrPoint >= 83)
		{
			//err
			Gps_RsStrPoint = 0;
		}

		HAL_UART_Receive_IT(&huart3, (uint8_t*)&Gps_IncomByte, 1);
	}
}

