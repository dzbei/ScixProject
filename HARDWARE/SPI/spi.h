#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//SPI��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/16/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

extern SPI_HandleTypeDef SPI5_Handler;  //SPI���

void SPI5_Init(void);
void SPI5_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI5_ReadWriteByte(u8 TxData);

void SPI_3W_SET_Cmd(u8 cmd);
void SPI_3W_SET_PAs(u8 value);
void Write_com(u16 vv);
void Write_register(u16 vv);
void SPI_2825_WrCmd(u8 cmd);
void SPI_2825_WrReg(u8 c,u16 value);
u8 Read_PAs(void);
u16   SPI_READ(void);
void  SPI_READ_ID(void);
void GP_COMMAD_PA(u16 num);
void SPI_WriteData(u8 value);
#endif
