#include "stm32f10x.h"
#include "DelayTimer.h"

#define DELAY_TIMER         TIM3
#define DELAY_CLK_ENABLE    RCC_APB1Periph_TIM3
#define DELAY_IRQn          TIM3_IRQn
#define DELAY_IRQHandler    TIM3_IRQHandler
#define APB_CLOCK_TIMER     72
#define PERIOD              65000


static volatile uint32_t tickMs = 0;
static volatile uint32_t tickUs = 0;

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
    uint32_t tickMsCurrent = Timer_GetTickMs();
    while (Timer_GetTickMs() - tickMsCurrent < ms);
}

void DelayTimer_Us(uint32_t us)
{
    uint32_t tickUsCurrent = Timer_GetTickUs();
    while (Timer_GetTickUs() - tickUsCurrent < us);
}

uint32_t Timer_GetTickMs(void)
{
    return ((tickUs + TIM_GetCounter(DELAY_TIMER)) / 1000);
}

uint32_t Timer_GetTickUs(void)
{
    return (tickUs + TIM_GetCounter(DELAY_TIMER));
}

void DELAY_IRQHandler(void)
{
    tickMs += 65;
    tickUs += 65000;
    TIM_ClearITPendingBit(DELAY_TIMER, TIM_IT_Update);
}
