#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "rtc.h"
#include "DHT11.h"
#include "LCD_I2C.h"
#include "Button.h"
#include "light.h"
#include "DelayTimer.h"
#include "appTimeManage.h"
#include "appServo.h"
#include "Serial.h"

#define BUTTON_PORT         (GPIOA)
#define MIDDLE_BUTTON_PIN   (GPIO_Pin_6)
#define RIGHT_BUTTON_PIN    (GPIO_Pin_5)
#define LEFT_BUTTON_PIN     (GPIO_Pin_7)

#define LED_ON_CODE         (0x01U)
#define LED_OFF_CODE        (0x02U)
#define OPEN_DOOR_CODE      (0x04U)
#define AUTO_FEED_CODE      (0x08U)
#define SIREN_ON_CODE        (0x10U)
#define SIREN_OFF_CODE       (0x20U)

#define AUTO_MODE            'A'
#define MANUAL_MODE          'M'

#define LIGHT_THRESHOLD     30

typedef enum
{
    MODE_ChangeTime = 0x00U,
    MODE_SeTime_ControlLed = 0x01U,
    MODE_SeTime_AutoFeed = 0x02U,
    MODE_SetTime_ControlDoor = 0x03U,
    MODE_Display = 0x04U,
    MODE_SelectOption = 0x05U
} Mode_SystemTime_t;

typedef enum
{
    TIME_Hour = 0x00U,
    TIME_Minute = 0x01U,
    TIME_Second = 0x02U
} Time_Handle_t;

typedef struct sensor_data
{
    uint8_t tem1;
    uint8_t tem2;
    uint8_t hum1;
    uint8_t hum2;
    uint8_t rain;
    uint8_t light;
} Sensor_valueStruct_t;

uint8_t g_Tem1, g_Tem2, g_Hum1,g_Hum2;

TaskHandle_t taskTimeManage;
TaskHandle_t taskControlDevice;
TaskHandle_t taskuart;
xQueueHandle queueBuffer;
uint8_t g_flagDisplay;
uint8_t g_flagCheckPressRL_Button = 0;
uint32_t g_TimeRun;
ButtonManagement g_mButton;
ButtonManagement g_rButton;
ButtonManagement g_lButton;
Mode_SystemTime_t g_SysTimeMode;
Time_Handle_t g_currentTimeEdit;
Mode_SystemTime_t g_timeChangeOption;
AppTimeManage_TimeValue_t g_TimeVal[4];
char g_optionInfor[5][16] = {"   [Edit Time] ", "   [Led Time]  ", "   [Feed Time] ", "  [Door Time]  ", "     [Exit] "};
uint8_t g_sensorBuffer[9] = {'(', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, ')'};
uint8_t g_frameBuffer[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t g_buffIdx = 0;
uint8_t g_mode = AUTO_MODE;
Sensor_valueStruct_t g_senSorStruct;

static void Led_Init(void);
static void Button_Init(void);
static void Button_CheckMode(void);
static void Button_EditTime( AppTimeManage_TimeValue_t *pTime);
static void Button_HandleData(Time_Handle_t timeHandle, AppTimeManage_TimeValue_t *pHandleData);

static void Mode_DisplayAndHandleData(void);
static void Mode_SelectOption(void);

static void Task_TimeManage( void *pvParameters );
static void Task_ControlDevice( void *pvParameters );
static void Task_Uart( void *pvParameters );
static void Handle_DataFromToEsp(uint8_t readData);

int main(void)
{
    // SystemInit();
    xTaskCreate(Task_TimeManage, "TimeManage", 128, NULL, 0, &taskTimeManage);
    //xTaskCreate(Task_Uart, "Uart", 128, NULL, 2, &taskuart);
    xTaskCreate(Task_ControlDevice, "ControlDevice", 128, NULL, 1, &taskControlDevice);
    queueBuffer = xQueueCreate (9, sizeof(uint8_t));

    vTaskStartScheduler();
    while (1)
    {

    }
}

static void Handle_DataFromToEsp(uint8_t readData)
{
    uint8_t ledCode;
    if (readData == '\n' || g_buffIdx == 4)
    {
//        g_frameBuffer[g_buffIdx] = 0x00;
//        g_resData = atoi(&g_frameBuffer[1]);
//        if (g_frameBuffer[0] == 'L')
//        {
//            g_resLightData = g_resData;
//        }
//        else if (g_frameBuffer[0] == 'R')
//        {
//            g_resRainData = g_resData;
//        }
//        else if (g_frameBuffer[0] == 'S')
//        {
//            g_resSoilData = g_resData;
//        }
//        else if (g_frameBuffer[0] == 'D')
//        {
//            if (g_frameBuffer[1] == '1')
//            {
//                ledCode = ON_LED1_CODE;
//                // xQueueSend(queue3Buffer, &ledCode, 20);
//            }
//            else if (g_frameBuffer[1] == '0')
//            {
//                ledCode = OFF_LED1_CODE;
//                // xQueueSend(queue3Buffer, &ledCode, 20);
//            }
//        }
//        else if (g_frameBuffer[0] == 'M')
//        {
//            if (g_frameBuffer[1] == '1')
//            {
//                g_mode = MANUAL_MODE;
//            }
//            else if (g_frameBuffer[1] == '0')
//            {
//                g_mode = AUTO_MODE;
//            }
//        }
//        else if (g_frameBuffer[0] == 'E')
//        {
//            if (g_frameBuffer[1] == '1')
//            {
//                ledCode = ON_RELAY_CODE;
//                // xQueueSend(queue2Buffer, &ledCode, 20);
//            }
//            else if (g_frameBuffer[1] == '0')
//            {
//                ledCode = OFF_RELAY_CODE;
//                // xQueueSend(queue2Buffer, &ledCode, 20);
//            }
//        }
//        /* Reset array */
//        g_buffIdx = 0;
//        g_frameBuffer[0] = 0x00;
//        g_frameBuffer[1] = 0x00;
//        g_frameBuffer[2] = 0x00;
//        g_frameBuffer[3] = 0x00;
//        g_frameBuffer[4] = 0x00;

    }
//    else
//    {
//        g_frameBuffer[g_buffIdx] = readData;
//        g_buffIdx += 1;
//    }
}

static void Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void Button_Init(void)
{
    Button_Config(BUTTON_PORT, MIDDLE_BUTTON_PIN, &g_mButton);
    Button_Config(BUTTON_PORT, RIGHT_BUTTON_PIN, &g_rButton);
    Button_Config(BUTTON_PORT, LEFT_BUTTON_PIN, &g_lButton);
    Button_SetHoldTime (&g_mButton, 2000);
    Button_SetHoldTime (&g_rButton, 2000);
    Button_SetHoldTime (&g_lButton, 2000);
}

static void Button_HandleData(Time_Handle_t timeHandle, AppTimeManage_TimeValue_t *pHandleData)
{
    if (Button_OnPress(BUTTON_PORT, RIGHT_BUTTON_PIN, &g_rButton))
    {
        if (timeHandle == TIME_Hour)
        {
            pHandleData->Hour++;
            if (pHandleData->Hour > 23)
            {
                pHandleData->Hour = 0;
            }
        }
        else if (timeHandle == TIME_Minute)
        {
            pHandleData->Minute++;
            if (pHandleData->Minute > 59)
            {
                pHandleData->Minute = 0;
            }
        }
        else if (timeHandle == TIME_Second)
        {
            pHandleData->Second++;
            if (pHandleData->Second > 59)
            {
                pHandleData->Second = 0;
            }
        }
    }
    else if (Button_OnPress(BUTTON_PORT, LEFT_BUTTON_PIN, &g_rButton))
    {
        if (timeHandle == TIME_Hour)
        {
            if (pHandleData->Hour != 0) pHandleData->Hour--;
            else pHandleData->Hour = 23;
        }
        else if (timeHandle == TIME_Minute)
        {
            if (pHandleData->Minute != 0) pHandleData->Minute--;
            else pHandleData->Minute = 59;
        }
        else if (timeHandle == TIME_Second)
        {
            if (pHandleData->Second != 0) pHandleData->Second--;
            else pHandleData->Second = 59;
        }
    }
}

static void Button_CheckMode(void)
{
    if (g_SysTimeMode == MODE_Display)
    {
        if (Button_OnHold(BUTTON_PORT, MIDDLE_BUTTON_PIN, &g_mButton))
        {
            g_SysTimeMode = MODE_SelectOption;
            LCD_Clear();
        }
    }
    else if (g_SysTimeMode >= MODE_ChangeTime && g_SysTimeMode <=  MODE_SetTime_ControlDoor)
    {
        if (Button_OnHold(BUTTON_PORT, MIDDLE_BUTTON_PIN, &g_mButton))
        {
            g_currentTimeEdit = TIME_Hour;
            AppTimeManage_SetRequestTime((AppTimeManage_RequestTime_t)g_SysTimeMode, &g_TimeVal[g_SysTimeMode]);
            g_SysTimeMode = MODE_Display;
            LCD_Clear();
        }
        if (Button_OnPress(BUTTON_PORT, MIDDLE_BUTTON_PIN, &g_mButton))
        {
            g_currentTimeEdit += 1;
            if (g_currentTimeEdit > TIME_Second)
            {
                g_currentTimeEdit = TIME_Hour;
            }
        }
        Button_HandleData(g_currentTimeEdit, &g_TimeVal[g_SysTimeMode]);
    }
    else if (g_SysTimeMode == MODE_SelectOption)
    {
        if (GPIO_ReadInputDataBit(BUTTON_PORT, MIDDLE_BUTTON_PIN) == 1)
        {
            g_flagCheckPressRL_Button = 1;
        }
        if (g_flagCheckPressRL_Button == 1 && Button_OnPress(BUTTON_PORT, MIDDLE_BUTTON_PIN, &g_mButton))
        {
            g_SysTimeMode = g_timeChangeOption;
            g_flagCheckPressRL_Button = 0;
            LCD_Clear();
        }
        if (Button_OnPress(BUTTON_PORT, RIGHT_BUTTON_PIN, &g_rButton))
        {
            g_timeChangeOption++;
            if (g_timeChangeOption > MODE_Display) g_timeChangeOption = MODE_ChangeTime;
        }
        else if (Button_OnPress(BUTTON_PORT, LEFT_BUTTON_PIN, &g_lButton))
        {
            if (g_timeChangeOption != MODE_ChangeTime) g_timeChangeOption--;
            else g_timeChangeOption = MODE_Display;
        }
    }
}

static void Button_EditTime(AppTimeManage_TimeValue_t *pTime)
{
    if (Timer_GetTickMs() - g_TimeRun > 300)
    {
        g_flagDisplay = !g_flagDisplay;
        g_TimeRun = Timer_GetTickMs();
    }
    if (g_flagDisplay)
    {
        LCD_Gotoxy(0, 1);
        LCD_Printf("    %02d:%02d:%02d", pTime->Hour, pTime->Minute, pTime->Second);
    }
    else
    {
        if (g_currentTimeEdit == TIME_Hour)
        {
            LCD_Gotoxy(0, 1);
            LCD_Printf("    %c%c:%02d:%02d", ' ', ' ', pTime->Minute, pTime->Second);
        }
        else if (g_currentTimeEdit == TIME_Minute)
        {
            LCD_Gotoxy(0, 1);
            LCD_Printf("    %02d:%c%c:%02d", pTime->Hour, ' ', ' ', pTime->Second);
        }
        else if (g_currentTimeEdit == TIME_Second)
        {
            LCD_Gotoxy(0, 1);
            LCD_Printf("    %02d:%02d:%c%c", pTime->Hour, pTime->Minute, ' ', ' ');
        }
    }
}

static void Mode_DisplayAndHandleData(void)
{
    uint8_t controlCode = 0x00U;
    uint8_t lightValue;
    bool checkSendQueue = false;

    /* Hien thi thong tin len man hinh LCD: time va sensor */
    AppTimeManage_GetCurrentTime(&g_TimeVal[MODE_ChangeTime]);
    lightValue = LIGHT_Read();
    LCD_Gotoxy(0, 0);
    LCD_Printf("%02d:%02d:%02d|%02d%c", g_TimeVal[MODE_ChangeTime].Hour, g_TimeVal[MODE_ChangeTime].Minute, g_TimeVal[MODE_ChangeTime].Second, lightValue, '%');
    LCD_Gotoxy(0, 1);

    LCD_Printf("%02d%cC%02d%c %02d%cC%02d%c", g_Tem1, 223, g_Hum1, '%', g_Tem2, 223, g_Hum2, '%');
    /*Sau 2s se doc gia tri cam bien DHT11*/
    if (Timer_GetTickMs() - g_TimeRun > 2000)
    {
//        DHT11_Read(PORTB, GPIO_Pin_8, &g_Tem1, &g_Hum1);
//        DHT11_Read(PORTB, GPIO_Pin_5, &g_Tem2, &g_Hum2);
        g_TimeRun = Timer_GetTickMs();
    }
    if (lightValue > LIGHT_THRESHOLD)
    {
        checkSendQueue = true;
        controlCode |= LED_ON_CODE;
    }
    else
    {
        checkSendQueue = true;
        controlCode |= LED_OFF_CODE;
    }

    /*Check thoi gian bao thuc cua tung che do*/
    if (AppTimeManage_CheckTimeStatus(RQ_ControlLed, &g_TimeVal[MODE_ChangeTime]) == true)
    {
        checkSendQueue = true;
        controlCode |= SIREN_ON_CODE;
    }
    if (AppTimeManage_CheckTimeStatus(RQ_AutoFeed, &g_TimeVal[MODE_ChangeTime]) == true)
    {
        checkSendQueue = true;
        controlCode |= AUTO_FEED_CODE;
    }
    if (AppTimeManage_CheckTimeStatus(RQ_AutoOpenDoor, &g_TimeVal[MODE_ChangeTime]) == true)
    {
        checkSendQueue = true;
        controlCode |= OPEN_DOOR_CODE;
    }
    if (checkSendQueue == true)
    {
        xQueueSend(queueBuffer, &controlCode, 20);
    }
}

static void Mode_SelectOption(void)
{
    LCD_Gotoxy(0, 0);
    LCD_Printf("=> Select:   ");
    if (Timer_GetTickMs() - g_TimeRun > 300)
    {
        g_flagDisplay = !g_flagDisplay;
        g_TimeRun = Timer_GetTickMs();
    }
    if (g_flagDisplay)
    {
        LCD_Gotoxy(0, 1);
        LCD_Printf("%s", g_optionInfor[g_timeChangeOption]);
    }
    else
    {
        LCD_Gotoxy(0, 1);
        LCD_Printf("%s", "               ");
    }
}

static void Task_TimeManage(void *pvParameters)
{
    DelayTimer_Init();
    LCD_Init();
    AppTimeManage_Init();
    DHT11_Init(PORTB, GPIO_Pin_8);
    DHT11_Init(PORTB, GPIO_Pin_5);
    uint8_t g_Tem1,g_Hum1;
    uint8_t g_Tem2, g_Hum2;
    DHT11_Read(PORTB, GPIO_Pin_8, &g_Tem1, &g_Hum1);
    DHT11_Read(PORTB, GPIO_Pin_5, &g_Tem2, &g_Hum2);
    Button_Init();
    LIGHT_Init();

    g_TimeRun = 0;
    g_SysTimeMode = MODE_Display;

    g_currentTimeEdit = TIME_Hour;
    g_flagDisplay = 0;
    g_timeChangeOption = MODE_Display;


    while (1)
    {
        Button_CheckMode();
        switch (g_SysTimeMode)
        {
        case MODE_Display:
            Mode_DisplayAndHandleData();
            break;
        case MODE_ChangeTime:
        case MODE_SeTime_ControlLed:
        case MODE_SeTime_AutoFeed:
        case MODE_SetTime_ControlDoor:
            LCD_Gotoxy(0, 0);
            LCD_Printf("%s", g_optionInfor[g_SysTimeMode]);
            Button_EditTime(&g_TimeVal[g_SysTimeMode]);
            break;
        case MODE_SelectOption:
            Mode_SelectOption();
            break;
        }
        vTaskDelay(10);
    }

}

static void Task_ControlDevice(void *pvParameters)
{
    Led_Init();
    AppServo_Init();
    AppServo_Control(SERVO_ControlDoor, 0);
    AppServo_Control(SERVO_Autofeed, 0);

    GPIO_WriteBit(GPIOA, GPIO_Pin_9, (BitAction)(1));
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)(1));

    uint8_t receiveData;

    AppServo_Control(SERVO_Autofeed, 60);
    AppServo_Control(SERVO_ControlDoor, 70);

    bool flag1 = false;
    bool flag2 = false;
    bool flag3 = false;
    uint8_t ledSta = 0;
    uint32_t timeCnt1 = 0;
    uint32_t timeCnt2 = 0;
    uint32_t timeCnt3 = 0;
    while (1)
    {
        if (xQueueReceive(queueBuffer, &receiveData, 20))
        {
            if (receiveData & LED_ON_CODE)
            {
                GPIO_WriteBit(GPIOA, GPIO_Pin_9, (BitAction)(1));
            }
            if (receiveData & LED_OFF_CODE)
            {
                GPIO_WriteBit(GPIOA, GPIO_Pin_9, (BitAction)(0));
            }
            if (receiveData & AUTO_FEED_CODE)
            {
                AppServo_Control(SERVO_Autofeed, 180);
                flag1 = true;
            }
            if (receiveData & OPEN_DOOR_CODE)
            {
                AppServo_Control(SERVO_ControlDoor, 180);
                flag2 = true;
            }
            if (receiveData & SIREN_ON_CODE)
            {
                flag3 = true;
            }
        }

        if (flag1 == true)
        {
            timeCnt1++;
        }
        if (flag2 == true)
        {
            timeCnt2++;
        }
        if (flag3 == true)
        {
            timeCnt3++;
        }
        if (timeCnt1 == 100 && flag1 == true)
        {
            flag1 = false;
            timeCnt1 = 0;
            AppServo_Control(SERVO_Autofeed, 70);
        }
        if (timeCnt2 == 100 && flag2 == true)
        {
            flag2 = false;
            timeCnt2 = 0;
            AppServo_Control(SERVO_ControlDoor, 60);
        }
        if (((timeCnt3 % 10) == 0) && flag3 == true)
        {
            if (timeCnt3 == 500)
            {
                flag3 = false;
                timeCnt3 = 0;
            }
            else
            {
                ledSta = !ledSta;
                GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)(ledSta));
            }
        }
        vTaskDelay(10);

    }

}
static void Task_Uart( void *pvParameters )
{
    Serial_Begin(115200);
    uint8_t readData;
    uint32_t tickCnt = 0;
    while(1)
    {
        if (Serial_Available() > 0)
        {
            readData = Serial_Read();
            Handle_DataFromToEsp(readData);
        }
        g_senSorStruct.tem1 += 1;
        g_senSorStruct.hum1 += 1;
        g_senSorStruct.tem2 += 2;
        g_senSorStruct.hum2 += 2;
        g_senSorStruct.rain += 3;
        g_senSorStruct.light += 3;
        if (tickCnt == 200)
        {
            g_sensorBuffer[1] = g_senSorStruct.tem1;
            g_sensorBuffer[2] = g_senSorStruct.hum1;
            g_sensorBuffer[3] = g_senSorStruct.tem2;
            g_sensorBuffer[4] = g_senSorStruct.hum2;
            g_sensorBuffer[5] = g_senSorStruct.light;
            g_sensorBuffer[6] = g_senSorStruct.rain;
            g_sensorBuffer[7] = g_sensorBuffer[1] ^ g_sensorBuffer[2] ^ g_sensorBuffer[3] ^\
                                g_sensorBuffer[4] ^ g_sensorBuffer[5] ^ g_sensorBuffer[6];
            Serial_Printf((char*)g_sensorBuffer);
            tickCnt = 0;
        }
        tickCnt += 1;
        vTaskDelay(10);
        
    }
}

