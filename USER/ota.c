#include "ota.h"

extern OS_TCB OTATaskTCB;
extern OS_TCB InteractiveTaskTCB;

u8 g_ota_sucess = 0;//����������
u8 g_ota_start = 0;

const unsigned int ccitt_table[256] = 
				{ 0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280,
				0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481,
				0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81,
				0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880,
				0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81,
				0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80,
				0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680,
				0xD641, 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081,
				0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281,
				0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480,
				0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80,
				0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881,
				0x3840, 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80,
				0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81,
				0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681,
				0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080,
				0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281,
				0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480,
				0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80,
				0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881,
				0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80,
				0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81,
				0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681,
				0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080,
				0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280,
				0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481,
				0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81,
				0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880,
				0x9841, 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81,
				0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80,
				0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680,
				0x8641, 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081,
				0x4040, 
			};

static unsigned int cal_crc16(unsigned char *ptr, unsigned int len)
{
	unsigned int crc = 0xffff;
	
	while (len-- > 0)
	crc = ccitt_table[(crc ^ *ptr++) & 0xff] ^ (crc >> 8);
	return crc;
}

static char * itoa(int num, char * str, int radix)
{
	char temp;

	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned unum;
	int i = 0, j, k;

	if(radix == 10 && num < 0)
	{
		unum = (unsigned) - num;
		str[i++] = '-';
	}
	else	unum = (unsigned)num;

	do{
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;
	}while(unum);
	str[i] = '\0';

	if(str[0] == '-')
	{
		k = 1;
	}
	else
	{
		k=0;
	}
	
	for(j = k; j <= (i - 1) / 2; j++)
	{
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - 1 + k - j] = temp;
	}
	return str;
}

void ask_for_update(void)
{
	char buf[64];
	sprintf(buf, "{\"id\":\"%s\",\"type\":\"%s\"}", "A12345", "1");
	u3_printf(buf);
	printf("%s\r\n", buf);
}

//����Ӧ�� ͸���������Ƿ�ֻ������һ��
void update_reply(unsigned char c_num, unsigned char a_num, unsigned char flag)
{
	char s_c_num[3], s_a_num[3];
	unsigned char buf[64];

	itoa(c_num, s_c_num, 10);
	itoa(a_num, s_a_num, 10);
	
	sprintf(buf, "{\"id\":\"%s\",\"type\":\"%s\",\"c_num\":\"%s\",\"a_num\":\"%s\",\"result\":\"%s\"}#", "A12345", "2", s_c_num, s_a_num, flag?"ok":"fail");

	//͸����������
	u3_printf(buf);
	printf("%s\r\n", buf);
}

u8 ota_prase(u8 *buffer, u16 all_length)
{
	u8 cur_packet_num = 0;
	u8 all_packet_num = 0;
	u16 data_length = 0;
	u16 crc_code = 0, crc_code_temp = 0;

	//Э��ͷ 						0xEE 0xEE
	//����   						1�ֽ�
	//�ܰ��� 						1�ֽ�
	//������Ч���� 			2�ֽ�
	//crc16У��         2�ֽ�
	//������            0x55 0x55
	if(all_length<10) return 1;
	
	//�ɹ�����һ֡����
	if((buffer[0]==OTA_HEAD)&&(buffer[1]==OTA_HEAD))
	{
			g_ota_start = 1;
		
			cur_packet_num = buffer[2];
			all_packet_num = buffer[3];
			data_length = (buffer[4]<<8) | buffer[5];  //��λ��ǰ  дflash����ʱ���õ�
			crc_code = buffer[all_length - 3] | (buffer[all_length -4] << 8);
			crc_code_temp = cal_crc16(buffer, all_length - 4 );
		
			if(crc_code == crc_code_temp)
			{
					//������Ӧ�����ݵ�SD����
					
					//�����ǰ���ŵ��������ż��������
					if(cur_packet_num == all_packet_num)
					{
							g_ota_sucess = 1;
					}

					//�ظ�������
					update_reply(cur_packet_num, all_packet_num, 1);
					
					return 0;//�������ݳɹ�
			}
			else
			{
					//�ظ�������
					update_reply(cur_packet_num, all_packet_num, 0);
			}
	}
	//��������ʧ��
	return 1;
}

void ota_task(void *p_arg)
{
		OS_ERR err;
		u8 res;
		u16 rlen;
		static u8 first_init = 0;
		static u8 ota_try_cnt = 0;
		static u8 ota_fail_cnt = 0;
		
		CPU_SR_ALLOC();
	
		while(1)
		{			
			if(atk_8266_staclient_init(0)&&(first_init==0))
			{
				//��������ʧ�� ��������pure����
				OS_TaskResume((OS_TCB*)&InteractiveTaskTCB,&err);//����pure����
				OS_TaskSuspend((OS_TCB*)&OTATaskTCB,&err);		//�����Լ�
			}
			//������������ ���10û�л�Ӧ��ô���˳�����ģʽ
			else
			{
					first_init = 1;
				
					if(g_ota_start == 0)
					{
							//����������������     
							ask_for_update();
							//����10������������ɹ����˳�����ģʽ
							ota_try_cnt++;
							if(ota_try_cnt > 9)
							{
								ota_try_cnt = 0;
								first_init = 0;
								
								OS_TaskResume((OS_TCB*)&InteractiveTaskTCB,&err);//����pure����
								OS_TaskSuspend((OS_TCB*)&OTATaskTCB,&err);		//�����Լ�
							}
																											
							//һֱ�����������
							OSTimeDlyHMSM(0,0,0,5000,OS_OPT_TIME_PERIODIC,&err);//��ʱ5000ms
					}
					
					//������ڽ��յ�����  ����20sû�����ݽ��� ����Ϊ����Ͽ������������
					if(USART3_RX_STA&0X8000)
					{
								OS_CRITICAL_ENTER(); //�����ٽ�
								ota_try_cnt = 0;
								rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
								USART3_RX_BUF[rlen]=0;		//��ӽ����� 
								printf("%s",USART3_RX_BUF);	//���͵�����   

								//���������� ֱ�Ӹ���Э����бȶ�
								res = ota_prase(USART3_RX_BUF, rlen);
								//���ʧ�ܴ�������10�� 
								if(res)
								{
										ota_fail_cnt++;
										if(ota_fail_cnt > 9)
										{
												first_init = 0;
												//ɾ�������ļ�
												OS_TaskResume((OS_TCB*)&InteractiveTaskTCB,&err);//����pure����
												OS_TaskSuspend((OS_TCB*)&OTATaskTCB,&err);		//�����Լ�
										}
								}
								
								//����������
								if(g_ota_sucess)
								{
									//д��������ɱ�־
									
									//����ϵͳ

								}
								
								USART3_RX_STA = 0;
								
								OS_CRITICAL_EXIT();//�˳��ٽ�
						}
						else
						{
								//����Ԥ��ʱ��û�����ݽ��� ����Ϊ����
								//ɾ�������ļ�
							  									
								first_init = 0;
								OS_TaskResume((OS_TCB*)&InteractiveTaskTCB,&err);//����pure����
								OS_TaskSuspend((OS_TCB*)&OTATaskTCB,&err);		//�����Լ�
						}
			}
			
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
		}
}
