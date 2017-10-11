#include "Oled.h"
#include "delay.h"
#include "Spi1.h"

//OLED的显存   左下角为坐标原点  及坐标为 （0，0）  此坐标为OLED上显示坐标
//存放格式如下.              
//[0]0 1 2 3 ... 127	        -------------------
//[1]0 1 2 3 ... 127	        |                 |
//[2]0 1 2 3 ... 127	        |                 |
//[3]0 1 2 3 ... 127	        |                 |
//[4]0 1 2 3 ... 127	        |                 |
//[5]0 1 2 3 ... 127	        |                 |	
//[6]0 1 2 3 ... 127	        |O                |	
//[7]0 1 2 3 ... 127	   原点 --------------------		   
u8 OLED_GRAM[8][128];	 

//更新显存到LCD		 
void OLED_Gram_Update(void)
{
	u8 x,y;
	for(y=0;y<8;y++)
	{
		OLED_Set_Pos(0,y);
    for(x=0;x<128;x++)
	  {      
	   OLED_WR_Byte(OLED_GRAM[y][x],OLED_DATA);
	  }
	}
} 	  
			 
//清空显示缓存  
void OLED_BUF_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)
	 {
		 for(n=0;n<128;n++)
		 {
			OLED_GRAM[i][n]=0X00; 
     }
   }
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
  OLED_BUF_Clear();
	OLED_Gram_Update();//更新显示
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空		2 取反		   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>=X_WIDTH||y>=Y_WIDTH)return;//超出范围了.
	pos=7-y/8;   // 哪一行 0~7
	bx=y%8;      //一个行变量中 哪个bit
	temp=1<<(7-bx);
	switch(t)
	{
    case 0:
			     OLED_GRAM[pos][x]&=~temp;    //灭
		break;
		
    case 1:
			     OLED_GRAM[pos][x]|=temp;   //亮
		break;
		
    case 2:
			     OLED_GRAM[pos][x]^=temp;   //取反
		break;
		
		default:
		break;
  }    
}

void OLED_Gram_Inverse(void)    //显存反色  显示
{
  u8   i=0;
	u16   n=0;
	for(i=0;i<8;i++)
	{
    for(n=0;n<128;n++)
		{
      OLED_GRAM[i][n] ^= 0xff;
    }
  }
  OLED_Gram_Update();
}

void OLED_Gram_BMP(u8 *bmp)  //图片数组写入显示并显示
{
  u8   i=0;
	u16   n=0;
	for(i=0;i<8;i++)
	{
    for(n=0;n<128;n++)
		{
      OLED_GRAM[i][n] = *bmp++;   //图片数组数据 写入显示缓存
    }
  }
  OLED_Gram_Update();
}

//制定区域显示
void OLED_SHOW_GRAM(u8 x0,u8 x1,u8 y)   // 0<=y<=63    0<=x<=127
{
	u8 sx=x0;
	u8 y0=0;
  if((x1>X_WIDTH)||(x0>X_WIDTH)||(y>Y_WIDTH))
	{
	  return;
	}
	
  y0 =7-y/8;   //实际显示内容在OLED区域

 OLED_Set_Pos(sx,y0);  //设置坐标
  
 for(;sx<x1;sx++)
 {
	 OLED_WR_Byte(OLED_GRAM[y0][sx],OLED_DATA); 
 }
 
 y0--;
 sx=x0;
 OLED_Set_Pos(sx,y0);   //设置坐标
 for(;sx<x1;sx++)
 {
	 OLED_WR_Byte(OLED_GRAM[y0][sx],OLED_DATA); 
 } 
 
 if(y%16)   //如果不是在标准位置  则要刷新3行
 {
  y0--;
  sx=x0;
  OLED_Set_Pos(sx,y0);   //设置坐标
  for(;sx<x1;sx++)
  {
	 OLED_WR_Byte(OLED_GRAM[y0][sx],OLED_DATA); 
  } 
 }
}

/*********************OLED 设置坐标************************************/
void OLED_Set_Pos(u8 x, u8 y)   // 0<=X<=7    0<=Y<=3 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD);
} 


/**************************实现函数********************************************
*函数原型:	   	void OLED_WR_Byte(u8 dat,u8 cmd)
*功　　能:	   	向SSD1306写入一个字节。
*输入参数：  		dat:要写入的数据/命令；cmd:数据/命令标志 0,表示命令;1,表示数据;
*输出参数：  		无
*******************************************************************************/
void OLED_WR_Byte(u8 dat,u8 cmd)
{				  
	OLED_RS(cmd); //写命令 
	OLED_CS(0);		  
	SPI1_ReadWriteByte(dat);
	OLED_CS(1);		  
	OLED_RS(1);   	  
} 
 

/**************************实现函数********************************************
*函数原型:	   	void OLED_Display_On(void)
*功　　能:	   	开启OLED显示 
*输入参数：  		无
*输出参数：  		
*******************************************************************************/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

  
/**************************实现函数********************************************
*函数原型:	   	void OLED_Display_Off(void)
*功　　能:	   	关闭OLED显示 
*输入参数：  		无
*输出参数：  		无
*******************************************************************************/
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		 

/**************************实现函数********************************************
*函数原型:	   	void OLED_Init(void)
*功　　能:	   	初始化SSD1306	
*输入参数：  		无
*输出参数：  		无
*******************************************************************************/
void OLED_Init(void)
{ 	 				 	 					    
	GPIO_InitTypeDef  GPIO_InitStructure;
  	  
	RCC_APB2PeriphClockCmd(OLED_BUS_RCC, ENABLE );
		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =OLED_CS_PIN;            //CS
	GPIO_Init(OLED_CS_, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =OLED_RS_PIN;            //RS
	GPIO_Init(OLED_RS_, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin =OLED_SCLK_PIN;          //SCLK
//	GPIO_Init(OLED_SCLK_, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin =OLED_SDIN_PIN;          //SDIN
	GPIO_Init(OLED_SDIN_, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =OLED_RST_PIN;           //RST
	GPIO_Init(OLED_RST_, &GPIO_InitStructure);
		

  SPI1_Init();                                // SPI1初始化
	SPI1_SetSpeed(SPI_BaudRatePrescaler_2);     // 36 M
	
	OLED_RST(0);
	delay_ms(100);
	OLED_RST(1);	
 		  
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC8,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	 
	OLED_Set_Pos(0,0) ;
	OLED_Clear();
}  


void OLED_Init_Only(void)
{
	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC8,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	 
}


 























