#ifndef __RTC_H_
#define __RTC_H_	    
#include "stm32f10x.h"

//ʱ��ṹ��
__packed typedef struct      //�����ֽڶ���
{
	vu8 hour;
	vu8 min;
	vu8 sec;	
	vu32 micro_sec;
	//������������
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}_calendar_obj;					 

extern _calendar_obj calendar;	      //�����ṹ��
extern _calendar_obj Cam_WRITE_time;	//����ʱ��д�����

extern u16 Tim3_1ms_Counter;//��ʱ��3�жϼ�����   1MS������

extern u8 const mon_table[12];	//�·��������ݱ�


extern const u8 table_week[12]; //���������ݱ�	  

extern const u8 mon_table[12]; //ƽ����·����ڱ�


void Disp_Time(u8 x,u8 y,u8 size);//���ƶ�λ�ÿ�ʼ��ʾʱ��
void Disp_Week(u8 x,u8 y,u8 size,u8 lang);//��ָ��λ����ʾ����
u8 RTC_Init(void);        //��ʼ��RTC,����0,ʧ��;1,�ɹ�;
u8 Is_Leap_Year(u16 year);//ƽ��,�����ж�
u8 RTC_Get(void);         //����ʱ��   

u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//����ʱ��			 


//�õ���ǰ��ʱ��
void Cal_RTC_Get(_calendar_obj * calendar_time) ;   //���º��ĵ�ʱ��
	

#endif


