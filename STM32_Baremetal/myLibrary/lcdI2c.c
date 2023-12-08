/*******************************************************************************
* Definitions
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "stm32f10x.h"
#include "delayTimer.h"
#include "lcdI2c.h"

#define PCF8574A_Address        0x27 << 1
#define I2C_Chanel              I2C2
/*******************************************************************************
* Prototypes
******************************************************************************/
static void LCD_Write_Byte(uint8_t data);
static void LCD_I2C_Configuration(void);
static void LCD_DataWrite(uint8_t data);
static void LCD_CmdWrite(uint8_t data);
/*******************************************************************************
* Code
******************************************************************************/
static void LCD_Write_Byte(uint8_t data)
{
    /* Send START condition */
    I2C_GenerateSTART(I2C_Chanel, ENABLE);
    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C_Chanel, I2C_EVENT_MASTER_MODE_SELECT));
    /* Send PCF8574A address for write */
    I2C_Send7bitAddress(I2C_Chanel, PCF8574A_Address, I2C_Direction_Transmitter);
    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C_Chanel, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    /* Send the byte to be written */
    I2C_SendData(I2C_Chanel, data);
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C_Chanel, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    /* Send STOP condition */
    I2C_GenerateSTOP(I2C_Chanel, ENABLE);
}

static void LCD_I2C_Configuration(void)
{
    GPIO_InitTypeDef                     GPIO_InitStructure;
    I2C_InitTypeDef                        I2C_InitStructure;

    // cap clock cho ngoai vi va I2C
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);                            // su dung kenh I2C2 cua STM32
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // cau hinh chan SDA va SCL
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;                        //PB10 - SCL, PB11 - SDA
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // cau hinh I2C
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0; //
    I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_Init(I2C2, &I2C_InitStructure);
    // cho phep bo I2C hoat dong
    I2C_Cmd(I2C2, ENABLE);

}

static void LCD_DataWrite(uint8_t data)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4], i = 0;
    data_u = data & 0xf0;
    data_l = (data << 4) & 0xf0;
    data_t[0] = data_u | 0x0d; //en=1, rs=0
    data_t[1] = data_u | 0x09; //en=0, rs=0
    data_t[2] = data_l | 0x0d; //en=1, rs=0
    data_t[3] = data_l | 0x09; //en=0, rs=0
    for (i = 0; i < 4; i++)
    {
        LCD_Write_Byte(data_t[i]);
    }

}

static void LCD_CmdWrite(uint8_t data)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4], i = 0;
    data_u = data & 0xf0;
    data_l = (data << 4) & 0xf0;
    data_t[0] = data_u | 0x04;          //en=1, rs=0
    data_t[1] = data_u;                      //en=0, rs=0
    data_t[2] = data_l | 0x04;          //en=1, rs=0
    data_t[3] = data_l | 0x08;          //en=0, rs=0
    for (i = 0; i < 4; i++)
    {
        LCD_Write_Byte(data_t[i]);
    }

}

void LCD_Init (void)
{
    DelayTimer_Init();
    LCD_I2C_Configuration();

    /* Set 4-bits interface */
    LCD_CmdWrite(0x33);
    DelayTimer_Ms(10);
    LCD_CmdWrite(0x32);
    DelayTimer_Ms(50);
    /* Start to set LCD function */
    LCD_CmdWrite(0x28);
    DelayTimer_Ms(50);
    /* clear LCD */
    LCD_CmdWrite(0x01);
    DelayTimer_Ms(50);
    /* wait 60ms */

    /* set entry mode */
    LCD_CmdWrite(0x06);
    DelayTimer_Ms(50);
    /* set display to on */
    LCD_CmdWrite(0x0C);
    DelayTimer_Ms(50);;
    /* move cursor to home and set data address to 0 */
    LCD_CmdWrite(0x02);
    DelayTimer_Ms(50);
}

void LCD_Clear(void)
{
    LCD_CmdWrite(0x01);
    DelayTimer_Ms(10);
}

void LCD_Gotoxy(uint8_t y, uint8_t x)
{
    switch (x)
    {
    case 0: LCD_CmdWrite ((0x80 | 0x00) + y); break;
    case 1: LCD_CmdWrite ((0x80 | 0x40) + y); break;
    case 2: LCD_CmdWrite ((0x80 | 0x14) + y); break;
    case 3: LCD_CmdWrite ((0x80 | 0x54) + y); break;
    }
}

void LCD_Printf(const char* str, ...)
{
    char stringArray[20];

    va_list args;
    va_start(args, str);
    vsprintf(stringArray, str, args);
    va_end(args);

    for (uint8_t i = 0;  i < strlen(stringArray) && i < 20; i++)
    {
        LCD_DataWrite((uint8_t)stringArray[i]);
    }
}

