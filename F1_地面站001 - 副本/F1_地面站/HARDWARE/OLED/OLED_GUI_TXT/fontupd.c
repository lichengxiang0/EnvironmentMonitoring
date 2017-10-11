#include "fontupd.h"
#include "ff.h"	  
#include "w25qxx.h"   
#include "OLED_TXT.h"
#include "string.h"
#include "malloc.h"
#include "delay.h"
#include "OLED_GUI.h"
#include "exfuns.h"

const u8 * OLED_16_FONT_GBK = "0:/config/OLED.FON";      //字库路径
const u8 * OLED_UNIGBK_BIN = "0:/config/UNIGBK.BIN";     //CC936文件 路径



//显示当前字体更新进度
//x,y:坐标
//size:字体大小
//fsize:整个文件大小
//pos:当前文件指针位置
void fupd_prog(u8 x,u8 y,u32 fsize,u32 pos)
{
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		OLED_ShowString(x+24,y,"%",16,1,1);		
		t=prog;
		if(t>100)t=100;
		OLED_ShowNum(x,y,t,3,16,1);//显示数值
	}					    
} 
//更新某一个
//x,y:坐标
//size:字体大小
//fxpath:路径
//fx:更新的内容 0,ungbk;1,gbk12; 2,gbk16
//返回值:0,成功;其他,失败.
u8 updata_fontx(u16 x,u16 y,u8 *fxpath,u8 fx)
{
	u32 flashaddr=0;								    
	u8 *tempbuf;  //SD卡 读缓存
 	u8 res = 0 ;	
	u16 bread;
	u32 offx=0;  
	tempbuf=mymalloc(4096);				//分配4096个字节空间
	if(tempbuf==NULL)
	{
		return 0XFF;   //内存申请失败    直接返回错误
	}
 	res=f_open(ftemp,(const TCHAR*)fxpath,FA_READ);  //打开文件
 	if(res)
	{
		return 0XFE;//打开文件失败
	}		
	
 	if(res==0)	 
	{
		switch(fx)
		{
			case 0:												//更新UNIGBK.BIN
				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	//信息头之后，紧跟UNIGBK转换码表
				ftinfo.ugbksize=ftemp->fsize;					//UNIGBK文件大小
				flashaddr=ftinfo.ugbkaddr;
			break;
			case 1:
				ftinfo.f12addr=ftinfo.ugbkaddr+ftinfo.ugbksize;	//UNIGBK之后，紧跟GBK12字库
				ftinfo.gbk12size=ftemp->fsize;					//GBK12字库大小
				flashaddr=ftinfo.f12addr;						//GBK12的起始地址
			break;
			case 2:		
				ftinfo.f16addr=ftinfo.f12addr+ftinfo.gbk12size;	//GBK12之后，紧跟GBK16字库	
				ftinfo.gbk16size=ftemp->fsize;					//GBK16字库大小		
				flashaddr=ftinfo.f16addr;						//GBK16的起始地址
		  break;
			case 3:
				ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;	//GBK16之后，紧跟GBK24字库
				ftinfo.gkb24size=ftemp->fsize;					//GBK24字库大小
				flashaddr=ftinfo.f24addr;						//GBK24的起始地址
			break;
		} 
			
		while(res==FR_OK)//死循环执行
		{
	 		res=f_read(ftemp,tempbuf,4096,(UINT *)&bread);		//读取数据	 
			if(res!=FR_OK)
			{
				break;								//执行错误
			}
			SPI_Flash_Write(tempbuf,offx+flashaddr,4096);		//从0开始写入4096个数据  
	  	offx+=bread;	        //FLASH 写地址偏移
			fupd_prog(x,y,ftemp->fsize,offx);	 			//进度显示
			if(bread!=4096)
			{
				break;								//读完了.
			}
	 	} 	
		f_close(ftemp);		  //关闭文件
	}			 
	myfree(tempbuf);	//释放内存
	return res;
} 

//更新字体文件,UNIGBK,GBK12,GBK16,GBK24一起更新
//x,y:提示信息的显示地址								  
//返回值:0,更新成功;
//		 其他,错误代码.	  
u8 update_font(void)
{	
	u32 *buf;
	u8 res=0;		   
 	u16 i,j;
	res=0XFF;		
	ftinfo.fontok=0XFF;
	buf=mymalloc(4096);	//申请4K字节内存  
	if(buf==NULL)
	{
		myfree(buf);
		return 1;	//内存申请失败
	}
	//先查找文件是否正常 
  res=f_open(ftemp,(const TCHAR*)OLED_UNIGBK_BIN,FA_READ);  //打开文件
	if(res)
	{
	  return 2 ;
	}
	f_close(ftemp);			 //文件存在则关闭文件
	
	res=f_open(ftemp,(const TCHAR*)OLED_16_FONT_GBK,FA_READ);  //打开文件
	if(res)
	{
	  return 3;
	}
	f_close(ftemp);			 //文件存在则关闭文件
	
	if(res==0)//字库文件都存在.
	{  
    OLED_BUF_Clear();   //清空显存
		OLED_GUI_Str(8,32,"Erasing Sector",1,2);//提示正在擦除扇区	
		for(i=0;i<FONTSECSIZE;i++)	//先擦除字库区域,提高写入速度
		{
			fupd_prog(48,16,FONTSECSIZE,i);//进度显示
			SPI_Flash_Read((u8*)buf,((FONTINFOADDR/4096)+i)*4096,4096);//读出整个扇区的内容
			for(j=0;j<1024;j++)//校验数据
			{
				if(buf[j]!=0XFFFFFFFF)break;//需要擦除  	  
			}
			if(j!=1024)
			{
				SPI_Flash_Erase_Sector((FONTINFOADDR/4096)+i);	//需要擦除的扇区
			}
		}
		myfree(buf);  //释放内存
		
		OLED_BUF_Clear();   //清空显存
		OLED_GUI_Str(0,48,"Updating UNIGBK",1,2);		
		res=updata_fontx(32,32,(u8*)OLED_UNIGBK_BIN,0);	//更新UNIGBK.BIN
		if(res)
    {
		  return 4;
		}
		
//		LCD_ShowString(x,y,240,320,size,"Updating GBK12.BIN  ");
//		strcpy((char*)pname,(char*)src);				//copy src内容到pname
//		strcat((char*)pname,(char*)GBK12_PATH); 
//		res=updata_fontx(x+20*size/2,y,size,pname,1);	//更新GBK12.FON
//		if(res)
//    {
//		 myfree(SRAMIN,pname);
//		 return 2;
//		}
		OLED_BUF_Clear();   //清空显存
		OLED_GUI_Str(0,16,"Updating GBK16 ",1,1);
		res=updata_fontx(32,0,(u8*)OLED_16_FONT_GBK,2);	//更新GBK16.FON
		if(res)
    {
		  return 5;
		}
		
//		LCD_ShowString(x,y,240,320,size,"Updating GBK24.BIN  ");
//		strcpy((char*)pname,(char*)src);				//copy src内容到pname
//		strcat((char*)pname,(char*)GBK24_PATH); 
//		res=updata_fontx(x+20*size/2,y,size,pname,3);	//更新GBK24.FON
//		if(res)
//    {
//		  myfree(SRAMIN,pname);
//		  return 4;
//		}
		//全部更新好了
		ftinfo.fontok=0XAA;
		SPI_Flash_Write((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));	//保存字库信息
	}
	myfree(buf);        //释放内存
  OLED_BUF_Clear();   //清空显存 
	OLED_GUI_Str(8,32,"字库更新成功！",1,2);
	
  delay_ms(3000);
	return res;//无错误.			 
} 
//初始化字体
//返回值:0,字库完好.
//		 其他,字库丢失
u8 font_init(void)
{		
	u8 t=0;
	while(t<10)//连续读取10次,都是错误,说明确实是有问题,得更新字库了
	{
		t++;
		OLED_TXT_Init();//读出ftinfo结构体数据
		if(ftinfo.fontok==0XAA)
		{
			break;
		}
		delay_ms(20);
	}
	if(ftinfo.fontok!=0XAA)
	{
		return 1;
	}
	return 0;		    
}





























