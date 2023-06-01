#include <Arduino.h>
#include "UART_QUEUE.h"

#define ATOK 		  0						//设置函数索引
#define ATCMD  		1
#define ATCREG 		2


/*********  工作状态处理  *********/
typedef struct
{
    uint8_t mStatus;
    uint8_t (* Funtion)(void); //函数指针的形式
} WorkStatus_TypeDef;  

// 模拟AT指令发送函数
uint8_t AT_OK(void)
{
    printf("AT_Send_OK\r\n");
    return 0;
}

uint8_t AT_CMD(void)
{
    printf("AT_Send_CMD\r\n");
    return 0;
}

uint8_t AT_CREG(void)
{
    printf("AT_Send_CREG\r\n");
    return 0;
}

WorkStatus_TypeDef WorkStatus_Tab[] =
{				//mStatus	 函数指针
  {ATOK,       AT_OK}, 		
  {ATCMD,      AT_CMD}, 		
  {ATCREG,     AT_CREG}, 		
};

uint8_t * buffer = (uint8_t*)malloc(50);	//用于接收队列中的字符，保存拼接后的字符串

uint8_t flag = 0;
uint8_t AtSendFlag = 0;
uint8_t at_index = 0;
uint16_t cnt = 0;
hw_timer_t *timer;  						//定时器	


// 预期的AT命令返回值，与接收到的进行比较
uint8_t * RetCmd(uint8_t index)
{
  uint8_t *buf = (uint8_t*)malloc(50);
  uint8_t *retcmd1 = (uint8_t *)"OK\r\n";
  uint8_t *retcmd2 = (uint8_t *)"CMD\r\n";
  uint8_t *retcmd3 = (uint8_t *)"CREG\r\n";
  switch(index)
  {
    case 0: buf = retcmd1;break;
    case 1: buf = retcmd2;break;
    case 2: buf = retcmd3;break;
  }
  return buf;
}

uint8_t AT_WorkStatus_Call(uint8_t Start)
{
    uint8_t i = 0;
    for(i = 0; i < 3; i++)
    {
        if(Start == WorkStatus_Tab[i].mStatus)
        {
            //printf("tab %d = %d  ",i,M26_WorkStatus_Tab[i].Funtion());
            return WorkStatus_Tab[i].Funtion();
        }
    }
    return 0;
}

/* 定时器中断函数*/
void IRAM_ATTR onTimer() 
{   
  flag = 1;		//定时器计时100ms，表示串口已经接收完一帧数据
  cnt++;		
}

//处理串口接收到的数据
void Data_Handle(void)
{
  uint8_t data;
  uint8_t tmp[2];
 // uint8_t *buf = (uint8_t *)"OK\r\n";
  if(Uart_RecvQueue_Read(&data))
  {
    sprintf((char*)tmp,"%c",data);
    strcat((char*)buffer,(char*)tmp);		//字符拼接
    if(Get_RecvQueue_Len() == 0)
    {
      Serial.printf("buffer = %s\r\n",buffer);
      if(strcmp((char*)buffer,(char*)RetCmd(at_index)) == 0)	//字符串比较，相同则发送下一条命令，并清空buffer
      {
        Serial.println("same");
        memset(buffer,0,50);
        AtSendFlag = 1;
      }
      else
      {
        Serial.println("different");							//不同则继续发送本条命令，并清空buffer
        memset(buffer,0,50);
        AtSendFlag = 0;
      }
    }
 }
  flag = 0;

  if(cnt >= 10)			// 1s 发送一次AT命令
  {
    if(AtSendFlag == 1)	//判断字符串相同
    {
      at_index++;		//索引到下一条命令
      AT_WorkStatus_Call(at_index);//调用与索引值相对应的函数来实现条命令
      AtSendFlag = 0;
    }
    else
    {
      AT_WorkStatus_Call(at_index);
    }
    cnt = 0;
  }
}

void setup() {
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("ESP32 Uart Init Successful");
  Uart_RecvQueue_Init();
  memset(buffer,0,50);

  timer = timerBegin(0, 80, true);                //定时器初始化
  timerAttachInterrupt(timer, &onTimer, true);    //调用中断函数
  timerAlarmWrite(timer, 100000, true);             //timerBegin的参数二 80位80MHZ，
  timerAlarmEnable(timer);                        //定时器使能
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t rev = 0;
  if(Serial.available())		//串口接收函数
  {
    rev=Serial.read();
    Uart_RecvQueue_Write(rev);

    }
    if(Serial.available() == 0 && flag == 1){	//串口完成接收，进行数据拼接处理
      Data_Handle();
    }
}