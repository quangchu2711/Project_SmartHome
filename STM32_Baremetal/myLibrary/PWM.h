/**
  ******************************************************************************
  * @file    PWM.h
  * @author  Chu Xuan Quang
  * @date    24-1-2022
  * @brief   PWM program body
  ******************************************************************************
  * @attention Timer2: PA0, PA1, PA2, PA3
							 Timer3: PA6, PA7, PB0, PB1
  ******************************************************************************
*/
#ifndef PWM_H
#define PWM_H
#include "stm32f10x.h"

typedef enum
{
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4
}TIM_CHANNEL;

void TIM_Base_Config(TIM_TypeDef *TIMX, uint16_t Prescaler, uint16_t Period);
void TIM_PWM_Channel_Config(TIM_TypeDef *TIM_X, TIM_CHANNEL channel_x);
void GPIO_Mode_AF_PP_Config(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X);

#endif
