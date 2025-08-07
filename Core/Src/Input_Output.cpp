#include "Input_Output.h"
#include "main.h"
#include "stdio.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

static uint64_t adcTimer = 0;
static bool adcIsRun = 0;
static bool adcIsComplete = 0;
static uint16_t adcData[ADC_CHANNELS_NUM];

static double adcVoltage[3] = { 0 };
static double bankVoltage[3] = { 0 };
static double bankVoltageSum[3] = { 0 };
static int countOfReads = 0;

int ADC::Init()
{
    return 0;
}

int ADC::Handler()
{
    if (adcIsComplete == 1)
    {
        adcIsComplete = 0;
        this->isAdcComplete = 1;
        countOfReads += 1;
        for (uint8_t i = 0; i < 3; i++)
        {
            //this->dataChannel[i] = adcData[i];
            adcVoltage[i] = 1.2 / adcData[0] * adcData[i + 1];
        }

        bankVoltage[0] = adcVoltage[0] * 5.91;
        bankVoltage[1] = adcVoltage[1] * 6.1786;
        bankVoltage[2] = adcVoltage[2] * 6.1865;

        bankVoltage[2] -= bankVoltage[1];
        bankVoltage[1] -= bankVoltage[0];

        for (uint8_t i = 0; i < 3; i++)
            bankVoltageSum[i] += bankVoltage[i];
        
    }

    if (HAL_GetTick() - adcTimer > 5 && adcIsRun == 0) {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, ADC_CHANNELS_NUM);
        adcIsRun = 1;
    }
    return 0;
}

double ADC::GetMinVolteOfPeriod()
{
    double bankVoltAver[3] = { 0 };
    double minVolt = 0;
    for (uint8_t i = 0; i < 3; i++)
        bankVoltAver[i] = bankVoltageSum[i] / countOfReads;
    if (bankVoltAver[0] < bankVoltAver[1])
        if (bankVoltAver[0] < bankVoltAver[2])
            minVolt = bankVoltAver[0];
        else
            minVolt = bankVoltAver[2];
    else
        if (bankVoltAver[1] < bankVoltAver[2])
            minVolt = bankVoltAver[1];
        else
            minVolt = bankVoltAver[2];
    for (uint8_t i = 0; i < 3; i++)
        bankVoltageSum[i] = 0;
    countOfReads = 0;

    return minVolt;
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

int PWM::Set_Null()
{
    TIM1->CCR1 = 1000;
    TIM1->CCR2 = 1700;
    TIM1->CCR3 = 1700;
    TIM1->CCR4 = 1700;
    TIM3->CCR1 = 1700;
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

