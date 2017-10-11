#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
#include <stm32f10x.h>


void fupd_prog(u8 x,u8 y,u32 fsize,u32 pos);	//显示更新进度
u8 updata_fontx(u16 x,u16 y,u8 *fxpath,u8 fx);	//更新指定字库
u8 update_font(void);			            //更新全部字库
u8 font_init(void);										//初始化字库






#endif





















