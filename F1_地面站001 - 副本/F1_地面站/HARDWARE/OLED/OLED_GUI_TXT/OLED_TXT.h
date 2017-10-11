#ifndef _OLED_TXT_H_
#include "sys.h"
/*********************V 2.1 版本说明*****************************
 * 作者： 周楚刚     完成时间： 15 - 4 -22
 * 实现功能： 已从原子开发板实现字库的制作
 * 已构成功能函数 显示字符串和汉字  能自动换行 
 * 存在问题： 暂时还未找到C51 字节对齐的有效关键字 
 * 无法读取到有效的成员变量值   移植性及扩展性较差
 * 无法有效的判断是否存在字库及字库完整
*****************************************************************/

/*********************V 2.2 版本说明*******************************
 * 更新时间： 15 - 4 - 23
 * 更新说明： 修复无法读取到有效成员变量值   
 * C51单片机为8位单片机 无字节对齐问题 
 * 字库烧写用STM32为小端模式   C51为大端模式  数据读取时出错
 * 已在本文件中加入大小端转换函数  如果字库制作不为大端模式 则需调用
 * 已修复大小端问题  移植性及扩展性大大增加
******************************************************************/

#define Mode_Swapping_EN     0      //如果字库烧写的单片机不为大端模式 此宏值需为 1
#define PACK_EN              1      //需要结构体字节对齐 此宏值为1   即单片机不为8位需要设为1


#define FONTINFOADDR      ((u32)0)   //字库头起始地址
#define FONTSECSIZE       ((u32)((766080+174344)/4096)+1)         //字库扇区数量


#if PACK_EN 
__packed typedef struct 
{
	u8  fontok;				//字库存在标志，0XAA，字库正常；其他，字库不存在
	u32 ugbkaddr; 		//unigbk的地址
	u32 ugbksize;			//unigbk的大小	 
	u32 f12addr;			//gbk12地址	
	u32 gbk12size;		//gbk12的大小	 
	u32 f16addr;			//gbk16地址
	u32 gbk16size;		//gbk16的大小		 
	u32 f24addr;			//gbk24地址
	u32 gkb24size;		//gbk24的大小 
}_fontinfo; 		

#else
typedef struct               //无字节对齐
{
	u8  fontok;				//字库存在标志，0XAA，字库正常；其他，字库不存在
	u32 ugbkaddr; 		//unigbk的地址
	u32 ugbksize;			//unigbk的大小	 
	u32 f12addr;			//gbk12地址	
	u32 gbk12size;		//gbk12的大小	 
	u32 f16addr;			//gbk16地址
	u32 gbk16size;		//gbk16的大小		 
	u32 f24addr;			//gbk24地址
	u32 gkb24size;		//gbk24的大小 
}_fontinfo;	

#endif

extern _fontinfo ftinfo;	//字库信息结构体

void convertToLittleEndian(u32 *date);   //大小端转换函数    大端转换成小端   小端转换成大端

u8 OLED_TXT_Init(void);     //字库初始化   0成功  1失败

void Get_HzMat(u8 *code_date,u8 *mat);  //code_date 字符串的开始地址,GBK码  从字库中查找出字模 mat  数据存放地址

void OLED_GUI_Str(u8 x,u8 y,u8 *p,u8 mode,u8 play) ; //显示汉字和字符串





#endif

