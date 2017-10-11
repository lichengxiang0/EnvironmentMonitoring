#ifndef _OLED_BMP_CONFIG_H_
#define _OLED_BMP_CONFIG_H_

#include "w25qxx.h"
#include "Oled.h"
#include <string.h>     //字符串处理

// 串口调试 使能------------------------------------------------------
#define  UART_TEST__EN          0      //为1时开启串口调试    必须包含串口头文件

// FLASH图片数据修改 使能---------------------------------------------
#define  FLASH_DATA_SET_EN      1       //为1时 允许修改FLASH图片数据

//本次写入FLASH 图片数据个数   必须宏  FLASH_DATA_SET_EN 大于0 有效
#define  BMP_FONT_NUM           21       //当前数组图片个数  请与OLED_BMP_FONT.C 中图片数据个数对应  大于图片数据数量后面写入图片为乱码



// 允许修改FLASH 图片数据 使能---------------------------
#if FLASH_DATA_SET_EN

#include "OLED_BMP.h"
//本次写入FLASH 图片数据个数   必须宏  FLASH_DATA_SET_EN 大于0 有效
#define  BMP_FONT_MAX           BMP_FONT_NUM       //当前数组图片个数

#endif

//--------------------------------移植修改-----------------------------
//数据结构声明---------------------------------------------------------
//typedef  u8                   BMP_u8;
//#define  BMP_CODE             const            
//typedef  BMP_u8  BMP_CODE     BMP_code_u8;
//typedef  u16                  BMP_u16;
//typedef  u32                  u32;


#define FLASH_Volume        ((u32)2)                                                            //FLASH容量  单位: M
#define FLASH_Sector_SIZE   ((u32)4096)                                                         //FLASH扇区大小   单位： 字节
#define BMP_SIZE            ((u32)200*1024)                                                     //FLASH给与图片数据字节大小
#define BMP_ALL_Sector      (BMP_SIZE/FLASH_Sector_SIZE)                                        //FLASH给与图片数据扇区数

#define FLASH_ALL_Adress    ((u32)FLASH_Volume*1024*1024)                                       //FLASH总字节地址
#define OLED_BMP_START      (FLASH_ALL_Adress-BMP_SIZE)                                         //FLASH图片数组指针开始地址   
#define FLASH_START_Sector  (OLED_BMP_START/FLASH_Sector_SIZE)                                  //FLASH图片数组指针开始地址对应扇区

#define FLASH_FLAT_SIZE     (sizeof(bmp_code_typ)-(sizeof(const char *)*2))                     //FLASH数组指针写入大小
	
#define FLASH_BMP_NUBL      ((u32)(BMP_SIZE - FLASH_Sector_SIZE )/1024)                         //FLASH最多能存放图片数量
#define FLASH_Pointer_MAX   (FLASH_Sector_SIZE/FLASH_FLAT_SIZE)                                 //FLASH图片数组指针最大数量  
	
#define OLED_CODE_Adress    ((u32)(FLASH_START_Sector+1)*FLASH_Sector_SIZE)                     //FLASH图片数组数据写入首地址
#define OLED_CODE_Sector    (FLASH_START_Sector+1)                                              //FLASH图片数组数据写入首地址对应扇区

//-------------------------------------------------------------------------------------------------------------

//串口检测使能------------------------------------------
#if UART_TEST__EN
#include "Usart_Driver.h"     //包含串口头文件

//重映射以下函数----------------------------------
#define   UartInit()     Uart1_Init(115200)    
#define   Uart_printf    printf
   
#endif




#endif



