#include <stdint.h>
#include "stm32f10x.h"
#include "rtc.h"

static rtc_time_t rtcTime;

static void RTC_Configuration(void);
static void NVIC_Configuration(void);

static void RTC_Configuration(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();

    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void RTC_Init(void)
{
    NVIC_Configuration();
    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        /* Backup data register value is not correct or not yet programmed (when
           the first time the program is executed) */

        /* RTC Configuration */
        RTC_Configuration();

        /* Adjust time by values entered by the user on the hyperterminal */
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
        /* Change the current time */
        RTC_SetCounter(0);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
    else
    {
        /* Check if the Power On Reset flag is set */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            /* Power On Reset occurred */
        }
        /* Check if the Pin Reset flag is set */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            /* External Reset occurred */
        }

        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();

        /* Enable the RTC Second */
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }

    /* Clear reset flags */
    RCC_ClearFlag();

    /*Reset time*/
    rtcTime.Hour = 0;
    rtcTime.Minute = 0;
    rtcTime.Second = 0;
}

void RTC_SetTime(rtc_time_t *pRtcTime)
{
    rtcTime.Hour = pRtcTime->Hour;
    rtcTime.Minute = pRtcTime->Minute;
    rtcTime.Second = pRtcTime->Second;
}

void RTC_GetTime(rtc_time_t *pRtcTime)
{
    pRtcTime->Hour = rtcTime.Hour;
    pRtcTime->Minute = rtcTime.Minute;
    pRtcTime->Second = rtcTime.Second;
}

void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_SEC);

        /* Enable time update */
        rtcTime.Second += 1;
        if (rtcTime.Second > 59)
        {
            rtcTime.Second = 0;
            rtcTime.Minute += 1;
            if (rtcTime.Minute > 59)
            {
                rtcTime.Minute = 0;
                rtcTime.Hour += 1;
                if (rtcTime.Hour > 23)
                {
                    rtcTime.Hour = 0;
                }
            }
        }

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

    }
}

