#ifndef _OLED_BMP_H_
#define _OLED_BMP_H_
 /**********************************************************************************
Flash图片数组存储(最后200k)
  (200*1024/4=50  511-50+1 = 462)  第462扇区（共 2*1024/4 = 512个）   字节地址为 1892352  = 462*4*1024
   单独的一个扇区存放图片数组指针，
   开始连续四字节存放图片数组存在标记及校验大小端，如0X12345678，
   后四字节存放图片数组开始地址。然后开始存放数组指针结构体。
	 
	 结构体结构 
   起始标记（OX5A）一个字节，
   大小端校验，校验数据如0X1234， 两个字节
   第几张图片两个字节，
   图片数组的起始地址 四个字节
   图片名字八个字节，最多4个汉字GBK码，无效则用0X00填充
   结束标记（OXA5）一个字节

typedef struct
{
   u8          start;      //图片指针结构体起始标记  0X5A     1个字节
	 u16         MOS_flaot;  //大小端校验标记   正确值为0X1234或者0x3412     0x3412需要转换  其它值错误   2个字节
   u16         bmp_num;    //图片索引                        2个字节
   u32         bmp_aderr;  //图片数组起始地址                 4个字节
   u8          bmp_ID[8];  //图片名字或简写  GBK              8个字节
   u8          end;        //字库指针结构体  结束标记 0XA5     1个字节
	 BMP_code_u8    *bmp_ID_code;//图片名字数据指针                  2个字节
	 BMP_code_u8    *bmp_code;   //图片数组数据指针                  2个字节
}bmp_code_typ;    //图片指针结构体    共22个字节   保存的字节数为 22-2-2 = 18

   结构体为22字节        经测试 BMP_code_u8 类型为 2字节


#define FLASH_Volume        ((u32)2)                                          //FLASH容量  单位: M
#define FLASH_Sector_SIZE   ((u32)4096)                                       //FLASH扇区大小   单位： 字节
#define BMP_SIZE            ((u32)200*1024)                                   //FLASH给与图片数据字节大小
#define BMP_ALL_Sector      (BMP_SIZE/FLASH_Sector_SIZE)                          //FLASH给与图片数据扇区数

#define FLASH_ALL_Adress    ((u32)FLASH_Volume*1024*1024)                     //FLASH总字节地址
#define OLED_BMP_START      (FLASH_ALL_Adress-BMP_SIZE)                           //FLASH图片数组指针开始地址   
#define FLASH_START_Sector  (OLED_BMP_START/FLASH_Sector_SIZE)                    //FLASH图片数组指针开始地址对应扇区

#define FLASH_Pointer_MAX   (FLASH_Sector_SIZE/(sizeof(bmp_code_typ)-sizeof(const char *)*2))          //FLASH图片数组指针最大数量           

#define OLED_CODE_Adress    ((u32)(FLASH_START_Sector+1)*FLASH_Sector_SIZE)   //FLASH图片数组数据写入首地址
#define OLED_CODE_Sector    (FLASH_START_Sector+1)                                //FLASH图片数组数据写入首地址对应扇区


  第X张图片数组起始地址为 = 图片数组起始地址+X*1024

***********************************************************************************/

#include "OLED_BMP_CONFIG.h"


#define   BMP_ID_SIZE          20        //图片ID 最大长度

__packed typedef struct                //放弃字节对齐
{
   u8          start;                   //图片指针结构体起始标记  0X5A     1个字节
	 u16         MOS_flaot;               //大小端校验标记   正确值为0X1234或者0x3412     0x3412需要转换  其它值错误   2个字节
   u16         bmp_num;                 //图片索引                        2个字节
   u32         bmp_aderr;               //图片数组起始地址                 4个字节
   u8          bmp_ID[BMP_ID_SIZE+1];     //图片名字或简写  GBK              8个字节    后面加一 为一结束符
   u8          end;                     //字库指针结构体  结束标记 0XA5     1个字节
	 const u8    *bmp_ID_code;            //图片名字数据指针                  2个字节
	 const u8    *bmp_code;               //图片数组数据指针                  2个字节
}bmp_code_typ;    //图片指针结构体    共22个字节   保存的字节数为 22-2-2 = 18


extern bmp_code_typ BMP_Code;  //FLASH中图片数组结构指针 


   
	




u8  FLASH_BMP_play(u8 num);         //指定索引图片显示    返回 0成功  其它失败

u16 FLASH_BMP_Scan(void);           //扫描FLASH中图片张数 返回  0表示无图片  其它为图片个数

u8  FLASH_BMP_ID_play(u8 *BMP_ID);  //显示FLASH中指定ID图片

  
//比较两字符串 str0遇到结束符或者遇到不相同比较完成
//返回 0 相同  其它不同
u8 MY_STRCMP(const char * str0,const char *str1);

#endif


