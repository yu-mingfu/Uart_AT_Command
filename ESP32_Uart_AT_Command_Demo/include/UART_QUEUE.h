#ifndef __UART_QUEUE_H
#define __UART_QUEUE_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#define QUEUE_DATA_MAX 50

typedef struct _SqQueue
{
    uint8_t queueTail;                     /* 队列尾，入队时需要自加 */
    uint8_t queueHead;                     /* 队列头，出队时需要自减 */
    uint8_t queueLength;                   /* 保存的是当前队列有效数据的长度 */
    uint8_t queueBuf[QUEUE_DATA_MAX];      /* 数据缓冲区 */
}SqQueue;

void Uart_RecvQueue_Init(void);
bool Uart_RecvQueue_Read(uint8_t *data);
bool Uart_RecvQueue_Write(uint8_t data);
uint8_t Get_RecvQueue_Len(void);


#ifdef __cplusplus
} /*extern "C"*/
#endif
#endif