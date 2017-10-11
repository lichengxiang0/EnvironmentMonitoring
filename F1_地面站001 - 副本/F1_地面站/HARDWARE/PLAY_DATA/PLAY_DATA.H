#ifndef   _PLAY_DATA_H_
#define   _PLAY_DATA_H_
#include "stm32f10x.h"
#include "Rtc.h" 

extern u32 Write_Counter ;	     //OLED打印GPS数据写入SD卡次数
extern u32 Write_Counter_CAMM ;  //拍摄照片数
extern u16 Config_Counter ;      //RTC校准次数
extern u16 RTC_Config_ERR;       //RTC校准错误计数  最高位校准错误标记  其它校准错误计数


typedef  struct
{
 u32 Write_Counter ;	           //OLED打印GPS数据写入SD卡次数
 u32 Write_Counter_CAMM ;        //拍摄照片数
 u16 Config_Counter ;            //RTC校准次数
 u16 RTC_Config_ERR;             //RTC校准错误计数  最高位校准错误标记  其它校准错误计数
 _calendar_obj   calendar_bf;    //RTC时间备份结构
	
}PLAY_BF;

extern PLAY_BF  PLAY_DATE;      //显示对象结构体


void PLAY_DATE_TASK(void) ;     //显示需显示数据   有变化才刷新

void PLAY_DATE_Init(void) ;     //显示需显示数据   有变化才刷新  初始化


void PLAY_GPS_Cartoon(u8 PLAY_NUM) ;   //显示GPS定位动画  节拍为 0至 4   共5片段

#endif

