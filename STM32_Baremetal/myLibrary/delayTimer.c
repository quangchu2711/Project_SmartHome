/*******************************************************************************
* Definitions
******************************************************************************/
#include <stdint.h>
#include "stm32f10x.h"
#include "delayTimer.h"
#define APB_CLOCK_TIMER         72
#define PERIOD                  65000
/*******************************************************************************
* Variables
******************************************************************************/
static volatile uint32_t g_tickMs = 0;
static volatile uint32_t g_tickUs = 0;
/*******************************************************************************
* Prototypes
******************************************************************************/
static uint32_t DelayTimer_GetTickMs(void);
static uint32_t DelayTimer_GetTickUs(void);
/*******************************************************************************
* Code
******************************************************************************/
static uint32_t DelayTimer_GetTickMs(void)
{
    return ((g_tickUs + TIM_GetCounter(DELAY_TIMER)) / 1000);
}

static uint32_t DelayTimer_GetTickUs(void)
{
    return (g_tickUs + TIM_GetCounter(DELAY_TIMER));
}

void DelayTimer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_StructInit;
    NVIC_InitTypeDef NVIC_StructInit;

    RCC_APB1PeriphClockCmd(DELAY_CLK_ENABLE, ENABLE);

    TIM_StructInit.TIM_Prescaler = APB_CLOCK_TIMER - 1;
    TIM_StructInit.TIM_Period = PERIOD - 1;
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_StructInit.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(DELAY_TIMER, &TIM_StructInit);

    TIM_ITConfig(DELAY_TIMER, TIM_IT_Update, ENABLE);
    TIM_Cmd(DELAY_TIMER, ENABLE);
    TIM_ClearFlag(DELAY_TIMER, TIM_FLAG_Update);

    NVIC_StructInit.NVIC_IRQChannel = DELAY_IRQn;
    NVIC_StructInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_StructInit.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_StructInit.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_StructInit);
}

void DelayTimer_Ms(uint32_t ms)
{
    uint32_t g_tickMsCurrent = DelayTimer_GetTickMs();
    while (DelayTimer_GetTickMs() - g_tickMsCurrent < ms);
}

void DelayTimer_Us(uint32_t us)
{
    uint32_t g_tickUsCurrent = DelayTimer_GetTickUs();
    while (DelayTimer_GetTickUs() - g_tickUsCurrent < us);
}

uint32_t Timer_GetTickMs(void)
{
	return ((g_tickUs + TIM_GetCounter(DELAY_TIMER)) / 1000);
}

uint32_t Timer_GetTickUs(void)
{
	return (g_tickUs + TIM_GetCounter(DELAY_TIMER));
}

void DELAY_IRQHandler(void)
{
    g_tickMs += 65;
    g_tickUs += 65000;
    TIM_ClearITPendingBit(DELAY_TIMER, TIM_IT_Update);
}
