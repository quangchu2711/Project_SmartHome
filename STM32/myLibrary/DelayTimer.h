#ifndef DELAY_TIMER_H
#define DELAY_TIMER_H

void DelayTimer_Init(void);
void DelayTimer_Ms(uint32_t ms);
void DelayTimer_Us(uint32_t us);
uint32_t Timer_GetTickMs(void);
uint32_t Timer_GetTickUs(void);

#endif

