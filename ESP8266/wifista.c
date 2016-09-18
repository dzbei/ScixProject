#include "common.h"
#include "stdlib.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� WIFI STA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

char SSID[16];
char PSW[16];
u8 netpro=0;	//����ģʽ

//����ʼ��ʧ�� ��û���������ȵ� ��ô�ͷ���ʧ�� -1 Ȼ���л���wifiapģʽ
u8 atk_8266_wifista_init(void)
{
	u8 ipbuf[16]; 	//IP����
	u8 res = 0;
	char *p;
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP�������ر�(��APģʽ��Ч) 
	delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	
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
		
//��һ������ ��������������P�˽���
	//����Ϊ������ģʽ
//	atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
//	
//	//ipbuf��Զ�˷�������IP ��Ҫ����  ����ָ����������ַ  Ӧ�ô��������������ַ ������֮ǰ�����л�
//	sprintf(p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);//�˿ںŹ̶�
//	res = atk_8266_send_cmd(p,"OK",200);         //���ӷ�����
//	//AT+CIPCLOSE   �ر�����  �����Ҫ�������� ���ȹرյ�ǰ�ķ���������
//	/**
//	ʧ�ܵĻ�:
//		DNS Fail
//		ERROR
//	**/
//	if(!res)  
//	{
//		//����Ϊ͸��ģʽ    TCP
//		res = atk_8266_send_cmd("AT+CIPMODE=1","OK",200);   
//	}
	
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	
	return res;
	//������֮�� �����������ݽ���״̬      ��ʱȥ����Ƿ���Ҫ���� ���������ϵ�ʱ����� �����ǵ���· ���в����ڳ�ͻ����
}


























