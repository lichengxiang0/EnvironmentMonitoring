#ifndef __OLED_H_
#define __OLED_H_			  	 
#include "sys.h"
#include "stdlib.h"	    
#include "M_data.h"  
		    					

#define X_WIDTH      128
#define Y_WIDTH       64

//-----------------OLED端口定义----------------  
//使用4线串行接口时使用 
//#define OLED_CS   PCout(12)
//#define OLED_RS   PCout(11)
//#define OLED_SCLK PAout(15)
//#define OLED_SDIN PCout(10)
//#define OLED_RST  PAout(3)
		   
#define OLED_BUS_RCC       RCC_APB2Periph_GPIOA

#define OLED_CS_           GPIOA                    //CS
#define OLED_CS_PIN        GPIO_Pin_4

#define OLED_RS_           GPIOA                    //DC
#define OLED_RS_PIN        GPIO_Pin_6

#define OLED_SCLK_         GPIOA                   //DO
#define OLED_SCLK_PIN      GPIO_Pin_5

#define OLED_SDIN_         GPIOA                   //DI
#define OLED_SDIN_PIN      GPIO_Pin_7

#define OLED_RST_          GPIOA                  //RST
#define OLED_RST_PIN       GPIO_Pin_1


#define OLED_CS(x)         x? GPIO_SetBits(OLED_CS_,OLED_CS_PIN) : GPIO_ResetBits(OLED_CS_,OLED_CS_PIN)

#define OLED_RS(x)         x? GPIO_SetBits(OLED_RS_,OLED_RS_PIN) : GPIO_ResetBits(OLED_RS_,OLED_RS_PIN)

#define OLED_SCLK(x)       x? GPIO_SetBits(OLED_SCLK_,OLED_SCLK_PIN) : GPIO_ResetBits(OLED_SCLK_,OLED_SCLK_PIN)

#define OLED_SDIN(x)       x? GPIO_SetBits(OLED_SDIN_,OLED_SDIN_PIN) : GPIO_ResetBits(OLED_SDIN_,OLED_SDIN_PIN)

#define OLED_RST(x)        x? GPIO_SetBits(OLED_RST_,OLED_RST_PIN) : GPIO_ResetBits(OLED_RST_,OLED_RST_PIN)



#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);


void OLED_Init(void);
void OLED_Init_Only(void);   //仅仅只是初始化OLED

void OLED_Set_Pos(u8 x, u8 y) ;  // 0<=X<=7    0<=Y<=3   设置坐标
void OLED_DrawPoint(u8 x,u8 y,u8 t) ;  //大点  1亮  0灭  3反

void OLED_SHOW_GRAM(u8 x0,u8 x1,u8 y) ; //制定区域显示  0<=y<=63    0<=x<=127
	
void OLED_Clear(void) ;    //清屏 显示   清除显存  显示
void OLED_Gram_Inverse(void) ;    //显存反色  显示
void OLED_Gram_Update(void);  //显示
void OLED_BUF_Clear(void)  ; //清空显存


void OLED_Gram_BMP(u8 *bmp) ;  //图片数组写入显示并显示  取模看取模文件说明
	

#endif  
	 



