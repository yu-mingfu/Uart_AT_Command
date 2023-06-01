#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include "UART_QUEUE.h"

SqQueue uartQueue;

/***************************************************************************
*	函 数 名: Uart_RecvQueue_Init
*	功能说明: 串口接收队列初始化
*	形    参: 无
*	返 回 值: 无
****************************************************************************/
void Uart_RecvQueue_Init(void)
{
    uartQueue.queueHead = 0;
    uartQueue.queueTail = 0;
    uartQueue.queueLength = 0;
    memset(uartQueue.queueBuf, 0, sizeof(uartQueue.queueBuf));
}

/***************************************************************************
*	函 数 名: Uart_RecvQueue_Read
*	功能说明: 读队列数据
*	形    参: *data: 数据指针
*	返 回 值: bool:队列为空返回false,否则返回true
****************************************************************************/
bool Uart_RecvQueue_Read(uint8_t *data)
{
    if(uartQueue.queueLength == 0){
        return false;
    }
    *data = uartQueue.queueBuf[uartQueue.queueHead];
    uartQueue.queueBuf[uartQueue.queueHead] = 0;
    uartQueue.queueHead ++;
    uartQueue.queueHead %= QUEUE_DATA_MAX;  /* 防止数组溢出 */
    uartQueue.queueLength --;

    return true;
}

/***************************************************************************
*	函 数 名: Uart_RecvQueue_Write
*	功能说明: 写队列数据
*	形    参: data: 数据
*	返 回 值: bool:队列已满返回false,否则返回true
****************************************************************************/
bool Uart_RecvQueue_Write(uint8_t data)
{
    if(uartQueue.queueLength >= QUEUE_DATA_MAX)
    {
        return false;
    }
    uartQueue.queueBuf[uartQueue.queueTail] = data;
    uartQueue.queueTail ++;
    uartQueue.queueTail %= QUEUE_DATA_MAX; /* 防止数组溢出 */
    uartQueue.queueLength ++;
    return true;
}

/***************************************************************************
*	函 数 名: Get_RecvQueue_Len
*	功能说明: 写队列数据
*	形    参: data: 数据
*	返 回 值: uint8_t:返回队列中数据长度
****************************************************************************/
uint8_t Get_RecvQueue_Len(void)
{
    return uartQueue.queueLength;
}