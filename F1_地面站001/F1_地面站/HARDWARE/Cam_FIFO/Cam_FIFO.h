#ifndef _CAM_FIFO_H__
#define _CAM_FIFO_H__
#include "sys.h"
#include "Rtc.h" 

#define Cam_TIME_MAX       10      //����ʱ��� FIFO ���泤��

typedef enum
{
  Cam_FIFO_Free  = 0     ,      //����
	Cam_FIFO_OK    = 0     ,      //�ɹ�
	Cam_FIFO_BUSY  = 0x01  ,      //æ
  Cam_FIFO_over  = 0X02  ,      //��
	Cam_FIFO_DATA  = 0x08  ,      //������  û����
	Cam_FIFO_NUM   = 0X04  ,      //û������  ���߻���û���㹻������Ч����
}
Cam_FIFO_STA_ENUM;    //FIFO״̬ö�� 


__packed typedef struct  //�����ֽڶ���
{
  u8   STA         ;       //״̬   Ԥ��
	u8   W_Number    ;       //дָ��
	u8   R_Number    ;       //��ָ��
	u8   DATA_Number ;       //FIFO��Ч����
	_calendar_obj  TIME_BUF[Cam_TIME_MAX] ;   //FIFO ����
}
Cam_FIFO_typ;   //����ʱ��� FIFO�ṹ



void Cam_FIFO_CLR(Cam_FIFO_typ * TIME_FIFO);                            //���FIFO

void Cam_FIFO_CLR_STA(Cam_FIFO_typ * TIME_FIFO,u8 Cam_FIFO_STA) ;      //���FIFO״̬���
void Cam_FIFO_SET_STA(Cam_FIFO_typ * TIME_FIFO,u8 Cam_FIFO_STA) ;      //����FIFO״̬
u8   Cam_FIFO_RETURN_STA(Cam_FIFO_typ * TIME_FIFO) ;                   //���ص�ǰFIFO״̬
u8   Cam_FIFO_STA_UP(Cam_FIFO_typ * TIME_FIFO);                        //����FIFO״̬������

u8   Cam_FIFO_WRITE(Cam_FIFO_typ * TIME_FIFO,_calendar_obj * TIME);      //���� 0  �ɹ�  ����ʧ��
u8   Cam_FIFO_READ(Cam_FIFO_typ * TIME_FIFO,_calendar_obj * TIME);     //���� 0  �ɹ�  ����ʧ��
u16  Cam_FIFO_BUF_Valid_NUM(Cam_FIFO_typ * TIME_FIFO);                 //���ص�ǰ������Ч����



















#endif






