#include "fontupd.h"
#include "ff.h"	  
#include "w25qxx.h"   
#include "OLED_TXT.h"
#include "string.h"
#include "malloc.h"
#include "delay.h"
#include "OLED_GUI.h"
#include "exfuns.h"

const u8 * OLED_16_FONT_GBK = "0:/config/OLED.FON";      //�ֿ�·��
const u8 * OLED_UNIGBK_BIN = "0:/config/UNIGBK.BIN";     //CC936�ļ� ·��



//��ʾ��ǰ������½���
//x,y:����
//size:�����С
//fsize:�����ļ���С
//pos:��ǰ�ļ�ָ��λ��
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
		OLED_ShowNum(x,y,t,3,16,1);//��ʾ��ֵ
	}					    
} 
//����ĳһ��
//x,y:����
//size:�����С
//fxpath:·��
//fx:���µ����� 0,ungbk;1,gbk12; 2,gbk16
//����ֵ:0,�ɹ�;����,ʧ��.
u8 updata_fontx(u16 x,u16 y,u8 *fxpath,u8 fx)
{
	u32 flashaddr=0;								    
	u8 *tempbuf;  //SD�� ������
 	u8 res = 0 ;	
	u16 bread;
	u32 offx=0;  
	tempbuf=mymalloc(4096);				//����4096���ֽڿռ�
	if(tempbuf==NULL)
	{
		return 0XFF;   //�ڴ�����ʧ��    ֱ�ӷ��ش���
	}
 	res=f_open(ftemp,(const TCHAR*)fxpath,FA_READ);  //���ļ�
 	if(res)
	{
		return 0XFE;//���ļ�ʧ��
	}		
	
 	if(res==0)	 
	{
		switch(fx)
		{
			case 0:												//����UNIGBK.BIN
				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	//��Ϣͷ֮�󣬽���UNIGBKת�����
				ftinfo.ugbksize=ftemp->fsize;					//UNIGBK�ļ���С
				flashaddr=ftinfo.ugbkaddr;
			break;
			case 1:
				ftinfo.f12addr=ftinfo.ugbkaddr+ftinfo.ugbksize;	//UNIGBK֮�󣬽���GBK12�ֿ�
				ftinfo.gbk12size=ftemp->fsize;					//GBK12�ֿ��С
				flashaddr=ftinfo.f12addr;						//GBK12����ʼ��ַ
			break;
			case 2:		
				ftinfo.f16addr=ftinfo.f12addr+ftinfo.gbk12size;	//GBK12֮�󣬽���GBK16�ֿ�	
				ftinfo.gbk16size=ftemp->fsize;					//GBK16�ֿ��С		
				flashaddr=ftinfo.f16addr;						//GBK16����ʼ��ַ
		  break;
			case 3:
				ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;	//GBK16֮�󣬽���GBK24�ֿ�
				ftinfo.gkb24size=ftemp->fsize;					//GBK24�ֿ��С
				flashaddr=ftinfo.f24addr;						//GBK24����ʼ��ַ
			break;
		} 
			
		while(res==FR_OK)//��ѭ��ִ��
		{
	 		res=f_read(ftemp,tempbuf,4096,(UINT *)&bread);		//��ȡ����	 
			if(res!=FR_OK)
			{
				break;								//ִ�д���
			}
			SPI_Flash_Write(tempbuf,offx+flashaddr,4096);		//��0��ʼд��4096������  
	  	offx+=bread;	        //FLASH д��ַƫ��
			fupd_prog(x,y,ftemp->fsize,offx);	 			//������ʾ
			if(bread!=4096)
			{
				break;								//������.
			}
	 	} 	
		f_close(ftemp);		  //�ر��ļ�
	}			 
	myfree(tempbuf);	//�ͷ��ڴ�
	return res;
} 

//���������ļ�,UNIGBK,GBK12,GBK16,GBK24һ�����
//x,y:��ʾ��Ϣ����ʾ��ַ								  
//����ֵ:0,���³ɹ�;
//		 ����,�������.	  
u8 update_font(void)
{	
	u32 *buf;
	u8 res=0;		   
 	u16 i,j;
	res=0XFF;		
	ftinfo.fontok=0XFF;
	buf=mymalloc(4096);	//����4K�ֽ��ڴ�  
	if(buf==NULL)
	{
		myfree(buf);
		return 1;	//�ڴ�����ʧ��
	}
	//�Ȳ����ļ��Ƿ����� 
  res=f_open(ftemp,(const TCHAR*)OLED_UNIGBK_BIN,FA_READ);  //���ļ�
	if(res)
	{
	  return 2 ;
	}
	f_close(ftemp);			 //�ļ�������ر��ļ�
	
	res=f_open(ftemp,(const TCHAR*)OLED_16_FONT_GBK,FA_READ);  //���ļ�
	if(res)
	{
	  return 3;
	}
	f_close(ftemp);			 //�ļ�������ر��ļ�
	
	if(res==0)//�ֿ��ļ�������.
	{  
    OLED_BUF_Clear();   //����Դ�
		OLED_GUI_Str(8,32,"Erasing Sector",1,2);//��ʾ���ڲ�������	
		for(i=0;i<FONTSECSIZE;i++)	//�Ȳ����ֿ�����,���д���ٶ�
		{
			fupd_prog(48,16,FONTSECSIZE,i);//������ʾ
			SPI_Flash_Read((u8*)buf,((FONTINFOADDR/4096)+i)*4096,4096);//������������������
			for(j=0;j<1024;j++)//У������
			{
				if(buf[j]!=0XFFFFFFFF)break;//��Ҫ����  	  
			}
			if(j!=1024)
			{
				SPI_Flash_Erase_Sector((FONTINFOADDR/4096)+i);	//��Ҫ����������
			}
		}
		myfree(buf);  //�ͷ��ڴ�
		
		OLED_BUF_Clear();   //����Դ�
		OLED_GUI_Str(0,48,"Updating UNIGBK",1,2);		
		res=updata_fontx(32,32,(u8*)OLED_UNIGBK_BIN,0);	//����UNIGBK.BIN
		if(res)
    {
		  return 4;
		}
		
//		LCD_ShowString(x,y,240,320,size,"Updating GBK12.BIN  ");
//		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
//		strcat((char*)pname,(char*)GBK12_PATH); 
//		res=updata_fontx(x+20*size/2,y,size,pname,1);	//����GBK12.FON
//		if(res)
//    {
//		 myfree(SRAMIN,pname);
//		 return 2;
//		}
		OLED_BUF_Clear();   //����Դ�
		OLED_GUI_Str(0,16,"Updating GBK16 ",1,1);
		res=updata_fontx(32,0,(u8*)OLED_16_FONT_GBK,2);	//����GBK16.FON
		if(res)
    {
		  return 5;
		}
		
//		LCD_ShowString(x,y,240,320,size,"Updating GBK24.BIN  ");
//		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
//		strcat((char*)pname,(char*)GBK24_PATH); 
//		res=updata_fontx(x+20*size/2,y,size,pname,3);	//����GBK24.FON
//		if(res)
//    {
//		  myfree(SRAMIN,pname);
//		  return 4;
//		}
		//ȫ�����º���
		ftinfo.fontok=0XAA;
		SPI_Flash_Write((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));	//�����ֿ���Ϣ
	}
	myfree(buf);        //�ͷ��ڴ�
  OLED_BUF_Clear();   //����Դ� 
	OLED_GUI_Str(8,32,"�ֿ���³ɹ���",1,2);
	
  delay_ms(3000);
	return res;//�޴���.			 
} 
//��ʼ������
//����ֵ:0,�ֿ����.
//		 ����,�ֿⶪʧ
u8 font_init(void)
{		
	u8 t=0;
	while(t<10)//������ȡ10��,���Ǵ���,˵��ȷʵ��������,�ø����ֿ���
	{
		t++;
		OLED_TXT_Init();//����ftinfo�ṹ������
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





























