#ifndef _PNGDISPLAY_H
#define _PNGDISPLAY_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//STemwin PNGͼƬ��ʾ 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/3/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////////	

//ʹ��GUI_PNG_Draw()��������BMPͼƬ�Ļ�
//ͼƬ�Ǽ��ص�RAM�еģ���˲��ܴ���PNGMEMORYSIZE
//ע�⣺��ʾPNGͼƬʱ�ڴ�����ʹ�õ�EMWIN���ڴ����뺯�������
//PNGMEMORYSIZE���ܴ������Ǹ�EMWIN������ڴ�ش�С
#define PNGMEMORYSIZE	(8*1024*1024)	//ͼƬ��С������8M

//����������ص�RAM�е�PNGͼƬʱ��ͼƬÿ�е��ֽ���
#define PNGPERLINESIZE	5*1024	

int displaypng(char *PNGFileName,u8 mode,u32 x,u32 y);
int displaypngex(char *PNGFileName,u8 mode,u32 x,u32 y);
void pngdisplay_demo(void);
#endif