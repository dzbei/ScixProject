#ifndef __SSD2828__H
#define __SSD2828__H
#include "sys.h"
#include "spi.h"

#define SSD2828_CSN    PGout(10)  //SPIÆ¬Ñ¡ÐÅºÅ	
#define SSD2828_CLK    PBout(13)  //OLED_SCLK
#define SSD2828_MISO   PBin(14)  
#define SSD2828_MOSI   PBout(15)  //OLED_SDIN

////for 720*720
//#define HSPW      2
//#define VSPW      2
//#define HBPD      58
//#define VBPD      10
//#define HFPD      60
//#define VFPD      12
#define HSPW      2
#define VSPW      2
#define HBPD      22
#define VBPD      10
#define HFPD      96
#define VFPD      12
#define HORIZONTAL 720
#define VERTICAL   720
////for 480*854

//#define HSPW      8
//#define VSPW      4
//#define HBPD      62
//#define VBPD      12
//#define HFPD      66
//#define VFPD      20
//#define HORIZONTAL 480
//#define VERTICAL   854

void SSD2828_Init(void);
#endif
