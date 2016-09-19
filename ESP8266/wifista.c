#include "common.h"
#include "stdlib.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� WIFI STA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

char SSID[16];
char PSW[16];
u8 netpro=0;	//����ģʽ

//����ʼ��ʧ�� ��û���������ȵ� ��ô�ͷ���ʧ�� -1 Ȼ���л���wifiapģʽ
u8 atk_8266_wifista_init(void)
{
	OS_ERR err;
	u8 ipbuf[16]; 	//IP����
	u8 res = 0;
	char *p;
	
	CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();
	
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 

	OSTimeDly(3000,0,&err);
	
	//SSID��PSW��ֵ��SD�������ȡ
	sprintf(p,"AT+CWJAP=\"%.16s\",\"%.16s\"\r\n",SSID,PSW);	
	//���Ƶȴ���ʱ    ��Ҫ���Է��صĽ��   �ȴ�ʱ����10msΪ��λ
	/*
		����ʧ��ʱ ���� +CWJAP:1
												FAIL
		���ӳɹ�ʱ ���� WIFI CONNECTED
										WIFI GOT IP
										OK
	*/
	//�ȴ�3s
	res = atk_8266_send_cmd(p,"WIFI GOT IP",300);
			
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	
	OS_CRITICAL_EXIT();
		
	return res;
	//������֮�� �����������ݽ���״̬      ��ʱȥ����Ƿ���Ҫ���� ���������ϵ�ʱ����� �����ǵ���· ���в����ڳ�ͻ����
}

//����1��ʾ�������
u8 atk_8266_staclient_init(u8 mode)
{
		u8 res = 0;
		char *p;
	
		CPU_SR_ALLOC();
		OS_CRITICAL_ENTER();
	
		p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
		
		atk_8266_send_cmd("AT+CIPCLOSE","OK",200);  
		atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
	
		if(mode == 0)
		{
				//ipbuf��Զ�˷�������IP ��Ҫ����  ����ָ����������ַ  Ӧ�ô��������������ַ ������֮ǰ�����л�
				sprintf(p,"AT+CIPSTART=\"TCP\",\"%s\",%s","192.168.1.1","8086");//�˿ںŹ̶�
				res = atk_8266_send_cmd(p,"OK",200);         //���ӷ�����

		}
		else
		{
				//ipbuf��Զ�˷�������IP ��Ҫ����  ����ָ����������ַ  Ӧ�ô��������������ַ ������֮ǰ�����л�
				sprintf(p,"AT+CIPSTART=\"TCP\",\"%s\",%s","192.168.1.2","8086");//�˿ںŹ̶�
				res = atk_8266_send_cmd(p,"OK",200);         //���ӷ�����
		}
		
		if(!res)  
		{
				res = atk_8266_send_cmd("AT+CIPMODE=1","OK",200);   
		}
		
		myfree(SRAMIN,p);		//�ͷ��ڴ� 
		
		OS_CRITICAL_EXIT();
		
		return res;
}


























