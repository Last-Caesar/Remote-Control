#include "I_O.h"
#include "main.h"
#include "stdio.h"

#define ADC_CHANNELS_NUM 7

extern ADC_HandleTypeDef hadc1;

uint64_t adcTimer = 0;
bool adcIsRun = 0;
uint16_t adcData[ADC_CHANNELS_NUM];
float adcVoltage[ADC_CHANNELS_NUM];

int I_O_Init()
{

    return 0;
}

int I_O_Handler() 
{
    if (HAL_GetTick() - adcTimer > 10 && adcIsRun == 0) {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, ADC_CHANNELS_NUM);
        adcIsRun = 1;
    }
    return 0;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        for (uint8_t i = 0; i < ADC_CHANNELS_NUM; i++)
        {
            adcVoltage[i] = adcData[i] * 3.3 / 4095;
        }
        adcTimer = HAL_GetTick();
        adcIsRun = 0;
        HAL_ADC_Stop_DMA(&hadc1);
    }
}

