#ifndef I_O_H
#define I_O_H

#include "main.h"
#define ADC_CHANNELS_NUM 7

class ADC
{
public:
	bool isAdcComplete = 0;
	uint16_t adcDataChannel[ADC_CHANNELS_NUM];
	int Init();
	int Handler();
private:

};

#endif

