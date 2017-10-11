#include "OLED_TXT.h"
#include "w25qxx.h"
#include "Oled.h"
#include "OLED_GUI.h"



_fontinfo ftinfo={0};	//�ֿ���Ϣ�ṹ��


static void OLED_GUI_Font(u8 x,u8 y,u8 *font,u8 mode,u8 play); //��ʾһ��ָ����С�ĺ���

void convertToLittleEndian(u32 *date)   //��С��ת������    ���ת����С��   С��ת���ɴ��
{
   *date = ((*date & 0xff000000) >> 24)\
         | ((*date & 0x00ff0000) >>  8)\
         | ((*date & 0x0000ff00) <<  8)\
         | ((*date & 0x000000ff) << 24);
}

u8 OLED_TXT_Init(void)
{
	SPI_Flash_Init();
  SPI_Flash_Read((u8 *)&ftinfo,FONTINFOADDR,sizeof(ftinfo));   //���ֿ���ʼ��ַ �����ֿ�ͷ  �ж��Ƿ����

//C51 Ϊ���ģʽ  �͵�ַ���λ  �ߵ�ַ���λ   ���� 0X1122  ���ڴ��ַ Ϊ0X01��0X02   �� ��ַ0X01��ֵΪ0X11    ��ַ0X02��ֵΪ0X22
//���ô�С��ת������     C51Ϊ��� ���д�ֿⵥƬ����Ϊ��� ����Ҫת��
 
 #if Mode_Swapping_EN
	convertToLittleEndian(&ftinfo.ugbkaddr);
	convertToLittleEndian(&ftinfo.ugbksize);
	convertToLittleEndian(&ftinfo.f12addr);
	convertToLittleEndian(&ftinfo.gbk12size);
	convertToLittleEndian(&ftinfo.f16addr);
	convertToLittleEndian(&ftinfo.gkb16size);
#endif
	
//--------------------------------------------------------
	if(ftinfo.fontok==0xAA)   
	{
    return 0;
  }
	 return 1;
}

//code_date �ַ�ָ�뿪ʼ
//���ֿ��в��ҳ���ģ
//code_date �ַ����Ŀ�ʼ��ַ,GBK��
//mat  ���ݴ�ŵ�ַ size*2 bytes��С	 
void Get_HzMat(u8 *code_date,u8 *mat)
{		    
	u8 qh,ql;
	u8 i;					  
	u32 foffset=0; 
	qh=*code_date;
	ql=*(++code_date);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//�� ���ú���  �����ȥGBK����Ч���
	{   		    
	    for(i=0;i<(16*2);i++)*mat++=0x00;//�������
	    return; //��������
	}          
	if(ql<0x7f)ql-=0x40;//ע��!    �ڶ��ֽ�    0x40 - 0x7E     0X80 - 0XFE
	else ql-=0x41;    //����ڶ��ֽ�������   ��������  �������� ʹ������
	qh-=0x81;   //�ѵ�һ�ֽ�������0��ʼ    ��һ�ֽ� 0X81 - 0XFE
	
	foffset=((u32)qh*190+ql)*32;//�õ��ֿ��е��ֽ�ƫ����  	

  SPI_Flash_Read(mat,(u32)foffset+ftinfo.f16addr,32);		
}  

//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:����GBK��
//mode: 0������ʾ   1������ʾ
static void OLED_GUI_Font(u8 x,u8 y,u8 *font,u8 mode,u8 play)
{
	u8 i=0;
	u8 num=0,table=0;
	u8 buff[32]={0};

  Get_HzMat(font,buff);   //�õ�������ģ

	if((x+16)>X_WIDTH)   //����X�߽�
	{	
		return;
  }
	if((y+16)>Y_WIDTH)   //����Y�߽�
	{
		return;
	}
	
	for(i = 0;i <16;i++)  //����8���ֽ�
	{
		table=buff[16+i];
		for(num=0;num<8;num++)
		{
      if(table&0x80)
			{
        OLED_DrawPoint(x+i,y+num,mode);   //y��ַƫ�ƴ�0��7  x��ַƫ�ƴ�0��15
      }
			else
			{
        OLED_DrawPoint(x+i,y+num,!mode);
      }
			table<<=1;
    }
	}

	for(i = 0;i <16;i++)  //��8���ֽ�
	{
		table=buff[i];
		for(num=0;num<8;num++)
		{
      if(table&0x80)
			{
        OLED_DrawPoint(x+i,y+num+8,mode);   //y��ַƫ�ƴ�0��7  x��ַƫ�ƴ�0��15
      }
			else
			{
        OLED_DrawPoint(x+i,y+num+8,!mode);
      }
				table<<=1;
    }
	}
	
	if(play)
	{
	  OLED_SHOW_GRAM(x,x+16,y);  //��ʾ
	}
}

//GUI��ʾ�ַ��� �������ּ�ASC II
//mode 0��ɫ��ʾ    1������ʾ
//play 0��ˢ���Դ�  1ˢ���Դ�   2 ˢ��ȫ��
void OLED_GUI_Str(u8 x,u8 y,u8 *p,u8 mode,u8 play)
{
  u8 x0=x;   //������ʼ����
	u8 y0=y;   //������ʼ����
	u8 HZ_flat=1;   //���ֱ��   1��ʾ����  0��ʾASCII   1ͬʱҲ��ʾˢ�±��  
	u8 code_buf[2]={0};
	while(*p!='\0')
	{ 
 //�ж�-------------------------------
	 if(HZ_flat)
	 {
    if(*p<0x80)   //ASCII
		{
      HZ_flat=0;
    }
   }
   
	 if(HZ_flat)  //������ʾ
	 {
		 if((x+16)>X_WIDTH)   //����һ��
		 {
			y+=16;   
      x=x0;
     }
		 
     if((y+16)>Y_WIDTH)  //��Խ��  �˳�
			 return ;
		 
		 code_buf[0]=*p++;     //��������
		 code_buf[1]=*p++;
		 
		 OLED_GUI_Font(x,y,code_buf,mode,(play==1)?1:0);
		 
     x+=16;        //X����ı�
		 HZ_flat=1;   //�ٴ�ˢ���ж� 
   }
	 else           //�ַ�
	 {	 
		 if((x+8)>X_WIDTH)   //����һ��
		 {
			y0+=16;   
      x=x0;
     }
		 
     if((y+16)>Y_WIDTH) //��Խ��  �˳�
			 return ;
		 
		 code_buf[0]=*p++;   //��������
		 code_buf[1]='\0';
		 
     OLED_ShowString(x,y,code_buf,16,mode,(play==1)?1:0);    //��ʾ16
		 
     x+=8; 
     HZ_flat=1;   //�ٴ�ˢ���ж�
   }
  }
	if(play==2)  //ˢ��ȫ��
	{
	 OLED_Gram_Update();   //ˢ��ȫ��
	}
}




