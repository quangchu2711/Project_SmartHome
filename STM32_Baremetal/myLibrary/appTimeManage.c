#include <stdbool.h>
#include "stm32f10x.h"
#include <stdint.h>
#include "rtc.h"
#include "appTimeManage.h"

#define NUMBER_REQUEST_TIME 3

AppTimeManage_TimeValue_t g_saveSetTime[NUMBER_REQUEST_TIME];
bool g_CheckTimeStatus[3] = {false, false, false};

bool AppTimeManage_CheckTimeStatus(AppTimeManage_RequestTime_t rqTime, AppTimeManage_TimeValue_t *pTime)
{
    bool check = false;
    if (g_CheckTimeStatus[rqTime] == true)
    {
        if (g_saveSetTime[rqTime].Hour == pTime->Hour)
        {
            if (g_saveSetTime[rqTime].Minute == pTime->Minute)
            {
                if (g_saveSetTime[rqTime].Second == pTime->Second)
                {
                    check = true;
                    g_CheckTimeStatus[rqTime] = false;
                }
            }
        }
    }

    return check;
}

void AppTimeManage_Init(void)
{
    RTC_Init();
}

void AppTimeManage_SetRequestTime(AppTimeManage_RequestTime_t rqTime, AppTimeManage_TimeValue_t *pTime)
{
    rtc_time_t rtcSetTime;

    g_saveSetTime[rqTime].Hour = pTime->Hour;
    g_saveSetTime[rqTime].Minute = pTime->Minute;
    g_saveSetTime[rqTime].Second = pTime->Second;
    if (rqTime == RQ_ChangeTime)
    {
        rtcSetTime.Hour = g_saveSetTime[RQ_ChangeTime].Hour;
        rtcSetTime.Minute = g_saveSetTime[RQ_ChangeTime].Minute;
        rtcSetTime.Second = g_saveSetTime[RQ_ChangeTime].Second;
        RTC_SetTime(&rtcSetTime);
    }
    else
    {
        g_CheckTimeStatus[rqTime] = true;
    }
}

void AppTimeManage_GetCurrentTime(AppTimeManage_TimeValue_t *pTime)
{
    rtc_time_t rtcGetTime;
    RTC_GetTime(&rtcGetTime);
    pTime->Hour = rtcGetTime.Hour;
    pTime->Minute = rtcGetTime.Minute;
    pTime->Second = rtcGetTime.Second;
}

