#ifndef __GPS_DATE_H_
#define __GPS_DATE_H_	 
#include "sys.h"

#define UTC_BUF_LEN       80

typedef enum
{
  UTC_FREE    = 0x00  ,      //����
	UTC_Gp_F    = 0x01  ,      //���յ�֡ͷ���
  UTC_Comple  = 0X02  ,      //������һ֡����
	UTC_OK_FIR  = 0x04  ,      //�Ѿ�������һ��UTCʱ������
	UTC_ERR     = 0X10  ,      //��������
	UTC_BUSY    = 0X20  ,      //UTC�����������ڽ���  ���������
	UTC_OVER    = 0X40  ,      //�յ��������   ���յ�0X0D  ���յ� 0X0A
}
UTC_STA_ENUM;    //UTC״̬ö�� 


__packed typedef struct
{
  u16     syear;
	u8      smon;
	u8      sday;
	u8      hour;
	u8      min;
	u8      sec;
}
MY_UTC_TIME_typ;     //UTCʱ�����ݽṹ��


__packed typedef struct
{
	u8     UTC_STA;       //״̬     �յ��ַ�֡ͷ  ������һ֡����     �ѳɹ�����һ��UTCʱ������
  u8     UTC_NUM;       //����ָ��   
	u8     UTC_SEC;       //��һ�ν�����ʱ��������
	u8     UTC_BUF[UTC_BUF_LEN+1] ;   //UTC����    ���һλ������д Ϊ�����ַ�
}
MY_UTC_typ;     //UTC���ݽ����ṹ��

void UTC_BUF_clear_Init(MY_UTC_typ *UTC_BUF) ;               //����������ݼ����
u16  UTC_BUF_Valid_NUM(MY_UTC_typ *UTC_BUF) ;                //���ص�ǰ������Ч���ݳ���
UTC_STA_ENUM UTC_W_BUF_Byte(MY_UTC_typ *UTC_BUF,u8 date)  ;        //д����

u8 *  UTC_BUF_RETURN(MY_UTC_typ *UTC_BUF) ;          //����UTC����ָ���ַ
void  SET_UTC_BUF_OVER(MY_UTC_typ *UTC_BUF);         //�ڻ���β���������


u8 RETURN_LAST_SEC(MY_UTC_typ *UTC_BUF);             //�����ϴ�У׼��UTC������

u8 UTC_TIME_Adjust(u8 *UTC_BUF);                     //UTC����У׼ 

u8 UTC_BUF_TIME_Adjust(MY_UTC_typ *UTC_BUF) ;        //ϵͳUTCʱ��У׼  ���� 0 ��ֱ�Ӹ�ֵUTC

void UTC_BUF_TIME_Adjust_Task(MY_UTC_typ *UTC_BUF,u8 RX_BUF) ;  //UTCУ׼����
void UTC_TIME_AMEND(MY_UTC_TIME_typ *UTC_TIME);   //��UTC ʱ������ 0��8ʱ���ڲ�������
u8 MY_NMEA_GNRMC_Analysis(MY_UTC_TIME_typ *UTC_TIME,u8 *buf);  //�ֶ���ѯ���� UTCʱ������  ֻ����ʱ��


//#define GPS_BUF_LEN       148
#define GPS_BUF_LEN         158

typedef enum
{
  GPS_FREE    = 0x00  ,      //����
	GPS_Gp_F    = 0x01  ,      //���յ�֡ͷ���
  GPS_Comple  = 0X02  ,      //������һ֡����
	GPS_OK_FIR  = 0x04  ,      //�Ѿ�������һ��UTCʱ������
	GPS_ERRR    = 0X10  ,      //��������
	GPS_BUSY    = 0X20  ,      //UTC�����������ڽ���  ���������
	GPS_OVER    = 0X40  ,      //�յ��������   ���յ�0X0D  ���յ� 0X0A
}
GPS_STA_ENUM;    //UTC״̬ö�� 

__packed typedef struct
{
  u16     syear;
	u8      smon;
	u8      sday;
	u8      hour;
	u8      min;
	u8      sec;
}
MY_GPS_TIME_typ;     //GPSʱ�����ݽṹ��


__packed typedef struct
{
	u8     GPS_STA;       //״̬     �յ��ַ�֡ͷ  ������һ֡����     �ѳɹ�����һ��UTCʱ������
  u8     GPS_NUM;       //����ָ��   
	u8     GPS_SEC;       //��һ�ν�����ʱ��������
	u8     GPS_BUFF[GPS_BUF_LEN+1] ;   //UTC����    ���һλ������д Ϊ�����ַ�
}
MY_GPS_typ;     //UTC���ݽ����ṹ��

u8 MY_NMEA_TIMEA_Analysis(MY_GPS_TIME_typ *GPS_TIME,u8 *buf) ;    //���� TIMEA ��֡����  �õ�UTCʱ�� ��GPSʱ��ƫ��
void GPS_BUF_clear_Init(MY_GPS_typ *GPS_BUF);       //����������ݼ����
u16  GPS_BUF_Valid_NUM(MY_GPS_typ *GPS_BUF) ;        //���ص�ǰ������Ч���ݳ���
GPS_STA_ENUM GPS_W_BUF_Byte(MY_GPS_typ *GPS_BUF,u8 date);                //д����
u8 * GPS_BUF_RETURN(MY_GPS_typ *GPS_BUF)   ;        //����GPS����ָ���ַ
void  SET_GPS_BUF_OVER(MY_GPS_typ *GPS_BUF);         //�ڻ���β���������
u8 RETURN_LAST_SEC_GPS(MY_GPS_typ *GPS_BUF) ;            //�����ϴ�У׼��UTC������
u8 GPS_TIME_Adjust(u8 *GPS_BUF) ;    //GPS����У׼ 

u8 GPS_BUF_TIME_Adjust(MY_GPS_typ *GPS_BUF) ;   //ϵͳUTCʱ��У׼  ���� 0 ��ֱ�Ӹ�ֵUTC
void GPS_BUF_TIME_Adjust_Task(MY_GPS_typ *GPS_BUF,u8 RX_BUF);   //UTCУ׼����

void GPS_TIME_AMEND(MY_GPS_TIME_typ *GPS_TIME,int gps_utc_offset);   //��UTC ʱ�������� GPSʱ�� ����0��8ʱ�������ڲ�������

#endif



