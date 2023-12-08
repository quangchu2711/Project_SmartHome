#ifndef DELAY_TIMER_H
#define DELAY_TIMER_H
/*******************************************************************************
* Definitions
******************************************************************************/
#define DELAY_TIMER        	TIM4
#define DELAY_CLK_ENABLE    RCC_APB1Periph_TIM4
#define DELAY_IRQn         	TIM4_IRQn
#define DELAY_IRQHandler    TIM4_IRQHandler
/*******************************************************************************
* API
******************************************************************************/
void DelayTimer_Init(void);
void DelayTimer_Ms(uint32_t ms);
void DelayTimer_Us(uint32_t us);
uint32_t Timer_GetTickMs(void);
uint32_t Timer_GetTickUs(void);
#endif

