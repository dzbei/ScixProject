#include "common.h"

extern char SSID[];
extern char PSW[];

//��������wifi����       �ⲻ��Ҫ������
void atk_8266_wifiap_init(void)
{
		u8 *p;
		p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	
		memset(SSID,0,16);
		memset(PSW,0,16);

		atk_8266_send_cmd("AT+CWMODE=2","OK",50);		//����WIFI APģʽ
		atk_8266_send_cmd("AT+RST","OK",20);	
		delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
	
		//���ù��ⲿ���ӵ�SSID��PSW    1����ͨ����     4������ܷ�ʽΪWPA_WPA2_PSK
		sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",%d,%d","esp8266_test","12345678",1, 4);
		atk_8266_send_cmd(p,"OK",20);	
	
		//���������Ӻ����÷������˿ں�
		atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0�������ӣ�1��������
		sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
		atk_8266_send_cmd(p,"OK",20);     //����Serverģʽ���˿ں�Ϊ8086
		//�ȴ��ⲿ����
		
		myfree(SRAMIN,p);
		
		//������Serverģʽ�� ��ʼ����APK�˷�����������  �������ô�����ͨ   ����������
		//client�˿���ֱ�ӷ������ݹ��� �����յ�������    +IPD,0,8:12345678
		//client�� ��connect��ʱ�� ��ʾ������   0,CONNECT
		/*
		OK
		AT+CIPMUX=1
		OK
		AT+CIPSERVER=1,8086
		OK
		0,CONNECT
		+IPD,0,6:1234560,CLOSED
		*/
		//��ʱ������������״̬���
}		






