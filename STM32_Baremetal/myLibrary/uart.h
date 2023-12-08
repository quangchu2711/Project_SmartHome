#ifndef UART_H
#define UART_H
/*******************************************************************************
* API
******************************************************************************/
uint8_t UART_Read(void);
uint16_t UART_Available(void);
void UART_Begin (uint32_t baud);
void UART_Printf(const char* str, ...);
#endif
