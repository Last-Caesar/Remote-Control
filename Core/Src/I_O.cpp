#include "I_O.h"
#include "main.h"
#include "stdio.h"

uint64_t adcTimer = 0;
bool adcIsRun = 0;
bool adcIsComplete = 0;
uint16_t adcData[ADC_CHANNELS_NUM];

extern ADC_HandleTypeDef hadc1;

int ADC::Init()
{

    return 0;
}

int ADC::Handler() 
{
    if (adcIsComplete == 1)
    {   
        for (uint8_t i = 0; i < ADC_CHANNELS_NUM; i++)
        {
            this->adcDataChannel[i] = adcData[i];
        }
        adcIsComplete = 0;
        this->isAdcComplete = 1;
    }

    if (HAL_GetTick() - adcTimer > 100 && adcIsRun == 0) {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, ADC_CHANNELS_NUM);
        adcIsRun = 1;
    }
    return 0;
}

int BUTTONS::what_button_pressed(uint8_t leftChannel, uint8_t rightChannel)
{
    return 0;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        adcTimer = HAL_GetTick();
        adcIsRun = 0;
        adcIsComplete = 1;
        HAL_ADC_Stop_DMA(&hadc1);
    }
}

