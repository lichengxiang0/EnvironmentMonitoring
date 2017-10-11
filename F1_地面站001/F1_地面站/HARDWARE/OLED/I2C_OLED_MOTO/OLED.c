#include "Oled.h"
#include "delay.h"
#include "Spi1.h"

//OLED���Դ�   ���½�Ϊ����ԭ��  ������Ϊ ��0��0��  ������ΪOLED����ʾ����
//��Ÿ�ʽ����.              
//[0]0 1 2 3 ... 127	        -------------------
//[1]0 1 2 3 ... 127	        |                 |
//[2]0 1 2 3 ... 127	        |                 |
//[3]0 1 2 3 ... 127	        |                 |
//[4]0 1 2 3 ... 127	        |                 |
//[5]0 1 2 3 ... 127	        |                 |	
//[6]0 1 2 3 ... 127	        |O                |	
//[7]0 1 2 3 ... 127	   ԭ�� --------------------		   
u8 OLED_GRAM[8][128];	 

//�����Դ浽LCD		 
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
			 
//�����ʾ����  
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
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
  OLED_BUF_Clear();
	OLED_Gram_Update();//������ʾ
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���		2 ȡ��		   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>=X_WIDTH||y>=Y_WIDTH)return;//������Χ��.
	pos=7-y/8;   // ��һ�� 0~7
	bx=y%8;      //һ���б����� �ĸ�bit
	temp=1<<(7-bx);
	switch(t)
	{
    case 0:
			     OLED_GRAM[pos][x]&=~temp;    //��
		break;
		
    case 1:
			     OLED_GRAM[pos][x]|=temp;   //��
		break;
		
    case 2:
			     OLED_GRAM[pos][x]^=temp;   //ȡ��
		break;
		
		default:
		break;
  }    
}

void OLED_Gram_Inverse(void)    //�Դ淴ɫ  ��ʾ
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

void OLED_Gram_BMP(u8 *bmp)  //ͼƬ����д����ʾ����ʾ
{
  u8   i=0;
	u16   n=0;
	for(i=0;i<8;i++)
	{
    for(n=0;n<128;n++)
		{
      OLED_GRAM[i][n] = *bmp++;   //ͼƬ�������� д����ʾ����
    }
  }
  OLED_Gram_Update();
}

//�ƶ�������ʾ
void OLED_SHOW_GRAM(u8 x0,u8 x1,u8 y)   // 0<=y<=63    0<=x<=127
{
	u8 sx=x0;
	u8 y0=0;
  if((x1>X_WIDTH)||(x0>X_WIDTH)||(y>Y_WIDTH))
	{
	  return;
	}
	
  y0 =7-y/8;   //ʵ����ʾ������OLED����

 OLED_Set_Pos(sx,y0);  //��������
  
 for(;sx<x1;sx++)
 {
	 OLED_WR_Byte(OLED_GRAM[y0][sx],OLED_DATA); 
 }
 
 y0--;
 sx=x0;
 OLED_Set_Pos(sx,y0);   //��������
 for(;sx<x1;sx++)
 {
	 OLED_WR_Byte(OLED_GRAM[y0][sx],OLED_DATA); 
 } 
 
 if(y%16)   //��������ڱ�׼λ��  ��Ҫˢ��3��
 {
  y0--;
  sx=x0;
  OLED_Set_Pos(sx,y0);   //��������
  for(;sx<x1;sx++)
  {
	 OLED_WR_Byte(OLED_GRAM[y0][sx],OLED_DATA); 
  } 
 }
}

/*********************OLED ��������************************************/
void OLED_Set_Pos(u8 x, u8 y)   // 0<=X<=7    0<=Y<=3 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD);
} 


/**************************ʵ�ֺ���********************************************
*����ԭ��:	   	void OLED_WR_Byte(u8 dat,u8 cmd)
*��������:	   	��SSD1306д��һ���ֽڡ�
*���������  		dat:Ҫд�������/���cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
*���������  		��
*******************************************************************************/
void OLED_WR_Byte(u8 dat,u8 cmd)
{				  
	OLED_RS(cmd); //д���� 
	OLED_CS(0);		  
	SPI1_ReadWriteByte(dat);
	OLED_CS(1);		  
	OLED_RS(1);   	  
} 
 

/**************************ʵ�ֺ���********************************************
*����ԭ��:	   	void OLED_Display_On(void)
*��������:	   	����OLED��ʾ 
*���������  		��
*���������  		
*******************************************************************************/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

  
/**************************ʵ�ֺ���********************************************
*����ԭ��:	   	void OLED_Display_Off(void)
*��������:	   	�ر�OLED��ʾ 
*���������  		��
*���������  		��
*******************************************************************************/
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		 

/**************************ʵ�ֺ���********************************************
*����ԭ��:	   	void OLED_Init(void)
*��������:	   	��ʼ��SSD1306	
*���������  		��
*���������  		��
*******************************************************************************/
void OLED_Init(void)
{ 	 				 	 					    
	GPIO_InitTypeDef  GPIO_InitStructure;
  	  
	RCC_APB2PeriphClockCmd(OLED_BUS_RCC, ENABLE );
		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =OLED_CS_PIN;            //CS
	GPIO_Init(OLED_CS_, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =OLED_RS_PIN;            //RS
	GPIO_Init(OLED_RS_, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin =OLED_SCLK_PIN;          //SCLK
//	GPIO_Init(OLED_SCLK_, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin =OLED_SDIN_PIN;          //SDIN
	GPIO_Init(OLED_SDIN_, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin =OLED_RST_PIN;           //RST
	GPIO_Init(OLED_RST_, &GPIO_InitStructure);
		

  SPI1_Init();                                // SPI1��ʼ��
	SPI1_SetSpeed(SPI_BaudRatePrescaler_2);     // 36 M
	
	OLED_RST(0);
	delay_ms(100);
	OLED_RST(1);	
 		  
	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
	OLED_WR_Byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC8,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	 
	OLED_Set_Pos(0,0) ;
	OLED_Clear();
}  


void OLED_Init_Only(void)
{
	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
	OLED_WR_Byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC8,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	 
}


 























