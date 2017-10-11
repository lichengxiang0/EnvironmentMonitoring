#include "SRAM_DATA.h"
#include "Malloc.h"    
#include "Led.h"
#include "sdio_sdcard.h"
#include "GPS_STA.h"
#include "OLED_GUI.h"
#include "OLED_BMP.h"
#include "OLED_TXT.h"
#include "Exit.h"
#include "ff.h"
#include "delay.h"



u8 *Folder_Name = 0;	                          //保存文件夹名    保存用户数据文件夹名字
u8 *Moving_Station_File_Name = 0;	              //保存文件名      GPS数据文件
u8 *Aerial_Point_File_Name = 0;	                //保存文件名      航拍点时间文件 
u8 *Icm20602_File_Name = 0;                      //保存文件名
u8 *laser_File_Name = 0;    

u8 *Write_Buff = 0;                           	//航拍点写书数据缓冲区
u8 *Write_Zt_Buff = 0;                           	//航拍点写书数据缓冲区
u8 *Write_Jg_Buff = 0;                           	//航拍点写书数据缓冲区

MY_GPS_TIME_typ  *GPS_TIME = 0   ;                 //GPS时间数据缓存
MY_UTC_TIME_typ  *UTC_TIME = 0   ;                 //UTC时间数据结构体

MY_FIFO_typ      *RX_BUF_FIFO = 0   ;              //串口接收缓存FIFO

MY_GPS_typ       *GPS_BUF  =  0     ;              //GPS时间数据解析缓存
MY_UTC_typ       *UTC_BUF  =  0     ;              //UTC时间数据解析缓存

Cam_FIFO_typ     *TIME_FIFO =  0    ;              //快门时间点 FIFO

u8  SD_Res_STA  =    0  ;                          //SD卡文件操作状态  
u8  RTC_ADJUS = 0       ;                          //校准指示变量

//双缓存变量------------------------------------------------------------------
u8  *RX_DMA_BUFF0  ;                               //双缓存
u8  *RX_DMA_BUFF1  ;  
u8   RX_DMA_STA = 0; 
//---------------------------------------------------------------------------

u8 SRAM_DATA_Init(void)  //用户数据指针申请内存  返回0 成功   1失败
{
 //指针缓存申请-----------------------------------------------------------------
 //航拍点数据缓冲区内存分配
//	Write_Buff = mymalloc(80);
//		Write_Zt_Buff = mymalloc(80);
//	  Write_Jg_Buff= mymalloc(80);
	//保存文件名      航拍点时间文件   申请缓存
//	Aerial_Point_File_Name = mymalloc(FIL_NAME_LEN);   
 //申请40个字节内存,类似"0:TEXT/TEXT20120321210633.txt"sprintf用到的缓冲区需要四个字节对齐
//	Moving_Station_File_Name = mymalloc(FIL_NAME_LEN);
	
//	Icm20602_File_Name = mymalloc(FIL_NAME_LEN);
	
	laser_File_Name = mymalloc(FIL_NAME_LEN);
	
 //GPS时间数据缓存内存申请----------------------------------------------------------------
//	GPS_TIME = mymalloc(sizeof(MY_GPS_TIME_typ));
	
 //FIFO申请缓存-------------------------------------------------------------------
//	 RX_BUF_FIFO = (MY_FIFO_typ *)mymalloc(sizeof(MY_FIFO_typ)) ;
	
 //GPS时间数据解析缓存申请缓存-------------------------------------------------------------- 
//	 GPS_BUF = (MY_GPS_typ *)mymalloc(sizeof(MY_GPS_typ));

 //保存文件夹名         保存用户数据文件夹名  申请缓存
	Folder_Name = mymalloc(FIL_NAME_LEN);

//	if((Write_Jg_Buff==0)||(Write_Zt_Buff==0)||(Write_Buff==0)||(Aerial_Point_File_Name==0)||(Moving_Station_File_Name==0)||(RX_BUF_FIFO==0)||(GPS_BUF==0)||(Folder_Name==0))   //只要有一个申请失败 就错误
//	{
//	   return 1;
//	}
	return 0;
}


u8 SYS_START_Init(void)   //系统开始运行时 内存申请  返回 0 成功  1失败
{
	//内存释放----------------------------------------------------------------
	 myfree(Folder_Name) ;  //释放保存文件夹名 保存用户数据文件夹明  释放缓存
//	 myfree(RX_BUF_FIFO) ;  //释放FIFO缓存
//	 myfree(GPS_BUF) ;      //释放GPS时间数据解析缓存
	//----------------------------------------------------------------------------
	
 //快门时间点 FIFO	 申请缓存-----------------------------------------------------
//	 TIME_FIFO = (Cam_FIFO_typ *)mymalloc(sizeof(Cam_FIFO_typ));

//DMA双缓存 申请内存-------------------------------------------
//   RX_DMA_BUFF0 = (u8 *)mymalloc(DMA_BUFF_SIZE) ;
//   RX_DMA_BUFF1 = (u8 *)mymalloc(DMA_BUFF_SIZE) ;  

//	if((TIME_FIFO==0)||(RX_DMA_BUFF0==0)||(RX_DMA_BUFF1==0))   //只要有一个申请失败 就错误
//	{
//	   return 1;
//	}
	return 0;
}



void SYS_Scan_TF_Err(void)   //TF 操作错误检测
{
 if((SD_Res_STA!=FR_OK)||(SD_SACN==1))  //有文件操作失败
 {
	LED0=1;
	LED1=0;
	Movie_Show_Img(SD_ERR);   //故障图标显示
	OLED_Gram_Update();    //显示	
	while(1)
	{
	 if(SD_SACN==0)   //SD卡插入
	 {
		 delay_ms(1500);
		 if(SD_SACN==0)
		 {
			Sys_Soft_Reset();  //系统软件复位		
		 }	
	 }
	  delay_ms(300);
	  OLED_Gram_Inverse();   //显存反色 显示	
	  LED0=!LED0;
	  LED1=!LED1;	
	 }
  }
}



























