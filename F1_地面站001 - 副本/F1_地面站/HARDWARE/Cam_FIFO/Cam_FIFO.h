#ifndef _CAM_FIFO_H__
#define _CAM_FIFO_H__
#include "sys.h"
#include "Rtc.h" 

#define Cam_TIME_MAX       10      //快门时间点 FIFO 缓存长度

typedef enum
{
  Cam_FIFO_Free  = 0     ,      //空闲
	Cam_FIFO_OK    = 0     ,      //成功
	Cam_FIFO_BUSY  = 0x01  ,      //忙
  Cam_FIFO_over  = 0X02  ,      //满
	Cam_FIFO_DATA  = 0x08  ,      //有数据  没有满
	Cam_FIFO_NUM   = 0X04  ,      //没有数据  或者缓存没有足够长的有效数据
}
Cam_FIFO_STA_ENUM;    //FIFO状态枚举 


__packed typedef struct  //放弃字节对齐
{
  u8   STA         ;       //状态   预留
	u8   W_Number    ;       //写指针
	u8   R_Number    ;       //读指针
	u8   DATA_Number ;       //FIFO有效数据
	_calendar_obj  TIME_BUF[Cam_TIME_MAX] ;   //FIFO 缓存
}
Cam_FIFO_typ;   //快门时间点 FIFO结构



void Cam_FIFO_CLR(Cam_FIFO_typ * TIME_FIFO);                            //清空FIFO

void Cam_FIFO_CLR_STA(Cam_FIFO_typ * TIME_FIFO,u8 Cam_FIFO_STA) ;      //清除FIFO状态标记
void Cam_FIFO_SET_STA(Cam_FIFO_typ * TIME_FIFO,u8 Cam_FIFO_STA) ;      //设置FIFO状态
u8   Cam_FIFO_RETURN_STA(Cam_FIFO_typ * TIME_FIFO) ;                   //返回当前FIFO状态
u8   Cam_FIFO_STA_UP(Cam_FIFO_typ * TIME_FIFO);                        //更新FIFO状态并返回

u8   Cam_FIFO_WRITE(Cam_FIFO_typ * TIME_FIFO,_calendar_obj * TIME);      //返回 0  成功  其它失败
u8   Cam_FIFO_READ(Cam_FIFO_typ * TIME_FIFO,_calendar_obj * TIME);     //返回 0  成功  其它失败
u16  Cam_FIFO_BUF_Valid_NUM(Cam_FIFO_typ * TIME_FIFO);                 //返回当前缓存有效数据



















#endif






