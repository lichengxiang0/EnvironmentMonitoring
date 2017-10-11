#include "PLAY_DATA.H"
#include "Oled.h"
#include "stdio.h"
#include "OLED_GUI.h"
#include "Cam_FIFO.h"
#include "SRAM_DATA.h"
#include "exfuns.h"
#include "Exit.h"
#include "Led.h"
#include <string.h>
#include "OLED_GUI.h"

u32 Write_Counter = 0;	//OLED打印GPS数据写入SD卡次数
u32 Write_Counter_CAMM = 0;  //拍摄照片数
u16 Config_Counter = 0; //RTC校准次数

u16 RTC_Config_ERR=0;   //RTC校准错误计数  最高位校准错误标记  其它校准错误计数

u8   SYS_TIMER_STA = 0 ;   //主循环任务单次重新刷新标记

static u16 SYS_TIMER = 0;     //主循环时间基   

#define  SYS_T        15     //执行间隙

#define  SYS_OS_NUM   6      //任务数

#define SYS_TASK(X)  (X*SYS_T)   //任务执行条件

PLAY_BF  PLAY_DATE={      //显示对象结构体   开始时全设为不可能值 强制刷新一次
        0xffffffff,0xfffff,0xffff,0xffff,
	      0xff,0xff,0xff,0xffff,0xffff,0xff,0xff,0xff      //时间数据
};     


#define TIMER_PLAY_F          24    //日期显示 X坐标相对左侧的偏移

#define TIMER_PLAY_FF         32   //时间数据显示 X坐标相对左侧的偏移


void PLAY_DATE_TASK(void)      //显示需显示数据   有变化才刷新
{
//	if(SYS_TIMER_FLAT)         //重新开始本次周期循环计数
//	{
//	   SYS_TIMER = (SYS_TIMER/SYS_T) * SYS_T;
//	   CLR_SYS_TIMER;
//	}
	
  SYS_TIMER++;       //循环计数          
 switch(SYS_TIMER)
 {
	case SYS_TASK(0):
	{
//日期显示--------------------------------------------------------------
	if(PLAY_DATE.calendar_bf.w_year!=calendar.w_year)   //年
	{
	   PLAY_DATE.calendar_bf.w_year = calendar.w_year;     //备份
		 OLED_ShowString(TIMER_PLAY_F,48,(const u8 *)"    -  -  ",16,1,1);  //先填好 -
		 OLED_ShowString(TIMER_PLAY_FF,32,(const u8 *)"  :  :  ",16,1,1);  //先填好 :
		
		 OLED_ShowNum(TIMER_PLAY_F,48,calendar.w_year,4,16,1);  //写入
	}	
	
	if(calendar.w_month!=PLAY_DATE.calendar_bf.w_month)   //月
	{
	   PLAY_DATE.calendar_bf.w_month = calendar.w_month;   //备份
		 OLED_ShowNum(TIMER_PLAY_F+40,48,calendar.w_month,2,16,1);  //写入    加8 是 - 字符 
	}		
	
	if(calendar.w_date!=PLAY_DATE.calendar_bf.w_date)   //日
	{
	   PLAY_DATE.calendar_bf.w_date = calendar.w_date;   //备份 
		 OLED_ShowNum(TIMER_PLAY_F+64,48,calendar.w_date,2,16,1);  //写入   加8 是 - 字符	
	}			
//时间显示--------------------------------------------------------------------
	if(calendar.hour!=PLAY_DATE.calendar_bf.hour)   //小时
	{
	   PLAY_DATE.calendar_bf.hour = calendar.hour;   //备份
		 OLED_ShowNum(TIMER_PLAY_FF,32,calendar.hour,2,16,1);  //写入
	}	
	
  if(calendar.min!=PLAY_DATE.calendar_bf.min)   //分
	{
	   PLAY_DATE.calendar_bf.min = calendar.min;   //备份
		 OLED_ShowNum(TIMER_PLAY_FF+24,32,calendar.min,2,16,1);  //写入  加8 是 - 字符
	}	
	
  if(calendar.sec!=PLAY_DATE.calendar_bf.sec)   //秒
	{
	   PLAY_DATE.calendar_bf.sec = calendar.sec;   //备份
		 OLED_ShowNum(TIMER_PLAY_FF+48,32,calendar.sec,2,16,1);  //写入  加8 是 - 字符 
	}
 }
 break;
 
 case SYS_TASK(1):
 {
//快门计数显示-----------------------------------------------------------------------
	if(Write_Counter_CAMM<=9999)
	{
//	  if(PLAY_DATE.Write_Counter_CAMM!=Write_Counter_CAMM)   //拍照数
//	  {
//	   PLAY_DATE.Write_Counter_CAMM = Write_Counter_CAMM;   //备份
//		 OLED_ShowNum(32,16,Write_Counter_CAMM,4,16,1);  //写入  	 
//	  }	
	}		
 }	
 break;
 
 case SYS_TASK(2):
 {
//正确校准计数显示--------------------------------------------------------------------
	if(Config_Counter<=9999)
	{
	  if(PLAY_DATE.Config_Counter!=Config_Counter)   //RTC正确校准数
	  {
	   PLAY_DATE.Config_Counter = Config_Counter;   //备份
		 OLED_ShowNum(96,16,Config_Counter,4,16,1);  //写入   
	  }		
	}
 }
 break; 

 case SYS_TASK(3):
 {	 
//GPS写入次数--------------------------------------------------
	if(Write_Counter<=99999999)
	{ 
	 if(PLAY_DATE.Write_Counter!=Write_Counter)   //GPS写入数
	 {
	  PLAY_DATE.Write_Counter = Write_Counter;   //备份
		OLED_ShowNum(64,0,Write_Counter,8,16,1);  //写入  
	 }	
  }
 }
 break;
 
 case SYS_TASK(4):
 {
	 OLED_Init_Only();     //仅仅重新配置一次	
	 OLED_Gram_Update();   //显示现存
 }
 break;
 
 case SYS_TASK(5):
 {
//	//快门文件数据写入-------------------------------------------------------------------------------------------		
//	if(Cam_FIFO_BUF_Valid_NUM(TIME_FIFO)>0) //如果快门时间记录 FIFO 有数据  则写入SD卡
//	{
//		Cam_FIFO_READ(TIME_FIFO,&Cam_WRITE_time) ;    //获取FIFO内时间数据
//		sprintf((char *)Write_Buff,"%04d-%02d-%02dT%02d:%02d:%02d:%06d\r\n",Cam_WRITE_time.w_year,Cam_WRITE_time.w_month,Cam_WRITE_time.w_date,Cam_WRITE_time.hour,Cam_WRITE_time.min,Cam_WRITE_time.sec,Cam_WRITE_time.micro_sec);
//	  SD_Res_STA=f_open(ftemp, (const TCHAR*)Aerial_Point_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//打开文件  		
//	  SD_Res_STA=f_lseek(ftemp,ftemp ->fsize); //移动指针到结尾
//		SD_Res_STA = f_write(ftemp,Write_Buff,strlen((char *)Write_Buff),&br);			 //写入文件
//	  SD_Res_STA=f_close(ftemp);      //关闭打开的文件
//	  Write_Counter_CAMM++;   //拍摄照片数加一

//		#if (Cal_LED_task_EN == 0)    //如果关闭指示灯任务
//		  		LED0_OFF;	//关闭指示灯
//    #endif
//   }
 }
 break;
 
 }
 
 if(SYS_TIMER>(SYS_T*(SYS_OS_NUM+1)))   //如果达到任务时间基最大值
 {
   SYS_TIMER = 0;
	 SYS_TIMER--;
 }
}

void PLAY_DATE_Init(void)      //显示需显示数据   有变化才刷新  初始化
{
	 Movie_Show_Img(PLAY_INTT) ;   //初始化显示界面
	 OLED_ShowString(0,0,"GPS",16,1,1);
//日期显示--------------------------------------------------------------
	if(PLAY_DATE.calendar_bf.w_year!=calendar.w_year)   //年
	{
	   PLAY_DATE.calendar_bf.w_year = calendar.w_year;     //备份
		 OLED_ShowString(TIMER_PLAY_F,48,(const u8 *)"    -  -  ",16,1,1);  //先填好 -
		 OLED_ShowString(TIMER_PLAY_FF,32,(const u8 *)"  :  :  ",16,1,1);  //先填好 :
		
		 OLED_ShowNum(TIMER_PLAY_F,48,calendar.w_year,4,16,1);  //写入
	}	
	
	if(calendar.w_month!=PLAY_DATE.calendar_bf.w_month)   //月
	{
	   PLAY_DATE.calendar_bf.w_month = calendar.w_month;   //备份
		 OLED_ShowNum(TIMER_PLAY_F+40,48,calendar.w_month,2,16,1);  //写入    加8 是 - 字符 
	}		
	
	if(calendar.w_date!=PLAY_DATE.calendar_bf.w_date)   //日
	{
	   PLAY_DATE.calendar_bf.w_date = calendar.w_date;   //备份 
		 OLED_ShowNum(TIMER_PLAY_F+64,48,calendar.w_date,2,16,1);  //写入   加8 是 - 字符
	}			
//时间显示--------------------------------------------------------------------
	if(calendar.hour!=PLAY_DATE.calendar_bf.hour)   //小时
	{
	   PLAY_DATE.calendar_bf.hour = calendar.hour;   //备份
		 OLED_ShowNum(TIMER_PLAY_FF,32,calendar.hour,2,16,1);  //写入
	}	
	
  if(calendar.min!=PLAY_DATE.calendar_bf.min)   //分
	{
	   PLAY_DATE.calendar_bf.min = calendar.min;   //备份
		 OLED_ShowNum(TIMER_PLAY_FF+24,32,calendar.min,2,16,1);  //写入  加8 是 - 字符
	}	
	
  if(calendar.sec!=PLAY_DATE.calendar_bf.sec)   //秒
	{
	   PLAY_DATE.calendar_bf.sec = calendar.sec;   //备份
		 OLED_ShowNum(TIMER_PLAY_FF+48,32,calendar.sec,2,16,1);  //写入  加8 是 - 字符
	}

//计数显示-----------------------------------------------------------------------
//	if(PLAY_DATE.Write_Counter_CAMM!=Write_Counter_CAMM)   //拍照数
//	{
//	   PLAY_DATE.Write_Counter_CAMM = Write_Counter_CAMM;   //备份
//		 OLED_ShowNum(32,16,Write_Counter_CAMM,4,16,1);  //写入  	 
//	}			
	
	if(PLAY_DATE.Config_Counter!=Config_Counter)   //RTC正确校准数
	{
	   PLAY_DATE.Config_Counter = Config_Counter;   //备份
		 OLED_ShowNum(96,16,Config_Counter,4,16,1);  //写入   
	}		
	
	if(PLAY_DATE.Write_Counter!=Write_Counter)   //GPS写入数
	{
	   PLAY_DATE.Write_Counter = Write_Counter;   //备份
		 OLED_ShowNum(64,0,Write_Counter,8,16,1);  //写入  
	}	
}

void PLAY_GPS_Cartoon(u8 PLAY_NUM)    //显示GPS定位动画  节拍为0至 GPS_PLAY_LEN-1   共GPS_PLAY_LEN片段
{
	PLAY_NUM = PLAY_NUM % GPS_PLAY_LEN ;
  Movie_Show_BMP(GPS_Cartoon[PLAY_NUM]);
	OLED_Gram_Update();    //显示
}






