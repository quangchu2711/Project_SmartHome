#include <stdint.h>
#include "stm32f10x.h"
#include "appServo.h"

#define PSC 99
#define PERIOD 14399
#define VALUE_FOR_DUTY_CYCLE_0_5_MS 360      //0.5 ms
#define VALUE_FOR_DUTY_CYCLE_2_5_MS 1800     //2.5 ms

typedef enum
{
    CHANNEL_1,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4
}TIM_CHANNEL;


static void TIM_Base_Config(TIM_TypeDef *TIMX, uint16_t Prescaler, uint16_t Period);
static void GPIO_Mode_AF_PP_Config(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X);
static void TIM_PWM_Channel_Config(TIM_TypeDef *TIM_X, TIM_CHANNEL channel_x);
static uint16_t Map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);

static void TIM_Base_Config(TIM_TypeDef *TIMX, uint16_t Prescaler, uint16_t Period)
{

    if (TIMX == TIM2)   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
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

static void GPIO_Mode_AF_PP_Config(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X)
{
    if (GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.GPIO_Pin = GPIO_Pin_X;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOx, &gpio_init_struct);
}

static void TIM_PWM_Channel_Config(TIM_TypeDef *TIM_X, TIM_CHANNEL channel_x)
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
        if (TIM_X == TIM2) GPIO_Mode_AF_PP_Config(GPIOA, GPIO_Pin_0);
        else if (TIM_X == TIM3) GPIO_Mode_AF_PP_Config(GPIOA, GPIO_Pin_6);
        else if (TIM_X == TIM4) GPIO_Mode_AF_PP_Config(GPIOB, GPIO_Pin_6);
        break;
    case CHANNEL_2:
        TIM_OC2Init(TIM_X, &oc_init_struct);
        if (TIM_X == TIM2) GPIO_Mode_AF_PP_Config(GPIOA, GPIO_Pin_1);
        else if (TIM_X == TIM3) GPIO_Mode_AF_PP_Config(GPIOA, GPIO_Pin_7);
        else if (TIM_X == TIM4) GPIO_Mode_AF_PP_Config(GPIOB, GPIO_Pin_7);
        break;
    case CHANNEL_3:
        TIM_OC3Init(TIM_X, &oc_init_struct);
        if (TIM_X == TIM2) GPIO_Mode_AF_PP_Config(GPIOA, GPIO_Pin_2);
        else if (TIM_X == TIM3) GPIO_Mode_AF_PP_Config(GPIOB, GPIO_Pin_0);
        else if (TIM_X == TIM4) GPIO_Mode_AF_PP_Config(GPIOB, GPIO_Pin_8);
        break;
    case CHANNEL_4:
        TIM_OC4Init(TIM_X, &oc_init_struct);
        if (TIM_X == TIM2) GPIO_Mode_AF_PP_Config(GPIOA, GPIO_Pin_3);
        else if (TIM_X == TIM3) GPIO_Mode_AF_PP_Config(GPIOB, GPIO_Pin_1);
        else if (TIM_X == TIM4) GPIO_Mode_AF_PP_Config(GPIOB, GPIO_Pin_9);
        break;
    }
}

static uint16_t Map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void AppServo_Init(void)
{
    TIM_Base_Config(TIM4, PSC, PERIOD);
    TIM_PWM_Channel_Config(TIM4, CHANNEL_1);
    TIM_PWM_Channel_Config(TIM4, CHANNEL_2);
}

void AppServo_Control(AppServo_Select_t servoSelect, uint8_t pos)
{
    uint16_t compare_value;

    if (pos < 0) pos = 0;
    else if (pos > 180) pos = 180;
    compare_value = Map(pos, 0, 180, VALUE_FOR_DUTY_CYCLE_0_5_MS, VALUE_FOR_DUTY_CYCLE_2_5_MS);

    if (servoSelect == SERVO_ControlDoor)
    {
        TIM4->CCR1 = compare_value;
    }
    else if (servoSelect == SERVO_Autofeed)
    {
        TIM4->CCR2 = compare_value;
    }
}
