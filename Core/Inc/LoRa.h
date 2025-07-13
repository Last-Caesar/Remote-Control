#ifndef LORA_H
#define LORA_H
#include "stdint.h"

class LoRa
{
public:
	int Init();
	int Handler();
	int Transmit_Package(uint8_t packetType, uint8_t channel1, uint8_t channel2, uint8_t channel3, uint8_t channel4, uint16_t param);
	int Package_Decoder_A(char* str);
	int Package_Decoder_B(char* str);
	double batteryVoltage = 0;   //напряжение акб
	double latitude = 0;   //широта
	double longitude = 0;   //долгота
	int nSatellite = 0; //кол. спутников
	double altitude = 0; //высота над уровнем моря
	double speed = 0;
	char timeStr[15] = { 0 }; //время


private:

};



#endif // !LORA_H
