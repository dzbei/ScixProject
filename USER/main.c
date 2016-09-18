#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "rgblcd.h"
#include "adc.h"
#include "pcf8574.h"
#include "timer.h"
#include "sdram.h"
#include "malloc.h"
#include "touch.h"
#include "rtc.h"
#include "GUI.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "ftl.h" 
#include "exfuns.h"
#include "w25qxx.h"
#include "fontupd.h"
#include "EmWinHZFont.h"
#include "mytext.h"
#include "WM.h"
#include "includes.h"
#include "DIALOG.h"
#include "Hzfontupdata.h"
#include "bmpdisplay.h"
#include "gifdisplay.h"
#include "jpegdisplay.h"
#include "pngdisplay.h"
#include "ssd2828.h"
#include "common.h"
#include "ttffontcreate.h"
/************************************************
 ALIENTEK STM32开发板STemWin实验
 STemwin BMP图片显示
 
 UCOSIII中以下优先级用户程序不能使用，ALIENTEK
 将这些优先级分配给了UCOSIII的5个系统内部任务
 优先级0：中断服务服务管理任务 OS_IntQTask()
 优先级1：时钟节拍任务 OS_TickTask()
 优先级2：定时任务 OS_TmrTask()
 优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
 优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//任务优先级
#define START_TASK_PRIO				3
//任务堆栈大小	
#define START_STK_SIZE 				128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//WIFI状态检测和调整
//设置任务优先级
#define WIFI_STATUS_TASK_PRIO				4
//任务堆栈大小
#define WIFI_STATUS_STK_SIZE				128
//任务控制块
OS_TCB WifiStatusTaskTCB;
//任务堆栈
CPU_STK WIFI_STATUS_TASK_STK[WIFI_STATUS_STK_SIZE];
//touch任务
void wifistatus_task(void *p_arg);

//按键任务
//设置任务优先级
#define KEY_TASK_PRIO 				5
//任务堆栈大小
#define KEY_STK_SIZE				256
//任务控制块
OS_TCB KeyTaskTCB;
//任务堆栈
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
//led0任务
void key_task(void *p_arg);

//P端交互任务
//设置任务优先级
#define INTERACTIVE_TASK_PRIO 				6
//任务堆栈大小
#define INTERACTIVE_STK_SIZE				128
//任务控制块
OS_TCB InteractiveTaskTCB;
//任务堆栈
CPU_STK INTERACTIVE_TASK_STK[INTERACTIVE_STK_SIZE];
//P端交互任务
void interactive_task(void *p_arg);

//EMWINDEMO任务
//设置任务优先级
#define EMWINDEMO_TASK_PRIO			8
//任务堆栈大小
#define EMWINDEMO_STK_SIZE			1024
//任务控制块
OS_TCB EmwindemoTaskTCB;
//任务堆栈
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task任务
void emwindemo_task(void *p_arg);

int main(void)
{
    OS_ERR err;
		CPU_SR_ALLOC();
    
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz   
    HAL_Init();                     //初始化HAL库
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
		usart3_init(115200);  	
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //SDRAM初始化
    RGBLCD_Init();  		        //LCD初始化
		SSD2828_Init();
    W25QXX_Init();				    //初始化W25Q256
	
    my_mem_init(SRAMIN);		    //初始化内部内存池
		my_mem_init(SRAMEX);		    //初始化外部内存池
		my_mem_init(SRAMCCM);		    //初始化CCM内存池

    exfuns_init();			        //为fatfs相关变量申请内存				 
  	f_mount(fs[0],"0:",1); 		    //挂载SD卡 
    f_mount(fs[1],"1:",1); 	        //挂载FLASH.	  这里不需要挂载
    while(font_init())		        //初始化字库
	{
		LCD_ShowString(30,70,200,16,16,"Font Error!");
		while(SD_Init())	//检测SD卡
		{
			LCD_ShowString(30,90,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,90,200+30,70+16,WHITE);
			delay_ms(200);		    
		}
		update_font(30,90,16,"0:");	//如果字库不存在就更新字库
		delay_ms(2000);
		LCD_Clear(WHITE);	//清屏
		break;
	}

	OSInit(&err);		            //初始化UCOSIII
	OS_CRITICAL_ENTER();            //进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	            //退出临界区	 
	OSStart(&err);                  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	//使能时间片轮转调度功能,设置默认的时间片长度
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	__HAL_RCC_CRC_CLK_ENABLE();		//使能CRC时钟
	GUI_Init();  			//STemWin初始化
  WM_SetCreateFlags(WM_CF_MEMDEV); //启动所有窗口的存储设备
	WM_MULTIBUF_Enable(1);  //开启STemWin多缓冲,RGB屏可能会用到
	OS_CRITICAL_ENTER();	//进入临界区
	//STemWin Demo任务	
	OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,		
				 (CPU_CHAR*   )"Emwindemo task", 		
                 (OS_TASK_PTR )emwindemo_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,     
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
								 
	//获取WIFI状态任务
	OSTaskCreate((OS_TCB*     )&WifiStatusTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )wifistatus_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )WIFI_STATUS_TASK_PRIO,     
                 (CPU_STK*    )&WIFI_STATUS_TASK_STK[0],	
                 (CPU_STK_SIZE)WIFI_STATUS_STK_SIZE/10,	
                 (CPU_STK_SIZE)WIFI_STATUS_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);		
								 
	//interactive任务					设置初始状态为挂起
	OSTaskCreate((OS_TCB*     )&InteractiveTaskTCB,		
				 (CPU_CHAR*   )"interactive task", 		
                 (OS_TASK_PTR )interactive_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )INTERACTIVE_TASK_PRIO,     
                 (CPU_STK*    )&INTERACTIVE_TASK_STK[0],	
                 (CPU_STK_SIZE)INTERACTIVE_STK_SIZE/10,	
                 (CPU_STK_SIZE)INTERACTIVE_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
     
	//按键任务	
	OSTaskCreate((OS_TCB*     )&KeyTaskTCB,		
				 (CPU_CHAR*   )"Key task", 		
                 (OS_TASK_PTR )key_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )KEY_TASK_PRIO,     
                 (CPU_STK*    )&KEY_TASK_STK[0],	
                 (CPU_STK_SIZE)KEY_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEY_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);      
								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}

//EMWINDEMO任务
void emwindemo_task(void *p_arg)
{
	
	int result;
	result=Create_TTFFont("0:/SYSTEM/FONT/ScixFont/VisbyCF-MediumOblique.ttf");
	if(result) printf("TTF??????,%d\r\n",result);
	
	GUI_CURSOR_Show();
	GUI_SetBkColor(GUI_CYAN);
	GUI_SetColor(GUI_RED);
  GUI_UC_SetEncodeUTF8();
	GUI_Clear();
	GUI_SetFont(&TTF12_Font);
	GUI_DispStringAt("ALIENTEK STemWin TTF Font Test 0123456789!",5,10);
	GUI_SetFont(&TTF18_Font);
	GUI_DispStringAt("ALIENTEK STemWin TTF Font Test 0123456789!",5,25);
	GUI_SetFont(&TTF24_Font);
	GUI_DispStringAt("ALIENTEK STemWin TTF Font Test 0123456789!",5,45);
	GUI_SetFont(&TTF36_Font);
	GUI_DispStringAt("ALIENTEK STemWin TTF Font Test 0123456789!",5,70);
	while(1)
	{
       GUI_Delay(100);
	}
}

//按键处理任务
void key_task(void *pdata)
{
    u8 pname[30];
	u8 key;
	OS_ERR err;
	while(1)
	{
		key = KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES:
				OSSchedLock(&err);		//调度器上锁
				LED1=0;
                emwinbmp_new_pathname(pname);//得到文件名
                create_bmppicture(pname,0,0,lcddev.width,lcddev.height); //创建BMP截图            
                LED1=1;
				OSSchedUnlock(&err);	//调度器解锁
				break;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//延时10ms
	}
}

//任务优先级排序   APP设置连接的wifi密码(透传模式无法检测) > 按键任务 > P端交互任务 + 无线升级任务 
//APP设置连接的wifi密码
void wifistatus_task(void *p_arg) 
{
	OS_ERR err;
	u16 rlen=0;
	while(1)
	{
		//如果在非AP模式的情况下 查询网络连接的状态
		if(wifi_work_mode != WIFI_AP_SERVER_MODE)
		{
				//网络连接失败
				if(atk_8266_consta_check() != '+')
				{
						//需要切换P端服务器IP和无线升级服务器IP
						if(atk_8266_wifista_init())//返回true表示失败
						{
							atk_8266_wifiap_init();
							//在串口数据处理任务里面 分别对这两个进行处理
							wifi_work_mode = WIFI_AP_SERVER_MODE;//ap模式 设置wifi密码后 自动重启
						}
						else
						{
							//并且开启pure_task任务 
							wifi_work_mode = WIFI_STA_CLIENT_MODE;//进入正常工作模式
						}
					
						OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//延时100ms
				}
				else
				{
						OSTimeDlyHMSM(0,0,0,2000,OS_OPT_TIME_PERIODIC,&err);//延时2s
				}
		}
		//如果在AP模式 等待客户端连接对串口返回的数据进行处理
		else
		{
				//如果串口接收到数据
				if(USART3_RX_STA&0X8000)
				{
						//USART3_RX_BUF
						rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
						USART3_RX_BUF[rlen]=0;		//添加结束符 
						printf("%s",USART3_RX_BUF);	//发送到串口   
						//针对串口发送过来的值 进行解析
						//解析完保存到SD卡中
						
						//ssid和psw保存完后 把wifi模式设置为NONE 重新连接
						wifi_work_mode = WIFI_NONE_MODE;
					
						USART3_RX_STA = 0;
				}
				
				OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//延时100ms
		}
	}
}

//PURE任务  与无线升级共用一个任务   设置无线升级为定时升级
//挂起wifistatus_task 任务
void interactive_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{		
			
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//延时500ms
	}
}



