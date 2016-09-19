#include "pure.h"

extern OS_TCB WifiStatusTaskTCB;
extern OS_TCB InteractiveTaskTCB;
extern OS_TCB OTATaskTCB;

//PURE����  ��������������һ������   ������������Ϊ��ʱ����  tcp client��ʽ����
//����wifistatus_task ����       �����Ϊ�������� ���ͻ����л�������ipʱ �ᵼ�¶Ͽ�����
void interactive_task(void *p_arg)
{
	OS_ERR err;
	u16 rlen=0;
	static u32 cnt = 0;
	static u8 first_init = 0;
	
	CPU_SR_ALLOC();
	
	//����Э���������          ��ʼ������ͨ��ģʽ 
	while(1)
	{				
			cnt++;//��ΪϵͳOTA������ʱ�� 24Сʱ  24*60*60*60*2s   ��500msΪ��ʱ��λ 
		
			if(cnt > 3*60*60*60*2) //3Сʱ
			{
					cnt = 0;
					first_init = 0;
					OS_TaskResume((OS_TCB*)&OTATaskTCB,&err);//����ota����
					OS_TaskSuspend((OS_TCB*)&InteractiveTaskTCB,&err);		//�����Լ�
			}
			else
			{
					if(atk_8266_staclient_init(1)&&(first_init==0))
					{
						//��������ʧ��
						OS_TaskResume((OS_TCB*)&WifiStatusTaskTCB,&err);//����wifi����
						OS_TaskSuspend((OS_TCB*)&InteractiveTaskTCB,&err);		//�����Լ�
					}
									
					//������ڽ��յ�����  
					if(USART3_RX_STA&0X8000)
					{
							OS_CRITICAL_ENTER();	//�����ٽ�ģʽ
							first_init = 1;
							rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
							USART3_RX_BUF[rlen]=0;		//��ӽ����� 
							printf("%s",USART3_RX_BUF);	//���͵�����   
							//��Ӵ�����
							USART3_RX_STA = 0;
							OS_CRITICAL_EXIT();//�˳��ٽ�״̬
					}
					else
					{
							//�������жϳ�ʱֵ
							//����ָ��ʱ��û�����ݽ�������Ϊ����
																
							first_init = 0;
							OS_TaskResume((OS_TCB*)&WifiStatusTaskTCB,&err);//����wifi����
							OS_TaskSuspend((OS_TCB*)&InteractiveTaskTCB,&err);		//�����Լ�
					}
	  	}
		
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
	}
}