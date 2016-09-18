#include "spi.h"
#include "ssd2828.h"
#include "delay.h"
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

SPI_HandleTypeDef SPI5_Handler;  //SPI���
#define SSD2828_DELAY_MS     {delay_ms(1);}
#define SSD2828_DELAY_US     {delay_us(500);}
////////////////////////////////////////////////////////////////////////////////// 	 
void SPI_3W_SET_Cmd(u8 cmd)
{
	u32 kk;
	
	SSD2828_MOSI = 0;  //Set DC=0, for writing to Command register
	SSD2828_CLK = 0;
	SSD2828_DELAY_MS;//add for delay
	SSD2828_CLK = 1;
	SSD2828_DELAY_MS;//add for delay
	SSD2828_CLK = 0;
	SSD2828_DELAY_MS;//add for delay
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) SSD2828_MOSI = 1;
		else         SSD2828_MOSI = 0;
		SSD2828_DELAY_US;
		SSD2828_CLK = 1;
		SSD2828_DELAY_US;//add for delay
		SSD2828_CLK = 0;
		SSD2828_DELAY_MS;
		cmd = cmd<<1;	
	}
}

void SPI_3W_SET_PAs(u8 value)
{
	u32 kk;

	SSD2828_MOSI = 1;			//Set DC=1, for writing to Data register
	SSD2828_CLK = 0;
	SSD2828_DELAY_MS;//add for delay
	SSD2828_CLK = 1;
	SSD2828_DELAY_MS;//add for delay
	SSD2828_CLK = 0;
	SSD2828_DELAY_MS;//add for delay
	for(kk=0;kk<8;kk++)
	{
		if((value&0x80)==0x80) SSD2828_MOSI = 1;
		else         SSD2828_MOSI = 0;
		SSD2828_DELAY_US;
		SSD2828_CLK = 1;
		SSD2828_DELAY_US;//add for delay
		SSD2828_CLK = 0;
		SSD2828_DELAY_MS;
		value = value<<1;	
	}	
}

//-----------------------------------------------------
void Write_com(u16 vv)
{
	SSD2828_CSN=0; 
	SPI_3W_SET_Cmd(vv&0xff);	
}

void Write_register(u16 vv)
{
	SPI_3W_SET_PAs(vv&0xff);
	SPI_3W_SET_PAs((vv>>8)&0xff);	
}
//-----------------------------------------------------

void SPI_2825_WrCmd(u8 cmd)
{
	u32 kk;
	
	SSD2828_CSN=0;
	
	SSD2828_MOSI = 0;			//Set DC=0, for writing to Command register
	SSD2828_CLK = 0;
	SSD2828_DELAY_MS;//add for delay
	SSD2828_CLK = 1;
	
	SSD2828_DELAY_MS;//add for delay
	SSD2828_CLK = 0;
	SSD2828_DELAY_MS;//add for delay
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) SSD2828_MOSI = 1;
		else         SSD2828_MOSI = 0;
		SSD2828_DELAY_US;
		SSD2828_CLK = 1;
		SSD2828_DELAY_US;//add for delay
		SSD2828_CLK = 0;
		SSD2828_DELAY_MS;
		cmd = cmd<<1;	
	}
	
	SSD2828_CSN=1;	
}

//void SPI_2825_WrCmd(u8)
void SPI_2825_WrReg(u8 c,u16 value)
{
	SSD2828_CSN=0;
	SPI_3W_SET_Cmd(c);
	SPI_3W_SET_PAs(value&0xff);
	SPI_3W_SET_PAs((value>>8)&0xff);	
	SSD2828_CSN=1;
}

u8 Read_PAs(void)
{
	u32 kk;
	u8  rdValue;

	rdValue = 0;
	for(kk=0;kk<8;kk++)
	{
		rdValue = rdValue<<1;
		
		SSD2828_CLK = 1;
		SSD2828_DELAY_US;//add for delay
		if(SSD2828_MISO) rdValue |= 0x01;
		SSD2828_CLK = 0;
		SSD2828_DELAY_MS;
	}
	
	return rdValue;	
}

u16   SPI_READ(void)
{
	u8  cmd,rdT;
	u16 reValue;
	u32 kk;
	
	SSD2828_CSN=0;

	SSD2828_MOSI = 0;				//Set DC=0, for writing to Command register
	SSD2828_CLK = 0;
	SSD2828_DELAY_MS;//add for delay
	SSD2828_CLK = 1;
	SSD2828_DELAY_MS;
	cmd = 0xB0;
	SSD2828_CLK = 0;
	SSD2828_DELAY_MS;//add for delay
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) SSD2828_MOSI = 1;
		else         SSD2828_MOSI = 0;
		SSD2828_DELAY_US;
		SSD2828_CLK = 1;
		SSD2828_DELAY_US;//add for delay
		SSD2828_CLK = 0;
		SSD2828_DELAY_MS;
		cmd = cmd<<1;	
	}
	
	SSD2828_DELAY_MS;
	
	SSD2828_MOSI = 0;			//Set DC=0, for writing to Command register
	SSD2828_CLK = 0;
	SSD2828_DELAY_MS;//add for delay
	SSD2828_CLK = 1;
	
	SSD2828_DELAY_MS;
	
	cmd = 0xFA;
	SSD2828_CLK = 0;
	SSD2828_DELAY_MS;//add for delay
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) SSD2828_MOSI = 1;
		else         SSD2828_MOSI = 0;
		SSD2828_DELAY_US;
		SSD2828_CLK = 1;
		SSD2828_DELAY_US;//add for delay
		SSD2828_CLK = 0;
		SSD2828_DELAY_MS;
		cmd = cmd<<1;	
	}	
	
	SSD2828_DELAY_MS;
	
	rdT=0;
	for(kk=0;kk<8;kk++)
	{
		rdT = rdT<<1;
		SSD2828_CLK = 1;
		SSD2828_DELAY_US;//add for delay
		if(SSD2828_MISO) rdT |= 0x01;
		SSD2828_CLK = 0;		
		SSD2828_DELAY_MS;		
	}
	
	reValue = rdT;

	SSD2828_DELAY_MS;//add for delay
	
	rdT=0;
	for(kk=0;kk<8;kk++)
	{
		rdT = rdT<<1;
		SSD2828_CLK = 1;
		SSD2828_DELAY_US;//add for delay
		if(SSD2828_MISO) rdT |= 0x01;
		SSD2828_CLK = 0;		
		SSD2828_DELAY_MS;
	}
	
	reValue += (rdT<<8);
	
	SSD2828_CSN=1;
	
	return reValue;			
}

void   SPI_READ_ID(void)
{
	SPI_2825_WrReg(0xd4, 0x00FA);
	printf("ID: 0x%x\n",SPI_READ());
}

void GP_COMMAD_PA(u16 num)
{
/////////////////////////
//	Write_com(0xB7);
//  SPI_WriteData(0x50);
//  SPI_WriteData(0x02);
//	
//	Write_com(0xBD);
//  SPI_WriteData(0x00);
//  SPI_WriteData(0x00);
/////////////////////////
	
	SPI_2825_WrReg(0xbc, num);
	Write_com(0x00bf);
	SSD2828_CSN=1;	
}

void SPI_WriteData(u8 value)
{
	SSD2828_CSN=0;
	SPI_3W_SET_PAs(value);
	SSD2828_CSN=1;	
}

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI5�ĳ�ʼ��
void SPI5_Init(void)
{
    SPI5_Handler.Instance=SPI5;                         //SP5
    SPI5_Handler.Init.Mode=SPI_MODE_MASTER;             //����SPI����ģʽ������Ϊ��ģʽ
    SPI5_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
    SPI5_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI5_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI5_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI5_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI5_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI5_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI5_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //�ر�TIģʽ
    SPI5_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//�ر�Ӳ��CRCУ��
    SPI5_Handler.Init.CRCPolynomial=7;                  //CRCֵ����Ķ���ʽ
    HAL_SPI_Init(&SPI5_Handler);
    
    __HAL_SPI_ENABLE(&SPI5_Handler);                    //ʹ��SPI5
    SPI5_ReadWriteByte(0Xff);                           //��������
}

//SPI5�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_SPI_Init()����
//hspi:SPI���
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOF_CLK_ENABLE();       //ʹ��GPIOFʱ��
    __HAL_RCC_SPI5_CLK_ENABLE();        //ʹ��SPI5ʱ��
    
    //PF7,8,9
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
    GPIO_Initure.Pull=GPIO_PULLUP;                  //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //����            
    GPIO_Initure.Alternate=GPIO_AF5_SPI5;           //����ΪSPI5
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
}

//SPI�ٶ����ú���
//SPI�ٶ�=fAPB1/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
//fAPB1ʱ��һ��Ϊ45Mhz��
void SPI5_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
    __HAL_SPI_DISABLE(&SPI5_Handler);            //�ر�SPI
    SPI5_Handler.Instance->CR1&=0XFFC7;          //λ3-5���㣬�������ò�����
    SPI5_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//����SPI�ٶ�
    __HAL_SPI_ENABLE(&SPI5_Handler);             //ʹ��SPI
    
}

//SPI5 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI5_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&SPI5_Handler,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //�����յ�������		
}
