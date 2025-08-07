#include "Input_Output.h"
#include "main.h"
#include "stdio.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

uint64_t adcTimer = 0;
bool adcIsRun = 0;
bool adcIsComplete = 0;
uint16_t adcData[ADC_CHANNELS_NUM];

uint64_t lButtTimerNoise = 0;
uint64_t lButtTimerHold = 0;
uint8_t lWhatButtHadPress = 0;
uint8_t lButtIsPress = 0;
uint64_t rButtTimerNoise = 0;
uint64_t rButtTimerHold = 0;
uint8_t rWhatButtHadPress = 0;
uint8_t rButtIsPress = 0;

static bool postBattAirAlarmLock = 0;
static uint64_t battAirAlarmTimer = 0;
static uint64_t battAirAlarmPeriodOfSignals = 0;
static uint64_t battAirAlarmFreq = 0;

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
        batVolt = 1.2 / adcDataChannel[0] * adcDataChannel[4] * 11.678;
        adcIsComplete = 0;
        this->isAdcComplete = 1;
    }

    if (HAL_GetTick() - adcTimer > 5 && adcIsRun == 0) {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, ADC_CHANNELS_NUM);
        adcIsRun = 1;
    }
    return 0;
}

int BUTTONS::Init()
{
    //HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_RESET);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    TIM2->CCR1 = 0; // включить светодиод на полную яркость
    TIM3->CCR1 = 0; // выключить пищалку (подать ноль)

    enBattAirAlarm = 1;
    return 0;
}

int BUTTONS::Handler(uint16_t lChannel, uint16_t rChannel)
{
    uint8_t lButton = 0;
    if (lChannel <= 360) { //определяем какая кнопка сейчас нажата
        lButton = 1;
    } else if (lChannel > 360 && lChannel <= 985) {
        lButton = 2;
    } else if (lChannel > 985 && lChannel <= 1675) {
        lButton = 3;
    } else if (lChannel > 1675 && lChannel <= 2335) {
        lButton = 4;
    } else if (lChannel > 2335 && lChannel <= 3100) {
        lButton = 5;
    }

    if (lButton > 0 && lWhatButtHadPress == 0) { //запоминаем состояние и запускаем таймер
        lWhatButtHadPress = lButton;
        lButtTimerNoise = HAL_GetTick();
    }

    if (HAL_GetTick() - lButtTimerNoise > 10 && lWhatButtHadPress > 0) { //после дребезга снова проверяем состояние
        if (lWhatButtHadPress == lButton && lButtIsPress == 0) { //если совпало, то это нужная кнопка
            lButtIsPress = lButton;
            this->lButtonsPress[lButton - 1] += 1;
            this->eventButtons = 1;
            lButtTimerHold = HAL_GetTick();
        }
        lWhatButtHadPress = 0;
    }

    if (HAL_GetTick() - lButtTimerHold > 200 && lButtIsPress == lButton && lButton != 0) {
        this->eventButtons = 1;
        this->lButtonsHoldCounter[lButton - 1] += 1;
        this->lButtonsIsHold[lButton - 1] = 1;
    }

    if (lButton == 0) {
        lButtIsPress = 0;
        for (uint8_t i = 0; i < 5; i++)
            this->lButtonsIsHold[i] = 0;
    }

    uint8_t rButton = 0;
    if (rChannel <= 315) {
        rButton = 1;
    } else if (rChannel > 315 && rChannel <= 985) {
        rButton = 2;
    } else if (rChannel > 985 && rChannel <= 1725) {
        rButton = 3;
    } else if (rChannel > 1725 && rChannel <= 2550) {
        rButton = 4;
    } else if (rChannel > 2550 && rChannel <= 3390) {
        rButton = 5;
    }

    if (rButton > 0 && rWhatButtHadPress == 0) {
        rWhatButtHadPress = rButton;
        rButtTimerNoise = HAL_GetTick();
    }

    if (HAL_GetTick() - rButtTimerNoise > 10 && rWhatButtHadPress > 0) {
        if (rWhatButtHadPress == rButton && rButtIsPress == 0) {
            rButtIsPress = rButton;
            this->rButtonsPress[rButton - 1] += 1;
            this->eventButtons = 1;
            rButtTimerHold = HAL_GetTick();
        }
        rWhatButtHadPress = 0;
    }

    if (HAL_GetTick() - rButtTimerHold > 200 && rButtIsPress == rButton && rButton != 0) {
        this->eventButtons = 1;
        this->rButtonsHoldCounter[rButton - 1] += 1;
        this->rButtonsIsHold[rButton - 1] = 1;
    }

    if (rButton == 0) {
        rButtIsPress = 0;
        for (uint8_t i = 0; i < 5; i++)
            this->rButtonsIsHold[i] = 0;
    }


    //далее идет обработка индикации через пищалку и светодиод

    if (enBattAirAlarm) {
        enBattAirAlarm = 0;
        battAirAlarmTimer = HAL_GetTick();
    }

    if (HAL_GetTick() - battAirAlarmTimer <= 2000) {
        //выполнять в течение 2 секунд
        
        if (HAL_GetTick() - battAirAlarmFreq >= 100) {
            //выполнять каждые 100 мс
            static bool trLg = 0;
            if (trLg == 0) {
                TIM3->CCR1 = 30; // максимальная громкость = 249
                trLg = 1;
            }
            else {
                TIM3->CCR1 = 0; // максимальная громкость = 249
                trLg = 0;
            }
            battAirAlarmFreq = HAL_GetTick();
            postBattAirAlarmLock = 0;
        }
    }

    if (HAL_GetTick() - battAirAlarmTimer > 2000 && postBattAirAlarmLock == 0) {
        postBattAirAlarmLock = 1;
        TIM3->CCR1 = 0;
        //HAL_GPIO_WritePin(buz_GPIO_Port, buz_Pin, GPIO_PIN_RESET);
    }


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

