#ifndef __BUTTON_H
#define __BUTTON_H

/* Includes ------------------------------------------------------------------*/

typedef struct
{
    uint32_t holdTime;
    uint64_t changeTime;
    uint8_t flags;
} ButtonManagement;

void Button_Config(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, ButtonManagement *ButtonX);
bool Button_OnPress(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, ButtonManagement *ButtonX);
bool Button_OnHold(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, ButtonManagement *ButtonX);
void Button_SetHoldTime (ButtonManagement *ButtonX, uint32_t timeMs);

#endif
