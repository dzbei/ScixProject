#include "common.h"
#include "stdlib.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 WIFI STA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

char SSID[16];
char PSW[16];
u8 netpro=0;	//网络模式

//当初始化失败 即没有连接上热点 那么就返回失败 -1 然后切换到wifiap模式
u8 atk_8266_wifista_init(void)
{
	u8 ipbuf[16]; 	//IP缓存
	u8 res = 0;
	char *p;
	p=mymalloc(SRAMIN,32);							//申请32字节内存
	
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 
	delay_ms(1000);         //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	
	//SSID和PSW的值在SD卡里面读取
	sprintf(p,"AT+CWJAP=\"%.16s\",\"%.16s\"\r\n",SSID,PSW);	
	//估计等待超时    需要测试返回的结果   等待时间以10ms为单位
	/*
		连接失败时 返回 +CWJAP:1
												FAIL
		连接成功时 返回 WIFI CONNECTED
										WIFI GOT IP
										OK
	*/
	//等待3s
	res = atk_8266_send_cmd(p,"WIFI GOT IP",300);
		
//这一步放在 无线升级处理和P端交互
	//设置为单连接模式
//	atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
//	
//	//ipbuf即远端服务器的IP 需要配置  可以指定服务器地址  应该存放两个服务器地址 在两者之前进行切换
//	sprintf(p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);//端口号固定
//	res = atk_8266_send_cmd(p,"OK",200);         //连接服务器
//	//AT+CIPCLOSE   关闭连接  如果需要更换连接 则先关闭当前的服务器连接
//	/**
//	失败的话:
//		DNS Fail
//		ERROR
//	**/
//	if(!res)  
//	{
//		//设置为透传模式    TCP
//		res = atk_8266_send_cmd("AT+CIPMODE=1","OK",200);   
//	}
	
	myfree(SRAMIN,p);		//释放内存 
	
	return res;
	//配置完之后 进入正常数据交互状态      定时去检测是否需要升级 升级在晚上的时间进行 由于是单链路 所有不存在冲突问题
}



























