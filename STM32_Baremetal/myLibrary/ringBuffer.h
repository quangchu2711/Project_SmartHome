#ifndef RING_BUFFER_H
#define RING_BUFFER_H
/*******************************************************************************
* Definitions
******************************************************************************/
typedef struct
{
    uint8_t *buffer;
    int8_t head;
    int8_t tail;
    uint16_t maxLen;
} RING_BUFFER;
/*******************************************************************************
* API
******************************************************************************/
int8_t RB_Push(RING_BUFFER *rb, uint8_t data);
int8_t RB_Pop(RING_BUFFER *rb, uint8_t *data);
uint16_t RB_Available(RING_BUFFER *rb);
void RB_Init(RING_BUFFER *rb, uint8_t *data, uint16_t len);
#endif
