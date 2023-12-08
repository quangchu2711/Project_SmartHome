#ifndef _LIGHT_H_
#define _LIGHT_H_

typedef enum
{
    LIGHT_NUMBER_1 = 0x00U,
    LIGHT_NUMBER_2 = 0x01U,
    LIGHT_NUMBER_3 = 0x02U,
} Light_number_t;

uint8_t LIGHT_Read(Light_number_t number);
void LIGHT_Init(void);

#endif
