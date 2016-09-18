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
 ALIENTEK STM32������STemWinʵ��
 STemwin BMPͼƬ��ʾ
 
 UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
 ����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
 ���ȼ�0���жϷ������������� OS_IntQTask()
 ���ȼ�1��ʱ�ӽ������� OS_TickTask()
 ���ȼ�2����ʱ���� OS_TmrTask()
 ���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
 ���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com  
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 �������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//�������ȼ�
#define START_TASK_PRIO				3
//�����ջ��С	
#define START_STK_SIZE 				128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//WIFI״̬���͵���
//�����������ȼ�
#define WIFI_STATUS_TASK_PRIO				4
//�����ջ��С
#define WIFI_STATUS_STK_SIZE				128
//������ƿ�
OS_TCB WifiStatusTaskTCB;
//�����ջ
CPU_STK WIFI_STATUS_TASK_STK[WIFI_STATUS_STK_SIZE];
//touch����
void wifistatus_task(void *p_arg);

//��������
//�����������ȼ�
#define KEY_TASK_PRIO 				5
//�����ջ��С
#define KEY_STK_SIZE				256
//������ƿ�
OS_TCB KeyTaskTCB;
//�����ջ
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
//led0����
void key_task(void *p_arg);

//P�˽�������
//�����������ȼ�
#define INTERACTIVE_TASK_PRIO 				6
//�����ջ��С
#define INTERACTIVE_STK_SIZE				128
//������ƿ�
OS_TCB InteractiveTaskTCB;
//�����ջ
CPU_STK INTERACTIVE_TASK_STK[INTERACTIVE_STK_SIZE];
//P�˽�������
void interactive_task(void *p_arg);

//EMWINDEMO����
//�����������ȼ�
#define EMWINDEMO_TASK_PRIO			8
//�����ջ��С
#define EMWINDEMO_STK_SIZE			1024
//������ƿ�
OS_TCB EmwindemoTaskTCB;
//�����ջ
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task����
void emwindemo_task(void *p_arg);

int main(void)
{
    OS_ERR err;
		CPU_SR_ALLOC();
    
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz   
    HAL_Init();                     //��ʼ��HAL��
    delay_init(180);                //��ʼ����ʱ����
    uart_init(115200);              //��ʼ��USART
		usart3_init(115200);  	
    LED_Init();                     //��ʼ��LED 
    KEY_Init();                     //��ʼ������
    SDRAM_Init();                   //SDRAM��ʼ��
    RGBLCD_Init();  		        //LCD��ʼ��
		SSD2828_Init();
    W25QXX_Init();				    //��ʼ��W25Q256
	
    my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
		my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
		my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ��

    exfuns_init();			        //Ϊfatfs��ر��������ڴ�				 
  	f_mount(fs[0],"0:",1); 		    //����SD�� 
    f_mount(fs[1],"1:",1); 	        //����FLASH.	  ���ﲻ��Ҫ����
    while(font_init())		        //��ʼ���ֿ�
	{
		LCD_ShowString(30,70,200,16,16,"Font Error!");
		while(SD_Init())	//���SD��
		{
			LCD_ShowString(30,90,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,90,200+30,70+16,WHITE);
			delay_ms(200);		    
		}
		update_font(30,90,16,"0:");	//����ֿⲻ���ھ͸����ֿ�
		delay_ms(2000);
		LCD_Clear(WHITE);	//����
		break;
	}

	OSInit(&err);		            //��ʼ��UCOSIII
	OS_CRITICAL_ENTER();            //�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	            //�˳��ٽ���	 
	OSStart(&err);                  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	//ʹ��ʱ��Ƭ��ת���ȹ���,����Ĭ�ϵ�ʱ��Ƭ����
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	__HAL_RCC_CRC_CLK_ENABLE();		//ʹ��CRCʱ��
	GUI_Init();  			//STemWin��ʼ��
  WM_SetCreateFlags(WM_CF_MEMDEV); //�������д��ڵĴ洢�豸
	WM_MULTIBUF_Enable(1);  //����STemWin�໺��,RGB�����ܻ��õ�
	OS_CRITICAL_ENTER();	//�����ٽ���
	//STemWin Demo����	
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
								 
	//��ȡWIFI״̬����
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
								 
	//interactive����					���ó�ʼ״̬Ϊ����
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
     
	//��������	
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
								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}

//EMWINDEMO����
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

//������������
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
				OSSchedLock(&err);		//����������
				LED1=0;
                emwinbmp_new_pathname(pname);//�õ��ļ���
                create_bmppicture(pname,0,0,lcddev.width,lcddev.height); //����BMP��ͼ            
                LED1=1;
				OSSchedUnlock(&err);	//����������
				break;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//��ʱ10ms
	}
}

//�������ȼ�����   APP�������ӵ�wifi����(͸��ģʽ�޷����) > �������� > P�˽������� + ������������ 
//APP�������ӵ�wifi����
void wifistatus_task(void *p_arg) 
{
	OS_ERR err;
	u16 rlen=0;
	while(1)
	{
		//����ڷ�APģʽ������� ��ѯ�������ӵ�״̬
		if(wifi_work_mode != WIFI_AP_SERVER_MODE)
		{
				//��������ʧ��
				if(atk_8266_consta_check() != '+')
				{
						//��Ҫ�л�P�˷�����IP����������������IP
						if(atk_8266_wifista_init())//����true��ʾʧ��
						{
							atk_8266_wifiap_init();
							//�ڴ������ݴ����������� �ֱ�����������д���
							wifi_work_mode = WIFI_AP_SERVER_MODE;//apģʽ ����wifi����� �Զ�����
						}
						else
						{
							//���ҿ���pure_task���� 
							wifi_work_mode = WIFI_STA_CLIENT_MODE;//������������ģʽ
						}
					
						OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//��ʱ100ms
				}
				else
				{
						OSTimeDlyHMSM(0,0,0,2000,OS_OPT_TIME_PERIODIC,&err);//��ʱ2s
				}
		}
		//�����APģʽ �ȴ��ͻ������ӶԴ��ڷ��ص����ݽ��д���
		else
		{
				//������ڽ��յ�����
				if(USART3_RX_STA&0X8000)
				{
						//USART3_RX_BUF
						rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
						USART3_RX_BUF[rlen]=0;		//���ӽ����� 
						printf("%s",USART3_RX_BUF);	//���͵�����   
						//��Դ��ڷ��͹�����ֵ ���н���
						//�����걣�浽SD����
						
						//ssid��psw������� ��wifiģʽ����ΪNONE ��������
						wifi_work_mode = WIFI_NONE_MODE;
					
						USART3_RX_STA = 0;
				}
				
				OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);//��ʱ100ms
		}
	}
}

//PURE����  ��������������һ������   ������������Ϊ��ʱ����
//����wifistatus_task ����
void interactive_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{		
			
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
	}
}


