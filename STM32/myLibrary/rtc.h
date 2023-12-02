#ifndef _RTC_H_
#define _RTC_H_

typedef struct
{
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
} rtc_time_t;
void RTC_Init(void);
void RTC_SetTime(rtc_time_t *pRtcTime);
void RTC_GetTime(rtc_time_t *pRtcTime);

#endif
