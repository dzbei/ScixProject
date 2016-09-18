#include "bmpdisplay.h"
#include "EmWinHZFont.h"
#include "GUI.h"
#include "malloc.h"
#include "delay.h"
#include "ff.h"
#include "tftlcd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//STemwin BMPͼƬ��ʾ 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/3/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

static FIL BMPFile;
static FIL ScrSortFile;                 //��Ļ��ͼ�ļ�	  
static char bmpBuffer[BMPPERLINESIZE];
/*******************************************************************
*
*       Static functions
*
********************************************************************
*/
/*********************************************************************
*
*       BmpGetData
*
* Function description
*   This routine is called by GUI_BMP_DrawEx(). The routine is responsible
*   for setting the data pointer to a valid data location with at least
*   one valid byte.
*
* Parameters:
*   p           - Pointer to application defined data.
*   NumBytesReq - Number of bytes requested.
*   ppData      - Pointer to data pointer. This pointer should be set to
*                 a valid location.
*   StartOfFile - If this flag is 1, the data pointer should be set to the
*                 beginning of the data stream.
*
* Return value:
*   Number of data bytes available.
*/
static int BmpGetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) 
{
	static int readaddress=0;
	FIL * phFile;
	UINT NumBytesRead;
	#if SYSTEM_SUPPORT_OS
		CPU_SR_ALLOC();
	#endif
	
	phFile = (FIL *)p;
	
	if (NumBytesReq > sizeof(bmpBuffer)) 
	{
		NumBytesReq = sizeof(bmpBuffer);
	}

	//�ƶ�ָ�뵽Ӧ�ö�ȡ��λ��
	if(Off == 1) readaddress = 0;
	else readaddress=Off;
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_ENTER();	//�ٽ���
	#endif
	f_lseek(phFile,readaddress); 
	
	//��ȡ���ݵ���������
	f_read(phFile,bmpBuffer,NumBytesReq,&NumBytesRead);
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_EXIT();	//�˳��ٽ��� 
	#endif
	*ppData = (U8 *)bmpBuffer;
	return NumBytesRead;//���ض�ȡ�����ֽ���
}

//��ָ��λ����ʾ���ص�RAM�е�BMPͼƬ
//BMPFileName:ͼƬ��SD�����������洢�豸�е�·��(���ļ�ϵͳ֧�֣�)
//mode:��ʾģʽ
//		0 ��ָ��λ����ʾ���ɲ���x,yȷ����ʾλ��
//		1 ��LCD�м���ʾͼƬ����ѡ���ģʽ��ʱ�����x,y��Ч��
//x:ͼƬ���Ͻ���LCD�е�x��λ��(������modeΪ1ʱ���˲�����Ч)
//y:ͼƬ���Ͻ���LCD�е�y��λ��(������modeΪ1ʱ���˲�����Ч)
//member:  ���ű����ķ�����
//denom:���ű����ķ�ĸ��
//����ֵ:0 ��ʾ����,���� ʧ��
int dispbmp(u8 *BMPFileName,u8 mode,u32 x,u32 y,int member,int denom)
{
	u16 bread;
	char *bmpbuffer;
	char result;
	int XSize,YSize;
	float Xflag,Yflag;
	
	#if SYSTEM_SUPPORT_OS
		CPU_SR_ALLOC();
	#endif

	result = f_open(&BMPFile,(const TCHAR*)BMPFileName,FA_READ);	//���ļ�
	//�ļ��򿪴�������ļ�����BMPMEMORYSIZE
	if((result != FR_OK) || (BMPFile.obj.objsize>BMPMEMORYSIZE)) 	return 1;

	bmpbuffer = mymalloc(SRAMEX,BMPFile.obj.objsize);//�����ڴ�
	if(bmpbuffer == NULL) return 2;
	
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_ENTER();	//�ٽ���
	#endif
		
	result = f_read(&BMPFile,bmpbuffer,BMPFile.obj.objsize,(UINT *)&bread); //��ȡ����
	if(result != FR_OK) return 3;
	
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_EXIT();//�˳��ٽ���
	#endif
	
	switch(mode)
	{
		case 0:	//��ָ��λ����ʾͼƬ
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				GUI_BMP_Draw(bmpbuffer,x,y);	//��ָ��λ����ʾBMPͼƬ
			}else //����ͼƬ��Ҫ����
			{
				GUI_BMP_DrawScaled(bmpbuffer,x,y,member,denom);
			}
			break;
		case 1:	//��LCD�м���ʾͼƬ
			XSize = GUI_BMP_GetXSize(bmpbuffer);	//��ȡͼƬ��X���С
			YSize = GUI_BMP_GetYSize(bmpbuffer);	//��ȡͼƬ��Y���С
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				//��LCD�м���ʾͼƬ
				GUI_BMP_Draw(bmpbuffer,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			}else //����ͼƬ��Ҫ����
			{
				Xflag = (float)XSize*((float)member/(float)denom);
				Yflag = (float)YSize*((float)member/(float)denom);
				XSize = (lcddev.width-(int)Xflag)/2-1;
				YSize = (lcddev.height-(int)Yflag)/2-1;
				GUI_BMP_DrawScaled(bmpbuffer,XSize,YSize,member,denom);
			}
			break;
	}
	f_close(&BMPFile);				//�ر�BMPFile�ļ�
	myfree(SRAMEX,bmpbuffer);		//�ͷ��ڴ�
	return 0;
}

//��ָ��λ����ʾ������ص�RAM�е�BMPͼƬ(���ļ�ϵͳ֧�֣�����СRAM���Ƽ�ʹ�ô˷�����)
//BMPFileName:ͼƬ��SD�����������洢�豸�е�·��
//mode:��ʾģʽ
//		0 ��ָ��λ����ʾ���в���x,yȷ����ʾλ��
//		1 ��LCD�м���ʾͼƬ����ѡ���ģʽ��ʱ�����x,y��Ч��
//x:ͼƬ���Ͻ���LCD�е�x��λ��(������modeΪ1ʱ���˲�����Ч)
//y:ͼƬ���Ͻ���LCD�е�y��λ��(������modeΪ1ʱ���˲�����Ч)
//member:  ���ű����ķ�����
//denom:���ű����ķ�ĸ��
//����ֵ:0 ��ʾ����,���� ʧ��
int dispbmpex(u8 *BMPFileName,u8 mode,u32 x,u32 y,int member,int denom)
{
	char result;
	int XSize,YSize;
	float Xflag,Yflag;
	
	result = f_open(&BMPFile,(const TCHAR*)BMPFileName,FA_READ);	//���ļ�
	//�ļ��򿪴���
	if(result != FR_OK) 	return 1;
		
	switch(mode)
	{
		case 0:	//��ָ��λ����ʾͼƬ
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				GUI_BMP_DrawEx(BmpGetData,&BMPFile,x,y);//��ָ��λ����ʾBMPͼƬ
			}else //����ͼƬ��Ҫ����
			{
				GUI_BMP_DrawScaledEx(BmpGetData,&BMPFile,x,y,member,denom);
			}
			break;
		case 1:	//��LCD�м���ʾͼƬ
			XSize = GUI_BMP_GetXSizeEx(BmpGetData,&BMPFile);	//��ȡͼƬ��X���С
			YSize = GUI_BMP_GetYSizeEx(BmpGetData,&BMPFile);	//��ȡͼƬ��Y���С
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				//��LCD�м���ʾͼƬ
				GUI_BMP_DrawEx(BmpGetData,&BMPFile,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			}else //����ͼƬ��Ҫ����
			{
				Xflag = (float)XSize*((float)member/(float)denom);
				Yflag = (float)YSize*((float)member/(float)denom);
				XSize = (lcddev.width-(int)Xflag)/2-1;
				YSize = (lcddev.height-(int)Yflag)/2-1;
				GUI_BMP_DrawScaledEx(BmpGetData,&BMPFile,XSize,YSize,member,denom);
			}
			break;
	}
	f_close(&BMPFile);		//�ر�BMPFile�ļ�
	return 0;
}

//�˺�����GUI_BMP_Serialize()���ã��������ļ�д���ֽ�
static void _WriteByte2File(U8 Data, void * p) 
{
	UINT nWritten;
	f_write((FIL*)p,&Data,1,&nWritten);
}

//�õ�һ���ļ���
//pname:�õ����ļ���
void emwinbmp_new_pathname(u8 *pname)
{
    u8 res;
    u16 index=0;		
    while(index<0XFFFF)
    {
        sprintf((char*)pname,"0:/SCREENSHORT/��Ļ��ͼ%05d.bmp",index);
        res=f_open(&ScrSortFile,(const TCHAR*)pname,FA_READ);//���Դ�����ļ�
        if(res==FR_NO_FILE)break;   //���ļ���������=����������Ҫ��
        else if(res==FR_OK) f_close(&ScrSortFile);//����򿪳ɹ����͹رյ�
        index++;
    }
}
//����һ��BMPͼƬ�����浽SD����,ʵ����Ļ��ͼ���ܣ�
//*filepath:�ļ�·��
//x0:����BMP�ļ���X��ʼλ��
//y0:����BMP�ļ���Y��ʼλ��
//Xsize:X�Ĵ�С
//Ysize:Y�Ĵ�С
void create_bmppicture(u8 *filename,int x0,int y0,int Xsize,int Ysize)
{
	static FIL hFile;
	//����һ���ļ���·��Ϊfilename,����ļ�ԭ���ʹ��ڵĻ��ᱻ�½����ļ����ǵ���
	f_open(&hFile,(const TCHAR*)filename,FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
	GUI_BMP_SerializeEx(_WriteByte2File,x0,y0,Xsize,Ysize,&hFile);
	f_close(&hFile);	//�ر��ļ�
}	

void bmpdisplay_demo(void)
{
	GUI_SetBkColor(GUI_BLUE);
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_FontHZ16);
	GUI_Clear();
	
	while(1)
	{
		GUI_DispStringHCenterAt("��ָ��λ����ʾһ�ż��ص�RAM�е�BMPͼƬ",400,5);
		dispbmp("0:/PICTURE/720.bmp",0,0,0,1,1);
		GUI_Delay(1000);
		GUI_Clear();
		dispbmp("0:/PICTURE/bmp/1.bmp",0,310,0,1,1);
		dispbmp("0:/PICTURE/bmp/2.bmp",0,310,100,1,1);
		dispbmp("0:/PICTURE/bmp/3.bmp",0,310,200,1,1);
		dispbmp("0:/PICTURE/bmp/4.bmp",0,310,300,1,1);
		dispbmp("0:/PICTURE/bmp/5.bmp",0,310,400,1,1);
		dispbmp("0:/PICTURE/bmp/6.bmp",0,310,500,1,1);
		GUI_Delay(1000);
		GUI_Clear();
		dispbmp("0:/PICTURE/9.bmp",0,310,10,1,1);
		GUI_Delay(1000);
		GUI_Clear();
		dispbmp("0:/PICTURE/1.bmp",0,310,10,1,1);
		dispbmp("0:/PICTURE/2.bmp",0,310,10,1,1);
		dispbmp("0:/PICTURE/3.bmp",0,310,10,1,1);
		dispbmp("0:/PICTURE/4.bmp",0,310,10,1,1);
		dispbmp("0:/PICTURE/5.bmp",0,310,10,1,1);
		dispbmp("0:/PICTURE/6.bmp",0,310,10,1,1);
		dispbmp("0:/PICTURE/7.bmp",0,310,10,1,1);
		dispbmp("0:/PICTURE/8.bmp",0,310,10,1,1);
		GUI_Delay(1000);
		GUI_Clear();
		dispbmp("0:/PICTURE/1.bmp",0,310,10,1,1);
		GUI_Delay(1000);
		GUI_Clear();
		/*
		GUI_DispStringHCenterAt("��ָ��λ����ʾһ�ż��ص�RAM�е�BMPͼƬ",400,5);
		dispbmp("0:/PICTURE/BMP/ALIENTEKLOGO.bmp",0,10,	78,	1,1);
		GUI_Delay(1000);
		GUI_Clear();
	
		GUI_DispStringHCenterAt("��LCD�м���ʾһ����С1/2�ļ��ص�RAM�е�BMPͼƬ",400,5);
		dispbmp("0:/PICTURE/BMP/ALIENTEKLOGO.bmp",1,0,	0,	1,2);
		GUI_Delay(1000);
		GUI_Clear();
	
		GUI_DispStringHCenterAt("��LCD�м���ʾһ������4/3���ļ��ص�RAM�е�BMPͼƬ",400,5);
		dispbmp("0:/PICTURE/BMP/ALIENTEKLOGO.bmp",1,0,	0,	4,3);
		GUI_Delay(1000);
		GUI_Clear();
		
		GUI_DispStringHCenterAt("��ָ��λ����ʾһ��������ص�BMPͼƬ",400,5);
		dispbmpex("0:/PICTURE/BMP/ALIENTEKLOGO.bmp",0,10,	78,	1,1);
		GUI_Delay(1000);
		GUI_Clear();
	
	
		GUI_DispStringHCenterAt("��LCD�м���ʾһ����С1/2��������ص�BMPͼƬ",400,5);
		dispbmpex("0:/PICTURE/BMP/ALIENTEKLOGO.bmp",1,0,	0,	1,2);
		GUI_Delay(1000);
		GUI_Clear();
	
		GUI_DispStringHCenterAt("��LCD�м���ʾһ������4/3����������ص�BMPͼƬ",400,5);
		dispbmpex("0:/PICTURE/BMP/ALIENTEKLOGO.bmp",1,0,	0,	4,3);
		GUI_Delay(1000);
		GUI_Clear();
		*/
	}
}