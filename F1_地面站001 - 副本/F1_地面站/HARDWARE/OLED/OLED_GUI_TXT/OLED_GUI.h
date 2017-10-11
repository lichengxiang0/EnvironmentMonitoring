#ifndef __OLED_GUI_H_
#define __OLED_GUI_H_	 
#include "sys.h"
#include "Oled.h"

#define pi 3.1412926 //����Բ����

void OLED_Gram_BMP(u8 *bmp);  //ͼƬ����д����ʾ����ʾ  ��ȡģ�ļ�˵��ȡģ

void OLED_DrawPoint(u8 x,u8 y,u8 t);  //���� 1��  0��  2 ȡ��
void OLED_Fills(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot,u8 play);//��� 1�� 0��  2ȡ�� play 1��ʾ�Դ�  0����ʾ
void OLED_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);//�����ľ���
void OLED_DrawCircle(u8 x,u8 y,u8 r);//��ԲȦ
void OLED_DrawFillCircle(u8 x,u8 y,u8 r,u8 n);//��ʵ��ԲȦ
void OLED_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 n);//����
void OLED_DrawHand(u8 x1,u8 y1,u8 R,u8 T,u16 S,u8 n);//��ָ�� x1 y1��ʾ����Բ������ R��ָ�볤�� T��ָ����ָ�̶ȣ��롢��Ϊ60��ʱΪ12��S:ָ�벽��

u32 oled_pow(u8 m,u8 n); //m^n����

void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size,u8 mode,u8 play);  //��ʾ�ַ���  16 8*16 ����  8  6*8   play 1��ʾ�Դ�  0����ʾ

void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size,u8 play);  //��ʾһ������ size  16 8*16 ����  8  6*8    play 1��ʾ�Դ�  0����ʾ


void Movie_Show_BMP(const u8 *BMP);  //��ֱȡģ
void Movie_Show_Img(u32 picx);  //��ֱȡģ    ��ʾָ������ ��ģ����

#endif








