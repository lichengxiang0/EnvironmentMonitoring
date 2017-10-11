#ifndef __GPS_DATE_H_
#define __GPS_DATE_H_	 
#include "sys.h"

#define UTC_BUF_LEN       80

typedef enum
{
  UTC_FREE    = 0x00  ,      //空闲
	UTC_Gp_F    = 0x01  ,      //接收到帧头标记
  UTC_Comple  = 0X02  ,      //接收完一帧数据
	UTC_OK_FIR  = 0x04  ,      //已经解析了一次UTC时间数据
	UTC_ERR     = 0X10  ,      //操作错误
	UTC_BUSY    = 0X20  ,      //UTC缓存数据正在解析  不允许更改
	UTC_OVER    = 0X40  ,      //收到结束标记   先收到0X0D  再收到 0X0A
}
UTC_STA_ENUM;    //UTC状态枚举 


__packed typedef struct
{
  u16     syear;
	u8      smon;
	u8      sday;
	u8      hour;
	u8      min;
	u8      sec;
}
MY_UTC_TIME_typ;     //UTC时间数据结构体


__packed typedef struct
{
	u8     UTC_STA;       //状态     收到字符帧头  接收完一帧数据     已成功解析一次UTC时间数据
  u8     UTC_NUM;       //缓存指针   
	u8     UTC_SEC;       //第一次解析的时间秒数据
	u8     UTC_BUF[UTC_BUF_LEN+1] ;   //UTC缓存    最后一位不允许写 为结束字符
}
MY_UTC_typ;     //UTC数据解析结构体

void UTC_BUF_clear_Init(MY_UTC_typ *UTC_BUF) ;               //清除所以数据及标记
u16  UTC_BUF_Valid_NUM(MY_UTC_typ *UTC_BUF) ;                //返回当前缓存有效数据长度
UTC_STA_ENUM UTC_W_BUF_Byte(MY_UTC_typ *UTC_BUF,u8 date)  ;        //写数据

u8 *  UTC_BUF_RETURN(MY_UTC_typ *UTC_BUF) ;          //返回UTC缓存指针地址
void  SET_UTC_BUF_OVER(MY_UTC_typ *UTC_BUF);         //在缓存尾加入结束符


u8 RETURN_LAST_SEC(MY_UTC_typ *UTC_BUF);             //返回上次校准的UTC秒数据

u8 UTC_TIME_Adjust(u8 *UTC_BUF);                     //UTC数据校准 

u8 UTC_BUF_TIME_Adjust(MY_UTC_typ *UTC_BUF) ;        //系统UTC时间校准  返回 0 可直接赋值UTC

void UTC_BUF_TIME_Adjust_Task(MY_UTC_typ *UTC_BUF,u8 RX_BUF) ;  //UTC校准任务
void UTC_TIME_AMEND(MY_UTC_TIME_typ *UTC_TIME);   //把UTC 时间修正 0至8时日期不对问题
u8 MY_NMEA_GNRMC_Analysis(MY_UTC_TIME_typ *UTC_TIME,u8 *buf);  //手动查询解析 UTC时间数据  只解析时间


//#define GPS_BUF_LEN       148
#define GPS_BUF_LEN         158

typedef enum
{
  GPS_FREE    = 0x00  ,      //空闲
	GPS_Gp_F    = 0x01  ,      //接收到帧头标记
  GPS_Comple  = 0X02  ,      //接收完一帧数据
	GPS_OK_FIR  = 0x04  ,      //已经解析了一次UTC时间数据
	GPS_ERRR    = 0X10  ,      //操作错误
	GPS_BUSY    = 0X20  ,      //UTC缓存数据正在解析  不允许更改
	GPS_OVER    = 0X40  ,      //收到结束标记   先收到0X0D  再收到 0X0A
}
GPS_STA_ENUM;    //UTC状态枚举 

__packed typedef struct
{
  u16     syear;
	u8      smon;
	u8      sday;
	u8      hour;
	u8      min;
	u8      sec;
}
MY_GPS_TIME_typ;     //GPS时间数据结构体


__packed typedef struct
{
	u8     GPS_STA;       //状态     收到字符帧头  接收完一帧数据     已成功解析一次UTC时间数据
  u8     GPS_NUM;       //缓存指针   
	u8     GPS_SEC;       //第一次解析的时间秒数据
	u8     GPS_BUFF[GPS_BUF_LEN+1] ;   //UTC缓存    最后一位不允许写 为结束字符
}
MY_GPS_typ;     //UTC数据解析结构体

u8 MY_NMEA_TIMEA_Analysis(MY_GPS_TIME_typ *GPS_TIME,u8 *buf) ;    //解析 TIMEA 此帧数据  得到UTC时间 及GPS时间偏差
void GPS_BUF_clear_Init(MY_GPS_typ *GPS_BUF);       //清除所以数据及标记
u16  GPS_BUF_Valid_NUM(MY_GPS_typ *GPS_BUF) ;        //返回当前缓存有效数据长度
GPS_STA_ENUM GPS_W_BUF_Byte(MY_GPS_typ *GPS_BUF,u8 date);                //写数据
u8 * GPS_BUF_RETURN(MY_GPS_typ *GPS_BUF)   ;        //返回GPS缓存指针地址
void  SET_GPS_BUF_OVER(MY_GPS_typ *GPS_BUF);         //在缓存尾加入结束符
u8 RETURN_LAST_SEC_GPS(MY_GPS_typ *GPS_BUF) ;            //返回上次校准的UTC秒数据
u8 GPS_TIME_Adjust(u8 *GPS_BUF) ;    //GPS数据校准 

u8 GPS_BUF_TIME_Adjust(MY_GPS_typ *GPS_BUF) ;   //系统UTC时间校准  返回 0 可直接赋值UTC
void GPS_BUF_TIME_Adjust_Task(MY_GPS_typ *GPS_BUF,u8 RX_BUF);   //UTC校准任务

void GPS_TIME_AMEND(MY_GPS_TIME_typ *GPS_TIME,int gps_utc_offset);   //把UTC 时间修正成 GPS时间 包括0至8时区域日期不对问题

#endif



