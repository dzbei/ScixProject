#include "common.h"

extern char SSID[];
extern char PSW[];

//用于配置wifi密码       这不需要出错处理
void atk_8266_wifiap_init(void)
{
		OS_ERR err;
		u8 *p;
		p=mymalloc(SRAMIN,32);							//申请32字节内存
	
		memset(SSID,0,16);
		memset(PSW,0,16);

		atk_8266_send_cmd("AT+CWMODE=2","OK",50);		//设置WIFI AP模式
		atk_8266_send_cmd("AT+RST","OK",20);	
	
		OSTimeDly(3000,0,&err);
	
		//设置供外部连接的SSID和PSW    1代表通道号     4代表加密方式为WPA_WPA2_PSK
		sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",%d,%d","esp8266_test","12345678",1, 4);
		atk_8266_send_cmd(p,"OK",20);	
	
		//开启多连接和设置服务器端口号
		atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
		sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
		atk_8266_send_cmd(p,"OK",20);     //开启Server模式，端口号为8086
		//等待外部连接
		
		myfree(SRAMIN,p);
		
		//开启了Server模式后 开始接收APK端发过来的数据  可以先用串口跑通   配置完重启
		//client端可以直接发送数据过来 这是收到的数据    +IPD,0,8:12345678
		//client端 在connect的时候 显示的数据   0,CONNECT
		/*
		OK
		AT+CIPMUX=1
		OK
		AT+CIPSERVER=1,8086
		OK
		0,CONNECT
		+IPD,0,6:1234560,CLOSED
		*/
		//定时进行网络连接状态监控
}		






