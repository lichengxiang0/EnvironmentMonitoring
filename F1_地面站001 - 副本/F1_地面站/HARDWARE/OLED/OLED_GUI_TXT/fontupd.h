#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
#include <stm32f10x.h>


void fupd_prog(u8 x,u8 y,u32 fsize,u32 pos);	//��ʾ���½���
u8 updata_fontx(u16 x,u16 y,u8 *fxpath,u8 fx);	//����ָ���ֿ�
u8 update_font(void);			            //����ȫ���ֿ�
u8 font_init(void);										//��ʼ���ֿ�






#endif





















