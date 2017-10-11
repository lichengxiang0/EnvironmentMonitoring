#ifndef _OLED_BMP_Font_h_
#define _OLED_BMP_Font_h_

#include "OLED_BMP_CONFIG.h"
#include "OLED_BMP.h"
/*****************************取模说明*****************************************************/   
 //     阴码         列行式            逆向输出																
 //    C51格式    注释前缀： /*"   数据前缀：0x    数据后缀：,   行后缀:, 行尾缀: */  
 //注意！ 经测试  无论图片取模还是文字取模都只能用 PCoLCD2002软件  
/******************************************************************************************/


__packed typedef struct
{
	const u8 BMP_ID[BMP_ID_SIZE+1];       //图片名字
  const u8 BMP_font[1024];  //图片数据字模
}BMP_FONT_typ;   //定义新结构类型

typedef const BMP_FONT_typ BMP_FONT_CODE;  //定义新结构保存至程序存储空间

extern BMP_FONT_CODE BMP_FONT[];   //定义结构体数组  保存至程序存储空间





/*************************************************************
                      应用层
**************************************************************/
#if FLASH_DATA_SET_EN   //文件使能

//把OLED_BMP_FONT中图片数据全部写入FLASH中  
//接在FLASH内部图片地址后面  返回0 成功  其它失败
// num为正确写入的照片数
u8  FLASH_Write_font(u16 *num) ;


//写入图片数据到FLASH   返回0 写入成功   其它失败
//如果待写地址没有擦除  则写入失败  退出
u8  FLASH_Write_bmp(bmp_code_typ *BMP_code);

void OLED_BMP_Erase(void);            //擦除FLASH图片数据所有扇区



#endif

#endif
