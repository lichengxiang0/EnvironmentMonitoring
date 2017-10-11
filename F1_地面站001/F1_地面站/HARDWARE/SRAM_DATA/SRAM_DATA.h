#ifndef _SRAM_DATA_H_
#define _SRAM_DATA_H_	 
#include "sys.h"
#include "MY_FIFO.H"
#include "GPS_DATE.h"
#include "Cam_FIFO.h"

#define   FIL_NAME_LEN  80   //创建文件名长度

#define   GPS_ERR_MAX      ((u32)300000)      //GPS板卡故障阈值

extern u8 *Folder_Name ;	               //保存文件夹名         保存用户数据文件夹明
extern u8 *Moving_Station_File_Name ;	   //保存文件名    GPS数据文件
extern u8 *Aerial_Point_File_Name ;	     //保存文件名      航拍点时间文件 
extern u8 *Write_Buff ;	                 //航拍点写书数据缓冲区
extern u8 *Icm20602_File_Name ;	   //保存文件名    GPS数据文件
extern u8 *laser_File_Name ;	   //保存文件名    GPS数据文件

extern MY_FIFO_typ   *RX_BUF_FIFO   ;    //串口接收缓存FIFO

extern MY_GPS_typ    *GPS_BUF        ;   //GPS时间数据缓存
extern MY_UTC_typ    *UTC_BUF        ;   //UTC时间数据缓存

extern MY_GPS_TIME_typ  *GPS_TIME    ;   //GPS解析时间缓存
extern MY_UTC_TIME_typ  *UTC_TIME    ;   //UTC时间数据结构体

extern Cam_FIFO_typ *TIME_FIFO ;         //快门时间点 FIFO

extern u8  SD_Res_STA            ;       //SD卡文件操作状态  
extern u8  RTC_ADJUS             ;       //校准指示变量
//RTC校准变量 控制宏--------------------------------------------
#define SET_RTC_Nuber(x)         RTC_ADJUS = x         //装载校准次数
#define RTC_CONFIG_Nuber_ADD     RTC_ADJUS --          //校准次数减一
#define RTC_CONFIG_FLAT          RTC_ADJUS             //校准标记   为0时校准完成


//DMA双缓存指针 -------------------------------------------------
//#define   DMA_BUFF_SIZE    ((u32)12*1024)      //双缓存大小  
#define   DMA_BUFF_SIZE    6      //双缓存大小  
#define   ICM20602_BUFF_SIZE   15

#define  RX_DMA_FLAT       (RX_DMA_STA&0x80)  //更新标记
#define  SET_RX_DMA_UP      RX_DMA_STA  |=  0x80  //设置更新标记
#define  CLR_RX_DMA_UP      RX_DMA_STA  &=~ 0x80  //清除标记

#define  SET_RX_DMA_BUF1    RX_DMA_STA |= 0x01     //设置DMA缓存 1
#define  SET_RX_DMA_BUF0    RX_DMA_STA &=~0X01     //设置DMA缓存 0 接收

#define  RX_DMA_Nuber       (RX_DMA_STA&0x01)      //当前接收缓存

extern u8  *RX_DMA_BUFF0 ;    //双缓存
extern u8  *RX_DMA_BUFF1 ;  
extern u8   RX_DMA_STA; 







u8 SRAM_DATA_Init(void) ;  //用户数据指针申请内存  返回0 成功   1失败

u8 SYS_START_Init(void) ;  //系统开始运行时 内存申请  返回 0 成功  1失败

void SYS_Scan_TF_Err(void) ;   //TF 操作错误检测

#endif


