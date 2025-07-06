#ifndef I_O_H
#define I_O_H

#include "main.h"
#define ADC_CHANNELS_NUM 8

class ADC
{
public:
	bool isAdcComplete = 0;
	uint16_t adcDataChannel[ADC_CHANNELS_NUM];
	int Init();
	int Handler();
private:

};

class BUTTONS
{
public:
	int what_button_pressed(uint8_t leftChannel, uint8_t rightChannel);
	uint8_t whatButtonPressed;
	int Init();
	int Handler();
private:
};

#endif

