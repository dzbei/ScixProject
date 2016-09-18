#include "ssd2828.h"
#include "delay.h"

#define REGFLAG_DELAY             			0xFD
#define REGFLAG_END_OF_TABLE      		  0xFE   // END OF REGISTERS MARKER

struct setting_table {
    unsigned char count;
    unsigned char para_list[64];
};

//static struct setting_table lcm_480_854_initialization_setting[] = {
//	{5,	{0xFF, 0xFF, 0x98, 0x06, 0x04, 0x01}},// Change to Page 1  
//	{1,	{0x08,	0x10}},	// output SDA
//	{1,	{0x21,	0x01}},	// DE = 1 Active
//	{1,	{0x30,	0x01}},	// 480 X 854
//	{1,	{0x31,	0x00}},	// 2-dot Inversion
//	
//	{1,	{0x40,	0x10}},	// BT
//	{1,	{0x41,	0x44}},	// DVDDH DVDDL clamp 
//	{1,	{0x42,	0x02}},	// VGH/VGL 
//	{1,	{0x43,	0x89}},	// VGH_CLAMP 0FF ;
//	{1,	{0x44,	0x87}},	// VGL_CLAMP OFF ; 
//	
//	//{0x45,	1,	{0x1B}},	// VGL_REG  -11V 
//	//{0x46,	1,	{0x55}},	// AVDD AVEE CHARGE PUMPING FREQ;  del
//	//{0x47,	1,	{0x55}},	// VGH VGL CHARGE PUMPING FREQ; 	 del
//	{1,	{0x50,	0xA8}},	// VGMP
//	{1,	{0x51,	0xA8}},	// VGMN
//	
//	  {1,	{0x52,	0x00}},	// Flicker 
//	{1,	{0x53,	0x63}},	// Flicker  83 adj by peter wang, 0x7c
//	{1,	{0x57,	0x50}},	// SDTI
//	{1,	{0x60,	0x07}},	// SDTI
//	{1,	{0x61,	0x00}},	// CRTI
//	{1,	{0x62,	0x08}},	// EQTI
//	{1,	{0x63,	0x00}},	// PCTI

//	{1,	{0xA0,	0x00}},	// Gamma 255     
//	{1,	{0xA1,	0x17}},	// Gamma 251    
//	{1,	{0xA2,	0x1E}},	// Gamma 247   
//	{1,	{0xA3,	0x0C}},	// Gamma 239   
//	{1,	{0xA4,	0x05}},	// Gamma 231  
//	{1,	{0xA5,	0x0A}},	// Gamma 203  
//	{1,	{0xA6,	0x06}},	// Gamma 175  
//	{1,	{0xA7,	0x05}},	// Gamma 147   
//	{1,	{0xA8,	0x08}},	// Gamma 108  
//	{1,	{0xA9,	0x0C}},	// Gamma 80  
//	{1,	{0xAA,	0x11}},	// Gamma 52  
//	{1,	{0xAB,	0x09}},	// Gamma 24   
//	{1,	{0xAC,	0x0F}},	// Gamma 16    
//	{1,	{0xAD,	0x16}},	// Gamma 8  
//	{1,	{0xAE,	0x0E}},	// Gamma 4      
//	{1,	{0xAF,	0x00}},	// Gamma 0   
//   
//	{1,	{0xC0,	0x00}},	// Gamma 255    
//	{1,	{0xC1,	0x17}},	// Gamma 251   
//	{1,	{0xC2,	0x1E}},	// Gamma 247  
//	{1,	{0xC3,	0x0C}},	// Gamma 239  
//	{1,	{0xC4,	0x06}},	// Gamma 231 
//	{1,	{0xC5,	0x0B}},	// Gamma 203  
//	{1,	{0xC6,	0x07}},	// Gamma 175   
//	{1,	{0xC7,	0x04}},	// Gamma 147   
//	{1,	{0xC8,	0x07}},	// Gamma 108   
//	{1,	{0xC9,	0x0C}},	// Gamma 80   
//	{1,	{0xCA,	0x11}},	// Gamma 52   
//	{1,	{0xCB,	0x08}},	// Gamma 24    
//	{1,	{0xCC,	0x0E}},	// Gamma 16    
//	{1,	{0xCD,	0x16}},	// Gamma 8   
//	{1,	{0xCE,	0x0E}},	// Gamma 4    
//	{1,	{0xCF,	0x00}},	// Gamma 0    
//	            
//	{5,	{0xFF,	0xFF, 0x98, 0x06, 0x04, 0x06}},// Change to Page 6
//	{1,	{0x00,	0x21}},	//
//	{1,	{0x01,	0x0A}},	//
//	{1,	{0x02,	0x00}},	//
//	{1,	{0x03,	0x00}},	//
//	{1,	{0x04,	0x01}},	//
//	{1,	{0x05,	0x01}},	//
//	{1,	{0x06,	0x80}},	//
//	{1,	{0x07,	0x06}},	//
//	{1,	{0x08,	0x01}},	//
//	{1,	{0x09,	0x80}},	//
//	{1,	{0x0A,	0x00}},	//
//	{1,	{0x0B,	0x00}},	//
//	{1,	{0x0C,	0x0A}},	//
//	{1,	{0x0D,	0x0A}},	//
//	{1,	{0x0E,	0x00}},	//
//	{1,	{0x0F,	0x00}},	//
//	{1,	{0x10,	0xF0}},	//
//	{1,	{0x11,	0xF4}},	//
//	{1,	{0x12,	0x04}},	//
//	{1,	{0x13,	0x00}},	//
//	{1,	{0x14,	0x00}},	//
//	{1,	{0x15,	0xC0}},	//
//	{1,	{0x16,	0x08}},	//
//	{1,	{0x17,	0x00}},	//
//	{1,	{0x18,	0x00}},	//
//	{1,	{0x19,	0x00}},	//
//	{1,	{0x1A,	0x00}},	//
//	{1,	{0x1B,	0x00}},	//
//	{1,	{0x1C,	0x00}},	//
//	{1,	{0x1D,	0x00}},	//
//	{1,	{0x20,	0x01}},	//
//	{1,	{0x21,	0x23}},	//
//	{1,	{0x22,	0x45}},	//
//	{1,	{0x23,	0x67}},	//
//	{1,	{0x24,	0x01}},	//
//	{1,	{0x25,	0x23}},	//
//	{1,	{0x26,	0x45}},	//
//	{1,	{0x27,	0x67}},	//
//	{1,	{0x30,	0x01}},	//
//	{1,	{0x31,	0x11}},	//
//	{1,	{0x32,	0x00}},	//
//	{1,	{0x33,	0xEE}},	//
//	{1,	{0x34,	0xFF}},	//	
//	{1,	{0x35,	0xBB}},	//	
//	{1,	{0x36,	0xCA}},	//
//	{1,	{0x37,	0xDD}},	//
//	{1,	{0x38,	0xAC}},	//
//	{1,	{0x39,	0x76}},	//		
//	{1,	{0x3A,	0x67}},	//
//	{1,	{0x3B,	0x22}},	//		
//	{1,	{0x3C,	0x22}},	//
//	{1,	{0x3D,	0x22}},	//	
//	{1,	{0x3E,	0x22}},	//
//	{1,	{0x3F,	0x22}},	//
//	{1,	{0x40,	0x22}},	//
//	{1,	{0x52,	0x10}},	//
//	{1,	{0x53,	0x10}},	//VGLO tie to VGL;
//              
//	{5,	{0xFF,	0xFF, 0x98, 0x06, 0x04, 0x07}},// Change to Page 7
////	{0x18,	1,	{0x1D}},	// VREG1 VREG2 output
//	{1,	{0x17,	0x22}},	// VGL_REG ON
//	{1,	{0x02,	0x77}},	//  
//	{1,	{0xE1,	0x79}},	//
//              
//	{5,	{0xFF,	0xFF, 0x98, 0x06, 0x04, 0x00}},// Change to Page 0
//	{1,	{0x11,	0x00}},	// Sleep-Out
////	{0x35,	1,	{0x00}}
//	{REGFLAG_DELAY, {120}},
//	{1,	{0x29,	0x00}},	// Display on
//	{REGFLAG_DELAY,  {10}},
////	{0x35,	1,	{0x00}},
//	{REGFLAG_DELAY, {10}},
//	{REGFLAG_END_OF_TABLE,  {0x00}}
//};

static struct setting_table lcm_720_720_initialization_setting[] = {
	{1,	{0x01,	0x00}},//—” ±5ms
	{REGFLAG_DELAY, {5}},
	{1,	{0xb0,	0x00}},//—” ±5ms
	{REGFLAG_DELAY, {5}},
	{5,	{0xe4,	0x00, 0x00, 0x00, 0x00, 0x08}},
	{3,	{0xD0,	0x45, 0x45, 0x71}},	
	{1,	{0x6F,	0x01}},	
	{1,	{0x11,  0x00}},	 //sleep-out
	{REGFLAG_DELAY, {120}},
	{3,	{0xD0,	0x52, 0x4a, 0x71}},	
	{8,	{0xbe,	0xff, 0x0f, 0x00, 0x18, 0x04, 0x04, 0x00, 0x5d}},		
	{1,	{0xbb,	0x2f}},		
	{1,	{0xb0,	0x2f}},	
	{1,	{0x29, 	0x00}},	 // Display on
	{REGFLAG_DELAY, {120}},
	{1,	{0x2C, 	0x00}}, //ø…—°
	{REGFLAG_DELAY, {10}},
};

//static struct setting_table ssd2828_initialization_setting_before[] = {
//		{2,	{0xb7,	0x50, 0x00}},//…Ë÷√Œ™LP MODE”√”⁄…Ë÷√LCM
//		{2,	{0xb8,	0x00, 0x00}},
//		{2,	{0xb9,	0x00, 0x00}},
//		{2,	{0xba,	0x11, 0x42}},
//		{2,	{0xbb,	0x03, 0x00}},
//		{2,	{0xb9,	0x01, 0x00}},
//		{2,	{0xde,	0x01, 0x00}},
//		{2,	{0xc9,	0x02, 0x23}},
//};

//static struct setting_table ssd2828_initialization_setting_after[] = {
//		{2,	{0xb7,	0x50, 0x00}},
//		{2,	{0xb8,	0x00, 0x00}},
//		{2,	{0xb9,	0x00, 0x00}},
//		{2,	{0xba,	0x22, 0x82}},
//		{2,	{0xbb,	0x06, 0x00}},
//		{2,	{0xb9,	0x01, 0x00}},
//		{2,	{0xc9,	0x02, 0x23}},
//		{REGFLAG_DELAY, {120}},
//		
//		{2,	{0xca,	0x01, 0x23}},
//		{2,	{0xcb,	0x10, 0x05}},
//		{2,	{0xcc,	0x05, 0x10}},
//		{2,	{0xd0,	0x00, 0x00}},
//		{2,	{0xb1,	HSPW, VSPW}},
//		{2,	{0xb2,	HBPD, VBPD}},
//		{2,	{0xb3,	HFPD, VFPD}},
//		{2,	{0xb4,	0xe0, 0x01}},
//		{2,	{0xb5,	0x56, 0x03}},
//		{2,	{0xb6,	0x04, 0x20}},
//		{2,	{0xde,	0x01, 0x00}},
//		{2,	{0xb7,	0x4b, 0x02}},
//		{REGFLAG_DELAY, {120}},
//};

static void push_lcm_table(struct setting_table *table, unsigned int count)
{
	unsigned int i,j;

	for(i = 0; i < count; i++) {

			unsigned cnt;
			cnt = table[i].count;
			GP_COMMAD_PA(cnt+1);
			switch (cnt) {

					case REGFLAG_DELAY :
							delay_ms(table[i].para_list[0]);
							break;

					case REGFLAG_END_OF_TABLE :
							break;

					default:
						
					for(j=0; j<cnt+1; j++)
					{
							SPI_WriteData(table[i].para_list[j]);
					}			  
			}
	}
}

//static void push_ssd2828_table(struct setting_table *table, unsigned int count)
//{
//	unsigned int i;

//	for(i = 0; i < count; i++) {

//			unsigned cnt;
//			cnt = table[i].count;//ªÒ»°≤Œ ˝∏ˆ ˝
//		
//			switch (cnt) {

//					case REGFLAG_DELAY :
//							delay_ms(table[i].para_list[0]);
//							break;

//					case REGFLAG_END_OF_TABLE :
//							break;

//					default:
//					//SSD2828_Write_Buf(table[i].para_list[0], &table[i].para_list[1], cnt);
//					delay_ms(2);  
//			}
//	}
//}

void SSD2828_Init(void)
{		
	  GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE();			//ø™∆ÙGPIOG ±÷”
		__HAL_RCC_GPIOB_CLK_ENABLE();
    
		//SSD2828_CSN
    GPIO_Initure.Pin=GPIO_PIN_10; //PG10
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //Õ∆ÕÏ ‰≥ˆ
    GPIO_Initure.Pull=GPIO_PULLUP;          //…œ¿≠
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //∏ﬂÀŸ
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //≥ı ºªØ
	
		//PB13 SCK
	  GPIO_Initure.Pin=GPIO_PIN_13; //PB13
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //Õ∆ÕÏ ‰≥ˆ
    GPIO_Initure.Pull=GPIO_PULLUP;          //…œ¿≠
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //∏ﬂÀŸ
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //≥ı ºªØ
	
		//PB14 = SPI2_MISO 
		GPIO_Initure.Pin=GPIO_PIN_14; //PB14
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  
 //   GPIO_Initure.Pull=GPIO_PULLUP;          //…œ¿≠
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //∏ﬂÀŸ
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //≥ı ºªØ
		
		//PB15 = SPI2_MOSI
		GPIO_Initure.Pin=GPIO_PIN_15; //PB15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //Õ∆ÕÏ ‰≥ˆ
  //  GPIO_Initure.Pull=GPIO_PULLUP;          //…œ¿≠
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //∏ﬂÀŸ
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //≥ı ºªØ
		
		SSD2828_CSN=1;//SPI SSD2828 «∑Ò πƒ‹	
		//P142

		SPI_READ_ID();

  SPI_2825_WrReg(0xb1,(VSPW<<8)|HSPW);	//Vertical sync and horizontal sync active period 
	SPI_2825_WrReg(0xb2,(VBPD<<8)|HBPD);	//Vertical and horizontal back porch period  
	SPI_2825_WrReg(0xb3,(VFPD<<8)|HFPD);	//Vertical and horizontal front porch period 
	SPI_2825_WrReg(0xb4, HORIZONTAL);		//Horizontal active period 
	SPI_2825_WrReg(0xb5, VERTICAL);		//Vertical active period
	SPI_2825_WrReg(0xb6, 0x2000);				//Video mode and video pixel format 
  delay_us(100);
	//MIPI lane configuration
	//00 - 1 lane mode 
	//01 - 2 lane mode 
	//10 - 3 lane mode 
	//11 - 4 lane mode 
	SPI_2825_WrReg(0xde, 0x0001);				//MIPI lane select 
	SPI_2825_WrReg(0xd6, 0x0005);				//Color order and endianess
	SPI_2825_WrReg(0xb9, 0x0000);				//Disable PLL
	SPI_2825_WrReg(0xc4, 0x0001);				//BTA setting
	//CABC brightness setting 
	SPI_2825_WrReg(0xe9, 0xff2f);				//CABC control
	SPI_2825_WrReg(0xeb, 0x0100);				//CABC control  1010
	//Communicate with LCD driver through MIPI 
	SPI_2825_WrReg(0xb7, 0x0342);				//DCS mode 0342
//	SPI_2825_WrReg(0xb8, 0x0000);				//VC registe
//	SPI_2825_WrReg(0xbc, 0x0000);				//Packet size 
	SPI_2825_WrCmd(0x11);					//LCD driver exit sleep mode
	delay_us(100);	
	SPI_2825_WrCmd(0x29);					//Set LCD driver display on 
//	PLL configuration     0x8332=0x100 00011 0011 0010     24/3 * 50 = 400
	SPI_2825_WrReg(0xba, 0x8332);				//PLL setting,8028   8012
	SPI_2825_WrReg(0xbb, 0x0003);				//LP clock divider
	SPI_2825_WrReg(0xb9, 0x0001);				//PLL enable 
	SPI_2825_WrReg(0xb8, 0x0000);				//VC register 
	SPI_2825_WrReg(0xb7, 0x030B);				//Generic mode, HS video mode
////	
	delay_ms(100);
	
	push_lcm_table(lcm_720_720_initialization_setting, sizeof(lcm_720_720_initialization_setting) / sizeof(struct setting_table));

			//Cmd code 3: Access video mode 
	//RGB interface configuration 
  SPI_2825_WrReg(0xb1,(VSPW<<8)|HSPW);	//Vertical sync and horizontal sync active period 
	SPI_2825_WrReg(0xb2,(VBPD<<8)|HBPD);	//Vertical and horizontal back porch period  
	SPI_2825_WrReg(0xb3,(VFPD<<8)|HFPD);	//Vertical and horizontal front porch period 
	SPI_2825_WrReg(0xb4, HORIZONTAL);		//Horizontal active period 
	SPI_2825_WrReg(0xb5, VERTICAL);		//Vertical active period
	SPI_2825_WrReg(0xb6, 0x2000);				//Video mode and video pixel format 
	//MIPI lane configuration
	//00 - 1 lane mode 
	//01 - 2 lane mode 
	//10 - 3 lane mode 
	//11 - 4 lane mode
	SPI_2825_WrReg(0xde, 0x0001);				//MIPI lane select, 4chl
	SPI_2825_WrReg(0xd6, 0x0005);				//Color order and endianess
	SPI_2825_WrReg(0xb9, 0x0000);				//Disable PLL
	SPI_2825_WrReg(0xc4, 0x0001);				//BTA setting
	//CABC brightness setting 
	SPI_2825_WrReg(0xe9, 0xff2f);				//CABC control
	SPI_2825_WrReg(0xeb, 0x0100);				//CABC control
	delay_us(200);
	//PLL configuration
	//FR: bit15~14
	//00 ñ 62.5 < OUT f  < 125 
	//01 ñ 126 < OUT f  < 250 
	//10 ñ 251 < OUT f  < 500  
	//11 ñ 501 < OUT f  < 1000 
	
	//MS: bit12~8
	//Fpre = fin/MS
	
	//NS: bit7~0
	//Fout = Fpre*NS
	//0x8012 = 1000 0000 0001 00010 //24 / 1 * 18 = 432
	SPI_2825_WrReg(0xba, 0x8012);		//0x8012-PLL setting, select TX_CLK=24MHZ, (24/8)*250 = 750MHZ(8012:24*18=432M;866E:24/6*110=440M,8337:24/3*55=440) 844b
										//844b:24/4*75=450
										//8446:24/4*70=420
										//8332:24/3*50=400
										//8440:24/4*64=384
										//843c:24/4*60=360
	SPI_2825_WrReg(0xbb, 0x0006);				//LP clock divider,??f/(1+1),750MHZ/2 = //////24/6=4M
	SPI_2825_WrReg(0xb9, 0x0001);				//PLL enable //0xb9,0x0001
	SPI_2825_WrReg(0xb8, 0x0000);				//VC register 
	//SPI_2825_WrReg(0xb7, 0x030B | 0x0020);		//Generic mode, HS video mode
	SPI_2825_WrReg(0xb7, 0x024B);

/***************************************************************************************/
	delay_us(100);
	
			//// Reset LCD Driver////
	//∆¡ƒ¨»œ «”≤º˛∏¥Œª
//		LCD_RST_SET; //¿≠∏ﬂ
//		delay_ms(30); // Delay 1ms
//    LCD_RST_CLR; //¿≠µÕ
//    delay_ms(150); // Delay 10ms // This Delay time is necessary
//    LCD_RST_SET; //¿≠∏ﬂ
//		delay_ms(200); // Delay 50 ms
		
//		push_ssd2828_table(ssd2828_initialization_setting_before, sizeof(ssd2828_initialization_setting_before) / sizeof(struct setting_table));
//		
//		push_lcm_table(lcm_480_854_initialization_setting, sizeof(lcm_480_854_initialization_setting) / sizeof(struct setting_table));
//		
//		push_ssd2828_table(ssd2828_initialization_setting_after, sizeof(ssd2828_initialization_setting_after) / sizeof(struct setting_table));
}
