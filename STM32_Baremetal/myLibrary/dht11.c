#include "dht11.h"

uint32_t g_rccPeriphGPIO[3] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC};
GPIO_TypeDef* gpioBase[3] = {GPIOA, GPIOB, GPIOC};

static void Delay_Init(void);
static void Delay_Ms(uint32_t u32Delay);
static void Delay_Us(uint32_t u32Delay);

static void Delay_Init(void)
{
    /* su dung timer nao do de tao delay */
    TIM_TimeBaseInitTypeDef timInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    timInit.TIM_ClockDivision = TIM_CKD_DIV1;
    timInit.TIM_CounterMode = TIM_CounterMode_Up;
    timInit.TIM_Period = 0xffff;
    timInit.TIM_Prescaler = 72 - 1;
    timInit.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM1, &timInit);
    TIM_Cmd(TIM1, ENABLE);
}

static void Delay_Us(uint32_t u32Delay)
{
    TIM_SetCounter(TIM1, 0);
    while (TIM_GetCounter(TIM1) < u32Delay);
}

static void Delay_Ms(uint32_t u32Delay)
{
    uint32_t i;

    for (i = 0; i < u32Delay; ++i) {
        Delay_Us(1000);
    }
}

// void DHT11_Init(void)
void DHT11_Init(PORT_t port, uint16_t gpioPin)
{
    Delay_Init();
    GPIO_InitTypeDef gpioInit;

    RCC_APB2PeriphClockCmd(g_rccPeriphGPIO[port], ENABLE);
    gpioInit.GPIO_Mode = GPIO_Mode_Out_OD;
    gpioInit.GPIO_Pin = gpioPin;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(gpioBase[port], &gpioInit);

    GPIO_SetBits(gpioBase[port], gpioPin);
}

uint8_t DHT11_Read(PORT_t port, uint16_t gpioPin, uint8_t *pTem, uint8_t *pHum)
{
    uint16_t u16Tim;
    uint8_t u8Buff[5];
    uint8_t u8CheckSum;
    uint8_t i;

    GPIO_ResetBits(gpioBase[port], gpioPin);
    Delay_Ms(20);
    GPIO_SetBits(gpioBase[port], gpioPin);

    /* cho chan PB12 len cao */
    TIM_SetCounter(TIM1, 0);
    while (TIM_GetCounter(TIM1) < 10) {
        if (GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
            break;
        }
    }
    u16Tim = TIM_GetCounter(TIM1);
    if (u16Tim >= 10) {
        return 0;
    }

    /* cho chan PB12 xuong thap */
    TIM_SetCounter(TIM1, 0);
    while (TIM_GetCounter(TIM1) < 45) {
        if (!GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
            break;
        }
    }
    u16Tim = TIM_GetCounter(TIM1);
    if ((u16Tim >= 45) || (u16Tim <= 5)) {
        return 0;
    }

    /* cho chan PB12 len cao */
    TIM_SetCounter(TIM1, 0);
    while (TIM_GetCounter(TIM1) < 90) {
        if (GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
            break;
        }
    }
    u16Tim = TIM_GetCounter(TIM1);
    if ((u16Tim >= 90) || (u16Tim <= 70)) {
        return 0;
    }

    /* cho chan PB12 xuong thap */
    TIM_SetCounter(TIM1, 0);
    while (TIM_GetCounter(TIM1) < 95) {
        if (!GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
            break;
        }
    }
    u16Tim = TIM_GetCounter(TIM1);
    if ((u16Tim >= 95) || (u16Tim <= 75)) {
        return 0;
    }

    /* nhan byte so 1 */
    for (i = 0; i < 8; ++i) {
        /* cho chan PB12 len cao */
        TIM_SetCounter(TIM1, 0);
        while (TIM_GetCounter(TIM1) < 65) {
            if (GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
                break;
            }
        }
        u16Tim = TIM_GetCounter(TIM1);
        if ((u16Tim >= 65) || (u16Tim <= 45)) {
            return 0;
        }

        /* cho chan PB12 xuong thap */
        TIM_SetCounter(TIM1, 0);
        while (TIM_GetCounter(TIM1) < 80) {
            if (!GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
                break;
            }
        }
        u16Tim = TIM_GetCounter(TIM1);
        if ((u16Tim >= 80) || (u16Tim <= 10)) {
            return 0;
        }
        u8Buff[0] <<= 1;
        if (u16Tim > 45) {
            /* nhan duoc bit 1 */
            u8Buff[0] |= 1;
        } else {
            /* nhan duoc bit 0 */
            u8Buff[0] &= ~1;
        }
    }

    /* nhan byte so 2 */
    for (i = 0; i < 8; ++i) {
        /* cho chan PB12 len cao */
        TIM_SetCounter(TIM1, 0);
        while (TIM_GetCounter(TIM1) < 65) {
            if (GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
                break;
            }
        }
        u16Tim = TIM_GetCounter(TIM1);
        if ((u16Tim >= 65) || (u16Tim <= 45)) {
            return 0;
        }

        /* cho chan PB12 xuong thap */
        TIM_SetCounter(TIM1, 0);
        while (TIM_GetCounter(TIM1) < 80) {
            if (!GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
                break;
            }
        }
        u16Tim = TIM_GetCounter(TIM1);
        if ((u16Tim >= 80) || (u16Tim <= 10)) {
            return 0;
        }
        u8Buff[1] <<= 1;
        if (u16Tim > 45) {
            /* nhan duoc bit 1 */
            u8Buff[1] |= 1;
        } else {
            /* nhan duoc bit 0 */
            u8Buff[1] &= ~1;
        }
    }

    /* nhan byte so 3 */
    for (i = 0; i < 8; ++i) {
        /* cho chan PB12 len cao */
        TIM_SetCounter(TIM1, 0);
        while (TIM_GetCounter(TIM1) < 65) {
            if (GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
                break;
            }
        }
        u16Tim = TIM_GetCounter(TIM1);
        if ((u16Tim >= 65) || (u16Tim <= 45)) {
            return 0;
        }

        /* cho chan PB12 xuong thap */
        TIM_SetCounter(TIM1, 0);
        while (TIM_GetCounter(TIM1) < 80) {
            if (!GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
                break;
            }
        }
        u16Tim = TIM_GetCounter(TIM1);
        if ((u16Tim >= 80) || (u16Tim <= 10)) {
            return 0;
        }
        u8Buff[2] <<= 1;
        if (u16Tim > 45) {
            /* nhan duoc bit 1 */
            u8Buff[2] |= 1;
        } else {
            /* nhan duoc bit 0 */
            u8Buff[2] &= ~1;
        }
    }

    /* nhan byte so 4 */
    for (i = 0; i < 8; ++i) {
        /* cho chan PB12 len cao */
        TIM_SetCounter(TIM1, 0);
        while (TIM_GetCounter(TIM1) < 65) {
            if (GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
                break;
            }
        }
        u16Tim = TIM_GetCounter(TIM1);
        if ((u16Tim >= 65) || (u16Tim <= 45)) {
            return 0;
        }

        /* cho chan PB12 xuong thap */
        TIM_SetCounter(TIM1, 0);
        while (TIM_GetCounter(TIM1) < 80) {
            if (!GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
                break;
            }
        }
        u16Tim = TIM_GetCounter(TIM1);
        if ((u16Tim >= 80) || (u16Tim <= 10)) {
            return 0;
        }
        u8Buff[3] <<= 1;
        if (u16Tim > 45) {
            /* nhan duoc bit 1 */
            u8Buff[3] |= 1;
        } else {
            /* nhan duoc bit 0 */
            u8Buff[3] &= ~1;
        }
    }

    /* nhan byte so 5 */
    for (i = 0; i < 8; ++i) {
        /* cho chan PB12 len cao */
        TIM_SetCounter(TIM1, 0);
        while (TIM_GetCounter(TIM1) < 65) {
            if (GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
                break;
            }
        }
        u16Tim = TIM_GetCounter(TIM1);
        if ((u16Tim >= 65) || (u16Tim <= 45)) {
            return 0;
        }

        /* cho chan PB12 xuong thap */
        TIM_SetCounter(TIM1, 0);
        while (TIM_GetCounter(TIM1) < 80) {
            if (!GPIO_ReadInputDataBit(gpioBase[port], gpioPin)) {
                break;
            }
        }
        u16Tim = TIM_GetCounter(TIM1);
        if ((u16Tim >= 80) || (u16Tim <= 10)) {
            return 0;
        }
        u8Buff[4] <<= 1;
        if (u16Tim > 45) {
            /* nhan duoc bit 1 */
            u8Buff[4] |= 1;
        } else {
            /* nhan duoc bit 0 */
            u8Buff[4] &= ~1;
        }
    }

    u8CheckSum = u8Buff[0] + u8Buff[1] + u8Buff[2] + u8Buff[3];
    if (u8CheckSum != u8Buff[4]) {
        return 0;
    }

    *pTem = u8Buff[2];
    *pHum = u8Buff[0];

    return 1;
}
