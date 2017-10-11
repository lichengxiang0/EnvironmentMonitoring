#include "OLED_TXT.h"
#include "w25qxx.h"
#include "Oled.h"
#include "OLED_GUI.h"



_fontinfo ftinfo={0};	//字库信息结构体


static void OLED_GUI_Font(u8 x,u8 y,u8 *font,u8 mode,u8 play); //显示一个指定大小的汉字

void convertToLittleEndian(u32 *date)   //大小端转换函数    大端转换成小端   小端转换成大端
{
   *date = ((*date & 0xff000000) >> 24)\
         | ((*date & 0x00ff0000) >>  8)\
         | ((*date & 0x0000ff00) <<  8)\
         | ((*date & 0x000000ff) << 24);
}

u8 OLED_TXT_Init(void)
{
	SPI_Flash_Init();
  SPI_Flash_Read((u8 *)&ftinfo,FONTINFOADDR,sizeof(ftinfo));   //从字库起始地址 读出字库头  判断是否存在

//C51 为大端模式  低地址存高位  高地址寸低位   例如 0X1122  设内存地址 为0X01及0X02   则 地址0X01的值为0X11    地址0X02的值为0X22
//调用大小端转换函数     C51为大端 如果写字库单片机不为大端 则需要转换
 
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

//code_date 字符指针开始
//从字库中查找出字模
//code_date 字符串的开始地址,GBK码
//mat  数据存放地址 size*2 bytes大小	 
void Get_HzMat(u8 *code_date,u8 *mat)
{		    
	u8 qh,ql;
	u8 i;					  
	u32 foffset=0; 
	qh=*code_date;
	ql=*(++code_date);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字  这里除去GBK里无效组合
	{   		    
	    for(i=0;i<(16*2);i++)*mat++=0x00;//填充满格
	    return; //结束访问
	}          
	if(ql<0x7f)ql-=0x40;//注意!    第二字节    0x40 - 0x7E     0X80 - 0XFE
	else ql-=0x41;    //内码第二字节是两段   不是连贯  这里修正 使其连贯
	qh-=0x81;   //把第一字节修正从0开始    第一字节 0X81 - 0XFE
	
	foffset=((u32)qh*190+ql)*32;//得到字库中的字节偏移量  	

  SPI_Flash_Read(mat,(u32)foffset+ftinfo.f16addr,32);		
}  

//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字GBK码
//mode: 0反白显示   1正常显示
static void OLED_GUI_Font(u8 x,u8 y,u8 *font,u8 mode,u8 play)
{
	u8 i=0;
	u8 num=0,table=0;
	u8 buff[32]={0};

  Get_HzMat(font,buff);   //得到汉字字模

	if((x+16)>X_WIDTH)   //超过X边界
	{	
		return;
  }
	if((y+16)>Y_WIDTH)   //超过Y边界
	{
		return;
	}
	
	for(i = 0;i <16;i++)  //下面8个字节
	{
		table=buff[16+i];
		for(num=0;num<8;num++)
		{
      if(table&0x80)
			{
        OLED_DrawPoint(x+i,y+num,mode);   //y地址偏移从0到7  x地址偏移从0到15
      }
			else
			{
        OLED_DrawPoint(x+i,y+num,!mode);
      }
			table<<=1;
    }
	}

	for(i = 0;i <16;i++)  //上8个字节
	{
		table=buff[i];
		for(num=0;num<8;num++)
		{
      if(table&0x80)
			{
        OLED_DrawPoint(x+i,y+num+8,mode);   //y地址偏移从0到7  x地址偏移从0到15
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
	  OLED_SHOW_GRAM(x,x+16,y);  //显示
	}
}

//GUI显示字符串 包括汉字及ASC II
//mode 0反色显示    1正常显示
//play 0不刷新显存  1刷新显存   2 刷新全屏
void OLED_GUI_Str(u8 x,u8 y,u8 *p,u8 mode,u8 play)
{
  u8 x0=x;   //备份起始坐标
	u8 y0=y;   //备份起始坐标
	u8 HZ_flat=1;   //汉字标记   1表示汉字  0表示ASCII   1同时也表示刷新标记  
	u8 code_buf[2]={0};
	while(*p!='\0')
	{ 
 //判断-------------------------------
	 if(HZ_flat)
	 {
    if(*p<0x80)   //ASCII
		{
      HZ_flat=0;
    }
   }
   
	 if(HZ_flat)  //汉字显示
	 {
		 if((x+16)>X_WIDTH)   //换下一行
		 {
			y+=16;   
      x=x0;
     }
		 
     if((y+16)>Y_WIDTH)  //已越界  退出
			 return ;
		 
		 code_buf[0]=*p++;     //加载数据
		 code_buf[1]=*p++;
		 
		 OLED_GUI_Font(x,y,code_buf,mode,(play==1)?1:0);
		 
     x+=16;        //X坐标改变
		 HZ_flat=1;   //再次刷新判断 
   }
	 else           //字符
	 {	 
		 if((x+8)>X_WIDTH)   //换下一行
		 {
			y0+=16;   
      x=x0;
     }
		 
     if((y+16)>Y_WIDTH) //已越界  退出
			 return ;
		 
		 code_buf[0]=*p++;   //加载数据
		 code_buf[1]='\0';
		 
     OLED_ShowString(x,y,code_buf,16,mode,(play==1)?1:0);    //显示16
		 
     x+=8; 
     HZ_flat=1;   //再次刷新判断
   }
  }
	if(play==2)  //刷新全部
	{
	 OLED_Gram_Update();   //刷新全部
	}
}




