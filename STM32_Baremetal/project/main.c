#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "stm32f10x.h"
#include "delayTimer.h"
#include "lcdI2c.h"
#include "dht11.h"
#include "light.h"
#include "servo.h"
#include "uart.h"
#include "rtc.h"
#include "appTimeManage.h"
#include "button.h"

//Define
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

//Typedef
typedef struct sensor_data
{
    uint8_t tem1;
    uint8_t tem2;
    uint8_t hum1;
    uint8_t hum2;
    uint8_t rain;
    uint8_t light;
} Sensor_valueStruct_t;

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

//Variable for sensor
Sensor_valueStruct_t g_senSorStruct;
uint8_t g_sensorBuffer[10];

//Variable for Time
Mode_SystemTime_t g_SysTimeMode;
Time_Handle_t g_currentTimeEdit;
Mode_SystemTime_t g_timeChangeOption;
AppTimeManage_TimeValue_t g_TimeVal[4];
char g_optionInfor[5][16] = {"   [Edit Time] ", "  [Alarm Time] ", "   [Feed Time] ", "  [Door Time]  ", "     [Exit] "};
uint8_t g_flagCheckPressRL_Button = 0;
uint32_t g_TimeRun;
uint8_t g_flagDisplay;

//Variable for Button
ButtonManagement g_mButton;
ButtonManagement g_rButton;
ButtonManagement g_lButton;

//Static function
static void Button_Init(void);
static void Button_CheckMode(void);
static void Button_EditTime( AppTimeManage_TimeValue_t *pTime);
static void Button_HandleData(Time_Handle_t timeHandle, AppTimeManage_TimeValue_t *pHandleData);

static void Mode_DisplayAndHandleData(void);
static void Mode_SelectOption(void);

static void GPIO_Config(void);

uint8_t g_Tem1, g_Hum1;

int main()
{
    UART_Begin(115200);

    AppTimeManage_Init();

    LCD_Init();

    SysTick_Init();

    Button_Init();

    GPIO_Config();

    DelayTimer_Init();

//    DHT11_Init(PORTB, GPIO_Pin_6);
//    uint8_t g_Tem1, g_Hum1;
//    DHT11_Read(PORTB, GPIO_Pin_6, &g_Tem1, &g_Hum1);
    
    DHT11_Init(PORTB, GPIO_Pin_8);
    DHT11_Read(PORTB, GPIO_Pin_8, &g_senSorStruct.tem1, &g_senSorStruct.hum1);
    DHT11_Init(PORTB, GPIO_Pin_5);
    DHT11_Read(PORTB, GPIO_Pin_5, &g_senSorStruct.tem2, &g_senSorStruct.hum2);    
//    SERVO_Init(GPIOA, GPIO_Pin_6);
//    SERVO_Write(GPIOA, GPIO_Pin_6, 0);
//    DelayTimer_Ms(1000);
//    SERVO_Write(GPIOA, GPIO_Pin_6, 180);
//    DelayTimer_Ms(1000);
    LIGHT_Init();

    uint32_t tickCnt;
    uint8_t readData;

    g_SysTimeMode = MODE_Display;

    g_currentTimeEdit = TIME_Hour;
    g_flagDisplay = 0;
    g_timeChangeOption = MODE_Display;

    uint8_t i;
    for (i = 0; i < sizeof(g_sensorBuffer)/sizeof(g_sensorBuffer[0]); i++)
    {
        g_sensorBuffer[i] = 0x00;
    }

    uint8_t ledSta = 0;
    uint8_t testValue = 15;
    uint8_t testValue1 = 10;

    while (1)
    {
        g_senSorStruct.light = LIGHT_Read(LIGHT_NUMBER_1);
        g_senSorStruct.rain = LIGHT_Read(LIGHT_NUMBER_2);
        DHT11_Read(PORTB, GPIO_Pin_8, &g_senSorStruct.tem1, &g_senSorStruct.hum1);
        DHT11_Read(PORTB, GPIO_Pin_5, &g_senSorStruct.tem2, &g_senSorStruct.hum2);

        if (UART_Available() > 0)
        {
            readData = UART_Read();
//            Handle_DataFromToEsp(readData);
        }

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

        if (Timer_GetTickMs() - tickCnt > 800)
        {
//            g_sensorBuffer[1] = testValue;
//            g_sensorBuffer[2] = testValue;
//            g_sensorBuffer[3] = testValue;
//            g_sensorBuffer[4] = testValue;
            g_sensorBuffer[1] = g_senSorStruct.tem1;
            g_sensorBuffer[2] = g_senSorStruct.hum1;
            g_sensorBuffer[3] = g_senSorStruct.tem2;
            g_sensorBuffer[4] = g_senSorStruct.hum2;
            g_sensorBuffer[5] = g_senSorStruct.light;
            g_sensorBuffer[6] = g_senSorStruct.rain;
            g_sensorBuffer[7] = g_sensorBuffer[1] ^ g_sensorBuffer[2] ^ g_sensorBuffer[3] ^\
                                g_sensorBuffer[4] ^ g_sensorBuffer[5] ^ g_sensorBuffer[6];


            g_sensorBuffer[0] = '(';
            g_sensorBuffer[8] = ')';
            g_sensorBuffer[9] = '\0';


            UART_Printf((char*)g_sensorBuffer);
            ledSta = !ledSta;
            GPIO_WriteBit(GPIOC, GPIO_Pin_13, ledSta);
            tickCnt = Timer_GetTickMs();
        }
    }
}

//Time fucitons
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
    bool checkSendQueue = false;

    /* Hien thi thong tin len man hinh LCD: time va sensor */
    AppTimeManage_GetCurrentTime(&g_TimeVal[MODE_ChangeTime]);
    LCD_Gotoxy(0, 0);
    LCD_Printf("%02d:%02d:%02d|%02d%c%02d%c ", g_TimeVal[MODE_ChangeTime].Hour, g_TimeVal[MODE_ChangeTime].Minute, g_TimeVal[MODE_ChangeTime].Second, \
                                         g_senSorStruct.light, '%', g_senSorStruct.rain, '%');

    LCD_Gotoxy(0, 1);
    LCD_Printf("%02d%cC%02d%c %02d%cC%02d%c", g_senSorStruct.tem1, 223, g_senSorStruct.hum1, '%', g_senSorStruct.tem2, 223, g_senSorStruct.hum2, '%');
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

static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_Struct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Struct.GPIO_Pin =  GPIO_Pin_13;
    GPIO_Struct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOC, &GPIO_Struct);
}

