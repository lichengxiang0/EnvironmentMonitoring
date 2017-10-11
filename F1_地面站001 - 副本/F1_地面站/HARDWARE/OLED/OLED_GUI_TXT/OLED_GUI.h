#ifndef __OLED_GUI_H_
#define __OLED_GUI_H_	 
#include "sys.h"
#include "Oled.h"

#define pi 3.1412926 //定义圆周率

void OLED_Gram_BMP(u8 *bmp);  //图片数组写入显示并显示  按取模文件说明取模

void OLED_DrawPoint(u8 x,u8 y,u8 t);  //画点 1亮  0灭  2 取反
void OLED_Fills(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot,u8 play);//填充 1亮 0灭  2取反 play 1显示显存  0不显示
void OLED_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);//画空心矩形
void OLED_DrawCircle(u8 x,u8 y,u8 r);//画圆圈
void OLED_DrawFillCircle(u8 x,u8 y,u8 r,u8 n);//画实心圆圈
void OLED_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 n);//画线
void OLED_DrawHand(u8 x1,u8 y1,u8 R,u8 T,u16 S,u8 n);//画指针 x1 y1表示表盘圆心坐标 R：指针长度 T：指针所指刻度（秒、分为60，时为12）S:指针步长

u32 oled_pow(u8 m,u8 n); //m^n函数

void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size,u8 mode,u8 play);  //显示字符串  16 8*16 或者  8  6*8   play 1显示显存  0不显示

void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size,u8 play);  //显示一个数字 size  16 8*16 或者  8  6*8    play 1显示显存  0不显示


void Movie_Show_BMP(const u8 *BMP);  //垂直取模
void Movie_Show_Img(u32 picx);  //垂直取模    显示指定数组 字模数据

#endif








