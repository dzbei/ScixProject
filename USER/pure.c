#include "pure.h"

extern OS_TCB WifiStatusTaskTCB;
extern OS_TCB InteractiveTaskTCB;
extern OS_TCB OTATaskTCB;

//PURE任务  与无线升级共用一个任务   设置无线升级为定时升级  tcp client方式工作
//挂起wifistatus_task 任务       如果分为两个任务 当客户端切换服务器ip时 会导致断开连接
void interactive_task(void *p_arg)
{
	OS_ERR err;
	u16 rlen=0;
	static u32 cnt = 0;
	static u8 first_init = 0;
	
	CPU_SR_ALLOC();
	
	//根据协议解析数据          初始化网络通信模式 
	while(1)
	{				
			cnt++;//做为系统OTA升级定时器 24小时  24*60*60*60*2s   以500ms为计时单位 
		
			if(cnt > 3*60*60*60*2) //3小时
			{
					cnt = 0;
					first_init = 0;
					OS_TaskResume((OS_TCB*)&OTATaskTCB,&err);//启动ota任务
					OS_TaskSuspend((OS_TCB*)&InteractiveTaskTCB,&err);		//挂起自己
			}
			else
			{
					if(atk_8266_staclient_init(1)&&(first_init==0))
					{
						//网络连接失败
						OS_TaskResume((OS_TCB*)&WifiStatusTaskTCB,&err);//启动wifi任务
						OS_TaskSuspend((OS_TCB*)&InteractiveTaskTCB,&err);		//挂起自己
					}
									
					//如果串口接收到数据  
					if(USART3_RX_STA&0X8000)
					{
							OS_CRITICAL_ENTER();	//进入临界模式
							first_init = 1;
							rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
							USART3_RX_BUF[rlen]=0;		//添加结束符 
							printf("%s",USART3_RX_BUF);	//发送到串口   
							//添加处理函数
							USART3_RX_STA = 0;
							OS_CRITICAL_EXIT();//退出临界状态
					}
					else
					{
							//在这里判断超时值
							//超过指定时间没有数据交互则认为断网
																
							first_init = 0;
							OS_TaskResume((OS_TCB*)&WifiStatusTaskTCB,&err);//启动wifi任务
							OS_TaskSuspend((OS_TCB*)&InteractiveTaskTCB,&err);		//挂起自己
					}
	  	}
		
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//延时500ms
	}
}