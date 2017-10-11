#ifndef  _MY_FIFO_H_
#define  _MY_FIFO_H_

#include "stm32f10x.h"

#define FIFO_BUF_MAX   (200)   //FIFO发送缓存长度   如果内存不够 可以减小

typedef enum
{
  FIFO_Free  = 0     ,      //空闲
	FIFO_OK    = 0     ,      //成功
	FIFO_BUSY  = 0x01  ,      //忙
  FIFO_over  = 0X02  ,      //满
	FIFO_NUM   = 0X04  ,      //没有数据  或者缓存没有足够长的有效数据
}
FIFO_STA_ENUM;    //FIFO状态枚举 


__packed typedef struct
{
  u8            FIFO_STA;                   //FIFO当前状态   忙  空   满   空闲
  u16           DATA_NUM;                   //FIFO缓存中有效数据个数
	u16           W_ADDR_NUM;                 //写指针
	u16           R_ADDR_NUM;                 //读指针
	u8            FIFO_BUFF[FIFO_BUF_MAX] ;   //FIFO缓存
}
MY_FIFO_typ;     //FIFO结构体


void FIFO_CLR_BUF(MY_FIFO_typ * FIFO);   //清空FIFO

void FIFO_CLR_STA(MY_FIFO_typ * FIFO,u8 FIFO_STA) ;         //清除FIFO状态标记
void FIFO_SET_STA(MY_FIFO_typ * FIFO,u8 FIFO_STA) ;         //设置FIFO状态
u8   FIFO_RETURN_STA(MY_FIFO_typ * FIFO) ;                  //返回当前FIFO状态
u8   FIFO_STA_UP(MY_FIFO_typ * FIFO);                       //更新FIFO状态并返回

u8   FIFO_WRITE_Byte(MY_FIFO_typ * FIFO,u8 date) ;          //返回 0  成功  其它失败
u8   FIFO_WRITE_BUF(MY_FIFO_typ * FIFO,u8 *buf,u16 lenght); //返回 0  成功  其它失败
u8   FIFO_READ_Byte(MY_FIFO_typ * FIFO,u8 *date_Byte);      //返回 0  成功  其它失败
u8   FIFO_READ_BUF(MY_FIFO_typ * FIFO,u8 *buf,u16 lenght) ; //返回 0  成功  其它失败

u16  FIFO_BUF_Valid_NUM(MY_FIFO_typ * FIFO);                //返回当前缓存有效数据



#endif










