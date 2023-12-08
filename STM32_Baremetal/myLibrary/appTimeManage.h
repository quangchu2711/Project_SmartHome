#ifndef _APP_MANAGE__
#define _APP_MANAGE__

typedef enum
{
    RQ_ChangeTime = 0x00U,
    RQ_ControlLed = 0x01U,
    RQ_AutoFeed = 0x02U,
    RQ_AutoOpenDoor = 0x03U
}AppTimeManage_RequestTime_t;

typedef struct timeValue
{
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
}AppTimeManage_TimeValue_t;

void AppTimeManage_Init(void);
void AppTimeManage_SetRequestTime(AppTimeManage_RequestTime_t rqTime, AppTimeManage_TimeValue_t *pTime);
void AppTimeManage_GetCurrentTime(AppTimeManage_TimeValue_t *pTime);
bool AppTimeManage_CheckTimeStatus(AppTimeManage_RequestTime_t rqTime, AppTimeManage_TimeValue_t *pTime);

#endif

