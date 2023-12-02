#include <stdint.h>
#include "stm32f10x.h"
#include "light.h"

static uint16_t Map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t LIGHT_Read(void)
{
    uint8_t ret;

    uint16_t data;
    data = ADC_GetConversionValue(ADC1);
    ret = Map(data, 0, 4095, 99, 0);

    return ret;
}

void LIGHT_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    /*cap clock cho chan GPIO va bo ADC1*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /*cau hinh chan Input cua bo ADC1 la chan PA0*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*cau hinh ADC1*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* Cau hinh chanel, rank, thoi gian lay mau */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    /* Cho phep bo ADC1 hoa dong */
    ADC_Cmd(ADC1, ENABLE);
    /* cho phep cam bien nhiet hoat dong */
    ADC_TempSensorVrefintCmd(ENABLE);
    /* Reset thanh ghi cablib  */
    ADC_ResetCalibration(ADC1);
    /* Cho thanh ghi cablib reset xong */
    while (ADC_GetResetCalibrationStatus(ADC1));
    /* Khoi dong bo ADC */
    ADC_StartCalibration(ADC1);
    /* Cho trang thai cablib duoc bat */
    while (ADC_GetCalibrationStatus(ADC1));
    /* Bat dau chuyen doi ADC */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
