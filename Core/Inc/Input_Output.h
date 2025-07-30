#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H

#include <stdint.h>

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
	uint8_t lButtonsPress[5] = { 0 };
	uint8_t rButtonsPress[5] = { 0 };
	uint8_t lButtonsHoldCounter[5] = { 0 };
	uint8_t rButtonsHoldCounter[5] = { 0 };
	bool lButtonsIsHold[5] = { 0 };
	bool rButtonsIsHold[5] = { 0 };
	int Init();
	int Handler(uint16_t lChannel, uint16_t rChannel);
private:
};

#endif

