#include "PWM.h"

void TIM_Base_Config(TIM_TypeDef *TIMX, uint16_t Prescaler, uint16_t Period)
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

void TIM_PWM_Channel_Config(TIM_TypeDef *TIM_X, TIM_CHANNEL channel_x)
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

void GPIO_Mode_AF_PP_Config(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X)
{
    if (GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.GPIO_Pin = GPIO_Pin_X;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOx, &gpio_init_struct);
}
