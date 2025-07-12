#ifndef LORA_H
#define LORA_H
#include "stdint.h"

class LoRa
{
public:
	int Init();
	int Handler();
	int Package_Decoder(char* str);
	int Transmit_Package_A(uint16_t batteryVoltage, double latitude, double longitude, double altitude, int nSatellites);
	int Transmit_Package_B(double speed, char* timeStr);
	uint8_t typeResPackage = 0; // 0 - обычный, 1 - запрос пакета A, 2 - запрос пакета B
	bool packageIsAvailable = 0;
	uint8_t channel1, channel2, channel3, channel4;
	uint16_t preferences;
private:

};

uint32_t hex2int(char* hex);

#endif // LORA_H
