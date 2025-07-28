#include "Input_Output.h"
#include "main.h"
#include "stdio.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

uint64_t adcTimer = 0;
bool adcIsRun = 0;
bool adcIsComplete = 0;
uint16_t adcData[ADC_CHANNELS_NUM];


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
            this->dataChannel[i] = adcData[i];
        }
        adcIsComplete = 0;
        this->isAdcComplete = 1;
    }

    if (HAL_GetTick() - adcTimer > 5 && adcIsRun == 0) {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, ADC_CHANNELS_NUM);
        adcIsRun = 1;
    }
    return 0;
}

int PWM::Init()
{

    return 0;
}

int PWM::Start()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    return 0;
}

int PWM::Stop()
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
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

