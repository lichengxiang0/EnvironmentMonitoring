#ifndef _SRAM_DATA_H_
#define _SRAM_DATA_H_	 
#include "sys.h"
#include "MY_FIFO.H"
#include "GPS_DATE.h"
#include "Cam_FIFO.h"

#define   FIL_NAME_LEN  80   //�����ļ�������

#define   GPS_ERR_MAX      ((u32)300000)      //GPS�忨������ֵ

extern u8 *Folder_Name ;	               //�����ļ�����         �����û������ļ�����
extern u8 *Moving_Station_File_Name ;	   //�����ļ���    GPS�����ļ�
extern u8 *Aerial_Point_File_Name ;	     //�����ļ���      ���ĵ�ʱ���ļ� 
extern u8 *Write_Buff ;	                 //���ĵ�д�����ݻ�����
extern u8 *Icm20602_File_Name ;	   //�����ļ���    GPS�����ļ�
extern u8 *laser_File_Name ;	   //�����ļ���    GPS�����ļ�

extern MY_FIFO_typ   *RX_BUF_FIFO   ;    //���ڽ��ջ���FIFO

extern MY_GPS_typ    *GPS_BUF        ;   //GPSʱ�����ݻ���
extern MY_UTC_typ    *UTC_BUF        ;   //UTCʱ�����ݻ���

extern MY_GPS_TIME_typ  *GPS_TIME    ;   //GPS����ʱ�仺��
extern MY_UTC_TIME_typ  *UTC_TIME    ;   //UTCʱ�����ݽṹ��

extern Cam_FIFO_typ *TIME_FIFO ;         //����ʱ��� FIFO

extern u8  SD_Res_STA            ;       //SD���ļ�����״̬  
extern u8  RTC_ADJUS             ;       //У׼ָʾ����
//RTCУ׼���� ���ƺ�--------------------------------------------
#define SET_RTC_Nuber(x)         RTC_ADJUS = x         //װ��У׼����
#define RTC_CONFIG_Nuber_ADD     RTC_ADJUS --          //У׼������һ
#define RTC_CONFIG_FLAT          RTC_ADJUS             //У׼���   Ϊ0ʱУ׼���


//DMA˫����ָ�� -------------------------------------------------
//#define   DMA_BUFF_SIZE    ((u32)12*1024)      //˫�����С  
#define   DMA_BUFF_SIZE    6      //˫�����С  
#define   ICM20602_BUFF_SIZE   15

#define  RX_DMA_FLAT       (RX_DMA_STA&0x80)  //���±��
#define  SET_RX_DMA_UP      RX_DMA_STA  |=  0x80  //���ø��±��
#define  CLR_RX_DMA_UP      RX_DMA_STA  &=~ 0x80  //������

#define  SET_RX_DMA_BUF1    RX_DMA_STA |= 0x01     //����DMA���� 1
#define  SET_RX_DMA_BUF0    RX_DMA_STA &=~0X01     //����DMA���� 0 ����

#define  RX_DMA_Nuber       (RX_DMA_STA&0x01)      //��ǰ���ջ���

extern u8  *RX_DMA_BUFF0 ;    //˫����
extern u8  *RX_DMA_BUFF1 ;  
extern u8   RX_DMA_STA; 







u8 SRAM_DATA_Init(void) ;  //�û�����ָ�������ڴ�  ����0 �ɹ�   1ʧ��

u8 SYS_START_Init(void) ;  //ϵͳ��ʼ����ʱ �ڴ�����  ���� 0 �ɹ�  1ʧ��

void SYS_Scan_TF_Err(void) ;   //TF ����������

#endif


