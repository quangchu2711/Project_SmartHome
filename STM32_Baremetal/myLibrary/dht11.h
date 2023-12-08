#ifndef DHT11_H_
#define DHT11_H_

#include "stm32f10x.h"

typedef enum
{
    PORTA = 0x00U,
    PORTB =0x001U,
    PORTC = 0x02U
} PORT_t;

void DHT11_Init(PORT_t port, uint16_t gpioPin);
uint8_t DHT11_Read(PORT_t port, uint16_t gpioPin, uint8_t *pTem, uint8_t *pHum);

#endif
