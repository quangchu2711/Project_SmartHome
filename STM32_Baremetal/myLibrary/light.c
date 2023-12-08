#include <stdint.h>
#include "stm32f10x.h"
#include "DMA.h"
#include "light.h"

#define LIGHT_ADC_CHANNEL_NUMBER    (3U)

uint16_t g_adcValue[LIGHT_ADC_CHANNEL_NUMBER];

static uint16_t Map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t LIGHT_Read(Light_number_t number)
{
    uint8_t ret;
    uint16_t data;

    if (number == LIGHT_NUMBER_1)
    {
        data = g_adcValue[0];
    }
    else if (number == LIGHT_NUMBER_2)
    {
        data = g_adcValue[1];
    }
    else if (number == LIGHT_NUMBER_3)
    {
        data = g_adcValue[2];
    }

    ret = Map(data, 0, 4095, 99, 1);

    return ret;
}

void LIGHT_Init(void)
{
    ADC_Multi_Channel_Config(3, ADC_SampleTime_239Cycles5);
    DMA_Multi_Channel_Config(g_adcValue, 3);
    DMA_Start();
}
