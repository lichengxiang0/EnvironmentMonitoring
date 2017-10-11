#ifndef  _MY_FIFO_H_
#define  _MY_FIFO_H_

#include "stm32f10x.h"

#define FIFO_BUF_MAX   (200)   //FIFO���ͻ��泤��   ����ڴ治�� ���Լ�С

typedef enum
{
  FIFO_Free  = 0     ,      //����
	FIFO_OK    = 0     ,      //�ɹ�
	FIFO_BUSY  = 0x01  ,      //æ
  FIFO_over  = 0X02  ,      //��
	FIFO_NUM   = 0X04  ,      //û������  ���߻���û���㹻������Ч����
}
FIFO_STA_ENUM;    //FIFO״̬ö�� 


__packed typedef struct
{
  u8            FIFO_STA;                   //FIFO��ǰ״̬   æ  ��   ��   ����
  u16           DATA_NUM;                   //FIFO��������Ч���ݸ���
	u16           W_ADDR_NUM;                 //дָ��
	u16           R_ADDR_NUM;                 //��ָ��
	u8            FIFO_BUFF[FIFO_BUF_MAX] ;   //FIFO����
}
MY_FIFO_typ;     //FIFO�ṹ��


void FIFO_CLR_BUF(MY_FIFO_typ * FIFO);   //���FIFO

void FIFO_CLR_STA(MY_FIFO_typ * FIFO,u8 FIFO_STA) ;         //���FIFO״̬���
void FIFO_SET_STA(MY_FIFO_typ * FIFO,u8 FIFO_STA) ;         //����FIFO״̬
u8   FIFO_RETURN_STA(MY_FIFO_typ * FIFO) ;                  //���ص�ǰFIFO״̬
u8   FIFO_STA_UP(MY_FIFO_typ * FIFO);                       //����FIFO״̬������

u8   FIFO_WRITE_Byte(MY_FIFO_typ * FIFO,u8 date) ;          //���� 0  �ɹ�  ����ʧ��
u8   FIFO_WRITE_BUF(MY_FIFO_typ * FIFO,u8 *buf,u16 lenght); //���� 0  �ɹ�  ����ʧ��
u8   FIFO_READ_Byte(MY_FIFO_typ * FIFO,u8 *date_Byte);      //���� 0  �ɹ�  ����ʧ��
u8   FIFO_READ_BUF(MY_FIFO_typ * FIFO,u8 *buf,u16 lenght) ; //���� 0  �ɹ�  ����ʧ��

u16  FIFO_BUF_Valid_NUM(MY_FIFO_typ * FIFO);                //���ص�ǰ������Ч����



#endif










