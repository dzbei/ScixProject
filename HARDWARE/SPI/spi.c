#include "spi.h"
#include "ssd2828.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//SPI驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/16/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

SPI_HandleTypeDef SPI5_Handler;  //SPI句柄
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

//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI5的初始化
void SPI5_Init(void)
{
    SPI5_Handler.Instance=SPI5;                         //SP5
    SPI5_Handler.Init.Mode=SPI_MODE_MASTER;             //设置SPI工作模式，设置为主模式
    SPI5_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI5_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI5_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //串行同步时钟的空闲状态为高电平
    SPI5_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI5_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI5_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//定义波特率预分频的值:波特率预分频值为256
    SPI5_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI5_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //关闭TI模式
    SPI5_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
    SPI5_Handler.Init.CRCPolynomial=7;                  //CRC值计算的多项式
    HAL_SPI_Init(&SPI5_Handler);
    
    __HAL_SPI_ENABLE(&SPI5_Handler);                    //使能SPI5
    SPI5_ReadWriteByte(0Xff);                           //启动传输
}

//SPI5底层驱动，时钟使能，引脚配置
//此函数会被HAL_SPI_Init()调用
//hspi:SPI句柄
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOF_CLK_ENABLE();       //使能GPIOF时钟
    __HAL_RCC_SPI5_CLK_ENABLE();        //使能SPI5时钟
    
    //PF7,8,9
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //快速            
    GPIO_Initure.Alternate=GPIO_AF5_SPI5;           //复用为SPI5
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
}

//SPI速度设置函数
//SPI速度=fAPB1/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
//fAPB1时钟一般为45Mhz：
void SPI5_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&SPI5_Handler);            //关闭SPI
    SPI5_Handler.Instance->CR1&=0XFFC7;          //位3-5清零，用来设置波特率
    SPI5_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&SPI5_Handler);             //使能SPI
    
}

//SPI5 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI5_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
    HAL_SPI_TransmitReceive(&SPI5_Handler,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //返回收到的数据		
}
