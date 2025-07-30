#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H

#include "stdint.h"
#define ADC_CHANNELS_NUM 4

class ADC
{
public:
	bool isAdcComplete = 0;
	//uint16_t dataChannel[ADC_CHANNELS_NUM];
	//double batteryVoltage = 0;
	double GetMinVolteOfPeriod();
	int Init();
	int Handler();
private:

};

class PWM
{
public:
	int Init();
	int Handler();
	int Start();
	int Stop();
	int Set_Null();
private:

};


#endif

