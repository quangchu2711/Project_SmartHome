#include "Buzzer.h"

void Buzzer_Config(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X)
{
	switch (GPIO_Pin_X)
  {
  	case GPIO_Pin_0:
			if (GPIOx == GPIOA)
			{
				TIM_Base_Config(TIM2, PSC, PERIOD);
				TIM_PWM_Channel_Config(TIM2, CHANNEL_1);
			}
			else if (GPIOx == GPIOB)
			{
				TIM_Base_Config(TIM3, PSC, PERIOD);
				TIM_PWM_Channel_Config(TIM3, CHANNEL_3);				
			}
  		break;
  	case GPIO_Pin_1:
			if (GPIOx == GPIOA)
			{
				TIM_Base_Config(TIM2, PSC, PERIOD);
				TIM_PWM_Channel_Config(TIM2, CHANNEL_2);
			}
			else if (GPIOx == GPIOB)
			{
				TIM_Base_Config(TIM3, PSC, PERIOD);
				TIM_PWM_Channel_Config(TIM3, CHANNEL_4);				
			}  		
			break;
  	case GPIO_Pin_2:
			TIM_Base_Config(TIM2, PSC, PERIOD);
			TIM_PWM_Channel_Config(TIM2, CHANNEL_3);
  		break;
  	case GPIO_Pin_3:
			TIM_Base_Config(TIM2, PSC, PERIOD);
			TIM_PWM_Channel_Config(TIM2, CHANNEL_4);
  		break;
  	case GPIO_Pin_6:
			if (GPIOx == GPIOA)
			{
				TIM_Base_Config(TIM3, PSC, PERIOD);
				TIM_PWM_Channel_Config(TIM3, CHANNEL_1);
			}
			else if (GPIOx == GPIOB)
			{
				TIM_Base_Config(TIM4, PSC, PERIOD);
				TIM_PWM_Channel_Config(TIM4, CHANNEL_1);				
			}  		
			break;
  	case GPIO_Pin_7:
			if (GPIOx == GPIOA)
			{
				TIM_Base_Config(TIM3, PSC, PERIOD);
				TIM_PWM_Channel_Config(TIM3, CHANNEL_2);
			}
			else if (GPIOx == GPIOB)
			{
				TIM_Base_Config(TIM4, PSC, PERIOD);
				TIM_PWM_Channel_Config(TIM4, CHANNEL_2);				
			}  		
			break;
  	case GPIO_Pin_8:
			TIM_Base_Config(TIM4, PSC, PERIOD);
			TIM_PWM_Channel_Config(TIM4, CHANNEL_3);
  		break;
  	case GPIO_Pin_9:
			TIM_Base_Config(TIM4, PSC, PERIOD);
			TIM_PWM_Channel_Config(TIM4, CHANNEL_4);
  		break;
  }
}

void Buzzer_Play(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X, uint16_t fre)
{
	Buzzer_Config(GPIOx, GPIO_Pin_X);
	uint16_t arr_value = FRE_72MHZ / (PSC * fre);
	uint16_t compare_value = arr_value / 2;
	
	switch (GPIO_Pin_X)
  {
  	case GPIO_Pin_0:
			if (GPIOx == GPIOA)
			{
				TIM2->ARR = arr_value;
				TIM2->CCR1 = compare_value;
			}
			else if (GPIOx == GPIOB)
			{
				TIM3->ARR = arr_value;
				TIM3->CCR3 = compare_value;
			}
  		break;
  	case GPIO_Pin_1:
			if (GPIOx == GPIOA)
			{
				TIM2->ARR = arr_value;
				TIM2->CCR2 = compare_value;				
			}
			else if (GPIOx == GPIOB)
			{
				TIM3->ARR = arr_value;
				TIM3->CCR4 = compare_value;
			}  		
			break;
  	case GPIO_Pin_2:
				TIM2->ARR = arr_value;
				TIM2->CCR3 = compare_value;
  		break;
  	case GPIO_Pin_3:
				TIM2->ARR = arr_value;
				TIM2->CCR4 = compare_value;
  		break;
  	case GPIO_Pin_6:
			if (GPIOx == GPIOA)
			{
				TIM3->ARR = arr_value;
				TIM3->CCR1 = compare_value;
			}
			else if (GPIOx == GPIOB)
			{
				TIM4->ARR = arr_value;
				TIM4->CCR1 = compare_value;
			}  		
			break;
  	case GPIO_Pin_7:
			if (GPIOx == GPIOA)
			{
				TIM3->ARR = arr_value;
				TIM3->CCR2 = compare_value;
			}
			else if (GPIOx == GPIOB)
			{
				TIM4->ARR = arr_value;
				TIM4->CCR2 = compare_value;
			}  		
			break;
  	case GPIO_Pin_8:
				TIM4->ARR = arr_value;
				TIM4->CCR3 = compare_value;
  		break;
  	case GPIO_Pin_9:
				TIM4->ARR = arr_value;
				TIM4->CCR4 = compare_value;
  		break;
  }
}

void Buzzer_Stop(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X)
{
	Buzzer_Play(GPIOx, GPIO_Pin_X, STOP_VALUE);
}
