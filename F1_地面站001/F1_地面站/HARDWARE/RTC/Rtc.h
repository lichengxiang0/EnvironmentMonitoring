#ifndef __RTC_H_
#define __RTC_H_	    
#include "stm32f10x.h"

//时间结构体
__packed typedef struct      //放弃字节对齐
{
	vu8 hour;
	vu8 min;
	vu8 sec;	
	vu32 micro_sec;
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}_calendar_obj;					 

extern _calendar_obj calendar;	      //日历结构体
extern _calendar_obj Cam_WRITE_time;	//航拍时间写入变量

extern u16 Tim3_1ms_Counter;//定时器3中断计数器   1MS计数器

extern u8 const mon_table[12];	//月份日期数据表


extern const u8 table_week[12]; //月修正数据表	  

extern const u8 mon_table[12]; //平年的月份日期表


void Disp_Time(u8 x,u8 y,u8 size);//在制定位置开始显示时间
void Disp_Week(u8 x,u8 y,u8 size,u8 lang);//在指定位置显示星期
u8 RTC_Init(void);        //初始化RTC,返回0,失败;1,成功;
u8 Is_Leap_Year(u16 year);//平年,闰年判断
u8 RTC_Get(void);         //更新时间   

u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//设置时间			 


//得到当前的时间
void Cal_RTC_Get(_calendar_obj * calendar_time) ;   //更新航拍点时间
	

#endif


