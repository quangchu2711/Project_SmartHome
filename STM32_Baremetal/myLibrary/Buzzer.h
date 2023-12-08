/**
  ******************************************************************************
  * @file    Buzzer.h
  * @author  Chu Xuan Quang
  * @date    24-1-2022
  * @brief   Buzzer program body
  ******************************************************************************
  * @attention on the stm32f103 kit only the following pins can be used:
	*						 PA0, PA1, PA2, PA3
	*						 PA6, PA7, PB0, PB1
	*						 PB6, PB7, PB8, PB9
  ******************************************************************************
*/
#ifndef BUZZER_H
#define BUZZER_H
#include "stm32f10x.h"
#include "PWM.h"

#define FRE_72MHZ 	72000000
#define PSC 				7199
#define PERIOD 			65535
#define STOP_VALUE	10000

void Buzzer_Config(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X);
void Buzzer_Play(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X, uint16_t fre);
void Buzzer_Stop(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_X);

#endif
