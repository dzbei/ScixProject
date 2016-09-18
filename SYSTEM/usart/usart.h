#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	

extern UART_HandleTypeDef UART1_Handler; //UART���

//����3��غ궨��
#define USART3_MAX_RECV_LEN		800					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		800					//����ͻ����ֽ���

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern u16 USART3_RX_STA;   						//��������״̬

void u3_printf(char* fmt,...);
void usart3_init(u32 bound);				//����3��ʼ�� 
void TIM7_Int_Init(u16 arr,u16 psc);

//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void usart3_init(u32 bound);
#endif
