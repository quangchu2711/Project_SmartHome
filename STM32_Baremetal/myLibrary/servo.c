/*******************************************************************************
* Definitions
******************************************************************************/
#include <stdint.h>
#include "stm32f10x.h"
#include "servo.h"
#define PSC 99
#define PERIOD 14399
#define VALUE_FOR_DUTY_CYCLE_0_5_MS 360
#define VALUE_FOR_DUTY_CYCLE_2_5_MS 1800

typedef enum
{
    CHANNEL_1,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4
}SERVO_TimmerChannel_t;
/*******************************************************************************
* Prototypes
******************************************************************************/
static uint16_t SERVO_Map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);
static void SERVO_TimeBaseConfig(TIM_TypeDef *TIMX, uint16_t Prescaler, uint16_t Period);
static void SERVO_PWMChannelConfig(TIM_TypeDef *TIM_X, SERVO_TimmerChannel_t channel_x);
static void SERVO_GPIOModeConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X);
/*******************************************************************************
* Code
******************************************************************************/
static uint16_t SERVO_Map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void SERVO_TimeBaseConfig(TIM_TypeDef *TIMX, uint16_t Prescaler, uint16_t Period)
{

    if (TIMX == TIM2)    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    else if (TIMX == TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    else if (TIMX == TIM4) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = Prescaler;
    TIM_TimeBaseStructure.TIM_Period = Period;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIMX, &TIM_TimeBaseStructure);
    TIM_Cmd(TIMX, ENABLE);
}

static void SERVO_PWMChannelConfig(TIM_TypeDef *TIM_X, SERVO_TimmerChannel_t channel_x)
{
    TIM_OCInitTypeDef  oc_init_struct;
    oc_init_struct.TIM_Pulse = TIM_CKD_DIV1;
    oc_init_struct.TIM_OCMode = TIM_OCMode_PWM1;
    oc_init_struct.TIM_OCPolarity = TIM_OCPolarity_High;
    oc_init_struct.TIM_OutputState = TIM_OutputState_Enable;

    switch (channel_x)
    {
    case CHANNEL_1:
        TIM_OC1Init(TIM_X, &oc_init_struct);
        if (TIM_X == TIM2) SERVO_GPIOModeConfig(GPIOA, GPIO_Pin_0);
        else if (TIM_X == TIM3) SERVO_GPIOModeConfig(GPIOA, GPIO_Pin_6);
        else if (TIM_X == TIM4) SERVO_GPIOModeConfig(GPIOB, GPIO_Pin_6);
        break;
    case CHANNEL_2:
        TIM_OC2Init(TIM_X, &oc_init_struct);
        if (TIM_X == TIM2) SERVO_GPIOModeConfig(GPIOA, GPIO_Pin_1);
        else if (TIM_X == TIM3) SERVO_GPIOModeConfig(GPIOA, GPIO_Pin_7);
        else if (TIM_X == TIM4) SERVO_GPIOModeConfig(GPIOB, GPIO_Pin_7);
        break;
    case CHANNEL_3:
        TIM_OC3Init(TIM_X, &oc_init_struct);
        if (TIM_X == TIM2) SERVO_GPIOModeConfig(GPIOA, GPIO_Pin_2);
        else if (TIM_X == TIM3) SERVO_GPIOModeConfig(GPIOB, GPIO_Pin_0);
        else if (TIM_X == TIM4) SERVO_GPIOModeConfig(GPIOB, GPIO_Pin_8);
        break;
    case CHANNEL_4:
        TIM_OC4Init(TIM_X, &oc_init_struct);
        if (TIM_X == TIM2) SERVO_GPIOModeConfig(GPIOA, GPIO_Pin_3);
        else if (TIM_X == TIM3) SERVO_GPIOModeConfig(GPIOB, GPIO_Pin_1);
        else if (TIM_X == TIM4) SERVO_GPIOModeConfig(GPIOB, GPIO_Pin_9);
        break;
    }
}

static void SERVO_GPIOModeConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X)
{
    if (GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.GPIO_Pin = GPIO_Pin_X;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOx, &gpio_init_struct);
}


void SERVO_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X)
{
    switch (GPIO_Pin_X)
    {
    case GPIO_Pin_0:
        if (GPIOx == GPIOA)
        {
            SERVO_TimeBaseConfig(TIM2, PSC, PERIOD);
            SERVO_PWMChannelConfig(TIM2, CHANNEL_1);
        }
        else if (GPIOx == GPIOB)
        {
            SERVO_TimeBaseConfig(TIM3, PSC, PERIOD);
            SERVO_PWMChannelConfig(TIM3, CHANNEL_3);
        }
        break;
    case GPIO_Pin_1:
        if (GPIOx == GPIOA)
        {
            SERVO_TimeBaseConfig(TIM2, PSC, PERIOD);
            SERVO_PWMChannelConfig(TIM2, CHANNEL_2);
        }
        else if (GPIOx == GPIOB)
        {
            SERVO_TimeBaseConfig(TIM3, PSC, PERIOD);
            SERVO_PWMChannelConfig(TIM3, CHANNEL_4);
        }
        break;
    case GPIO_Pin_2:
        SERVO_TimeBaseConfig(TIM2, PSC, PERIOD);
        SERVO_PWMChannelConfig(TIM2, CHANNEL_3);
        break;
    case GPIO_Pin_3:
        SERVO_TimeBaseConfig(TIM2, PSC, PERIOD);
        SERVO_PWMChannelConfig(TIM2, CHANNEL_4);
        break;
    case GPIO_Pin_6:
        if (GPIOx == GPIOA)
        {
            SERVO_TimeBaseConfig(TIM3, PSC, PERIOD);
            SERVO_PWMChannelConfig(TIM3, CHANNEL_1);
        }
        else if (GPIOx == GPIOB)
        {
            SERVO_TimeBaseConfig(TIM4, PSC, PERIOD);
            SERVO_PWMChannelConfig(TIM4, CHANNEL_1);
        }
        break;
    case GPIO_Pin_7:
        if (GPIOx == GPIOA)
        {
            SERVO_TimeBaseConfig(TIM3, PSC, PERIOD);
            SERVO_PWMChannelConfig(TIM3, CHANNEL_2);
        }
        else if (GPIOx == GPIOB)
        {
            SERVO_TimeBaseConfig(TIM4, PSC, PERIOD);
            SERVO_PWMChannelConfig(TIM4, CHANNEL_2);
        }
        break;
    case GPIO_Pin_8:
        SERVO_TimeBaseConfig(TIM4, PSC, PERIOD);
        SERVO_PWMChannelConfig(TIM4, CHANNEL_3);
        break;
    case GPIO_Pin_9:
        SERVO_TimeBaseConfig(TIM4, PSC, PERIOD);
        SERVO_PWMChannelConfig(TIM4, CHANNEL_4);
        break;
    }
}

void SERVO_Write(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X, int16_t pos)
{
    if (pos < 0) pos = 0;
    else if (pos > 180) pos = 180;
    uint16_t compare_value = SERVO_Map(pos, 0, 180, VALUE_FOR_DUTY_CYCLE_0_5_MS, VALUE_FOR_DUTY_CYCLE_2_5_MS);

    switch (GPIO_Pin_X)
    {
    case GPIO_Pin_0:
        if (GPIOx == GPIOA)
        {
            TIM2->CCR1 = compare_value;
        }
        else if (GPIOx == GPIOB)
        {
            TIM3->CCR3 = compare_value;
        }
        break;
    case GPIO_Pin_1:
        if (GPIOx == GPIOA)
        {
            TIM2->CCR2 = compare_value;
        }
        else if (GPIOx == GPIOB)
        {
            TIM3->CCR4 = compare_value;
        }
        break;
    case GPIO_Pin_2:
        TIM2->CCR3 = compare_value;
        break;
    case GPIO_Pin_3:
        TIM2->CCR4 = compare_value;
        break;
    case GPIO_Pin_6:
        if (GPIOx == GPIOA)
        {
            TIM3->CCR1 = compare_value;
        }
        else if (GPIOx == GPIOB)
        {
            TIM4->CCR1 = compare_value;
        }
        break;
    case GPIO_Pin_7:
        if (GPIOx == GPIOA)
        {
            TIM3->CCR2 = compare_value;
        }
        else if (GPIOx == GPIOB)
        {
            TIM4->CCR2 = compare_value;
        }
        break;
    case GPIO_Pin_8:
        TIM4->CCR3 = compare_value;
        break;
    case GPIO_Pin_9:
        TIM4->CCR4 = compare_value;
        break;
    }
}

