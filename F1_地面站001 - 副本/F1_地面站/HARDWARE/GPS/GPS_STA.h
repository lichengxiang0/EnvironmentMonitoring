#ifndef __GPS_STA_H_
#define __GPS_STA_H_	 
#include "sys.h"

#define GPS_PV_RCC         RCC_APB2Periph_GPIOB

#define GPS_PV_           GPIOB                   //PV
#define GPS_PV_PIN        GPIO_Pin_7


#define GPS_PV          GPIO_ReadInputDataBit(GPS_PV_ , GPS_PV_PIN)


#define GPS_COMM_CONFIG_Nuber        6                   //GPS����ָ�����

extern const u8 *GPS_REST_CMM           ;               //GPS�ָ��ɳ�����������
extern const u8 *GPS_TIME_CMM           ;               //GPS�������GPSʱ������
extern const u8 *GPS_UNLOGALL_CMM       ;               //GPS���ùر����д��ڴ�ӡ���
extern const u8 *GPS_REST_CMM           ;               //GPS��λ����
extern const u8 *GPS_SAVE_CMM           ;               //����GPS����
extern const u8 *GPS_CMM_5HZ[GPS_COMM_CONFIG_Nuber] ;   // 5 HZ  ������� 



void GPS_PV_Init(void);

u8 GPS_CONFIG_RESET(void) ;    //GPS��λ   ���� 0�ɹ�   1ʧ��
u8 GPS_CONFIG_5HZ(void) ;      //���ó� 5HZ   ����0�ɹ�   1ʧ��
u8 GPS_CONFIG_TIMEA(void) ;    //�������ʱ������ 0�ɹ�    1ʧ��
u8 GPS_UNLOGALL_CONFIG(void) ; //GPS�ر��������   0�ɹ�    1ʧ��
u8 GPS_SAVE_CONFIG(void)  ;    //��������				    
#endif
