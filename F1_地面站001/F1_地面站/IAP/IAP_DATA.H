#ifndef   __IAP_DATA_H__   
#define   __IAP_DATA_H__

#include "SYS.h"
#include "stmflash.h"

//STM32F103RCT6   FLASH 大小为256K     每个扇区为2K 大小     共128个扇区
//FLASH 内容定义如下
/*
//------------------------------------------------------------
       第扇区 0
			   .
				 . 
				 .                   用于存放 Bootloader    共 30K 大小   15个扇区  
				 .
			 第扇区 14                                              地址  0X8007000
//-------------------------------------------------------------

//-------------------------------------------------------------
       第扇区 15             用于存放IAP 数据     共2K  大小   地址  0X8007800
//-------------------------------------------------------------

//-------------------------------------------------------------
       第扇区 16            地址 0X8008000
			     .
					 .                用于存放 用户代码  共 224K    共112个扇区
					 .
			 第扇区 127
//-------------------------------------------------------------
*/
extern const u8  *Updata_Path ;  //更新文件路径
	
#define   CRC_SECTION_SIZE                       ((u32)2*1024)                                       //把用户代码区域分成此大小为一段

#define   APP_DATA_START_Adress                  ((u32)0X8007800)                                    //数据校验起始地址

#define   SECTION_PAGE_MAX                       ((u32)112)                                          //扇区数     只考虑用户代码扇区
#define   SECTION_PAGE_SIZE                      ((u32)(2*1024))                                     //块大小     只考虑用户代码扇区
#define   Section_Start_Adress(X)                ((u32)(FLASH_BASE + 0X8000 + X * SECTION_PAGE_SIZE))//扇区起始地址 只考虑用户代码扇区地址

#define   APP_START_PAGE                         ((u32)0)                                            //APP代码 起始扇区
#define   APP_START_ADDR                         Section_Start_Adress(APP_START_PAGE)                //APP代码 起始地址

#define   IAP_CRC_Fragment_Size                 ((u32)(SECTION_PAGE_SIZE*SECTION_PAGE_MAX)/CRC_SECTION_SIZE) //校验扇区数量  校验段数量  


#define  IAP_CRC_HEAD                      0X5378      //标记头

__packed typedef struct
{
	u16  IAP_CRC_DATA_HEAD                    ;       //标记头
	u32  APP_STAR_ADDR                        ;       //APP起始地址
  u32  File_SIZE                            ;       //文件大小
	u32  APP_END_ADDR                         ;       //APP结束地址
	u32  DATA_CRC[IAP_CRC_Fragment_Size]      ;       //CRC校验值
}
IAP_CRC_DATA_TYP;  //APP文件 数据结构体   

#define     SYS_Destruct_DR                    BKP->DR42          //自毁标记寄存器

void Read_FLASH_APP_CRC_DATA(IAP_CRC_DATA_TYP * IAP_CRC_DAT ) ;   //读取保存在FLASH 中的校验值

u8 Read_USER_FLASH_DATA_CRC(IAP_CRC_DATA_TYP * IAP_CRC_DAT )  ;   //得到用户代码 校验值  返回0 成功    1失败

void Write_FLASH_APP_CRC_DATA(IAP_CRC_DATA_TYP * IAP_CRC_DAT ) ;  //写FLASH数据 
void ErasePage_FLASH_APP_CRC_DATA(void);    //擦除IAP CRC数据
void ErasePage_FLASH_APP_FLASH(u32 APP_Size) ;    //擦除用户代码 FLASH 数据 扇区
	
u8  Bijiao_CRC_DATA(IAP_CRC_DATA_TYP * IAP_CRC_DAT0,IAP_CRC_DATA_TYP * IAP_CRC_DAT1) ;  //比较   相同为 0   不同为 1

void SYS_SCAN_Destruct_Exe(void) ; //检测设备是否需要自毁
void SET_SYS_Destruct(void) ;   //设置设备自毁


void Scan_SYS_Bootloader_Lock(void) ;  //检测用户代码是否为Bootloader烧录  如果不是设备自毁

#endif









