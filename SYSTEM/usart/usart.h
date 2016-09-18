#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	

extern UART_HandleTypeDef UART1_Handler; //UART句柄

//串口3相关宏定义
#define USART3_MAX_RECV_LEN		800					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		800					//最大发送缓存字节数

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern u16 USART3_RX_STA;   						//接收数据状态

void u3_printf(char* fmt,...);
void usart3_init(u32 bound);				//串口3初始化 
void TIM7_Int_Init(u16 arr,u16 psc);

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void usart3_init(u32 bound);
#endif
