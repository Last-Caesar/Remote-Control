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
	bool eventButtons = 0;
	uint8_t lButtonsPress[5];
	uint8_t rButtonsPress[5];
	uint8_t lbuttonsHold[5];
	uint8_t rbuttonsHold[5];
	int Init();
	int Handler(uint16_t lChannel, uint16_t rChannel);
private:
};

#endif

