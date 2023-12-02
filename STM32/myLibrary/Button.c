#include<stdio.h>
#include <stdbool.h>
#include "stm32f10x.h"
#include "Button.h"

#define DEBOUNCE_TIME   50

#define  IS_PRESS    0x01
#define  WAS_PRESS   0x02
#define  ON_PRESS    0x04
#define  ON_RELEASE  0x08
#define  IS_HOLD     0x10
#define  ON_HOLD     0x20

static volatile uint32_t tickMs = 0;
static volatile uint32_t tickUs = 0;

static void Button_Update(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, ButtonManagement *ButtonX);
static uint32_t Button_TimerGetTickMs(void);
static void Button_TimerInit(void);

static void Button_TimerInit(void)
{
    TIM_TimeBaseInitTypeDef TIM_StructInit;
    NVIC_InitTypeDef NVIC_StructInit;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_StructInit.TIM_Prescaler = 72 - 1;
    TIM_StructInit.TIM_Period = 65000 - 1;
    TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_StructInit.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_StructInit.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_StructInit);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    NVIC_StructInit.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_StructInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_StructInit.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_StructInit.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_StructInit);
}

static uint32_t Button_TimerGetTickMs(void)
{
    return ((tickUs + TIM_GetCounter(TIM2)) / 1000);
}

void TIM2_IRQHandler(void)
{
    tickMs += 65;
    tickUs += 65000;
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}


void Button_Config(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, ButtonManagement *ButtonX)
{
    Button_TimerInit();
    //Set up pin button
    GPIO_InitTypeDef GPIO_Struct;
    if (GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_Struct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Struct.GPIO_Pin =  GPIO_Pin;
    GPIO_Struct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOx, &GPIO_Struct);

    //Set up ButtonManagement
    ButtonX->changeTime = 0;
    ButtonX->flags = 0x00;
    ButtonX->holdTime = 1000;
}

static void Button_Update(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, ButtonManagement *ButtonX)
{
    //Read pin state to IS_PRESS
    uint64_t interval = Button_TimerGetTickMs() - ButtonX->changeTime;
    if (interval > DEBOUNCE_TIME)
    {
        if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0)
        {
            ButtonX->flags |= IS_PRESS;
        }
        else
        {
            ButtonX->flags &= ~IS_PRESS;
        }
    }

    //Detect changes
    switch (ButtonX->flags & (WAS_PRESS | IS_PRESS))
    {
    case WAS_PRESS:
        ButtonX->flags = ON_RELEASE;
        ButtonX->changeTime = Button_TimerGetTickMs();
        break;

    case IS_PRESS:
        ButtonX->flags |= IS_PRESS | ON_PRESS | WAS_PRESS;
        ButtonX->changeTime = Button_TimerGetTickMs();
        interval = 0;
        break;
    }

    //Checking Hold state
    if ((ButtonX->flags & IS_PRESS) &&       //is press state
            (!(ButtonX->flags & IS_HOLD)) &&       //is hod not set
            (interval > ButtonX->holdTime))        //long hold flags
    {
        ButtonX->flags |= IS_HOLD | ON_HOLD;
    }
}

bool Button_OnPress(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, ButtonManagement *ButtonX)
{
    Button_Update(GPIOx, GPIO_Pin, ButtonX);
    bool result = ButtonX->flags & ON_PRESS;
    ButtonX->flags &= ~ON_PRESS;                    // Reset event flag after checking
    return result;
}

bool Button_OnHold(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, ButtonManagement *ButtonX)
{
    Button_Update(GPIOx, GPIO_Pin, ButtonX);
    bool result = ButtonX->flags & ON_HOLD;
    ButtonX->flags &= ~ON_HOLD;                    // Reset event flag after checking
    return result;
}

void Button_SetHoldTime (ButtonManagement *ButtonX, uint32_t timeMs)
{
    ButtonX->holdTime = timeMs;
}
