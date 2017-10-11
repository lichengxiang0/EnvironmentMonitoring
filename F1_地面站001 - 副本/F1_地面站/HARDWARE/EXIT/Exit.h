#ifndef __EXIT_H_
#define __EXIT_H_	 
#include "sys.h"


#define  KMM_BUS     GPIOB
#define  KMM_PIN     GPIO_Pin_1

#define  KMM         GPIO_ReadInputDataBit(KMM_BUS,KMM_PIN)


#define SD_SACN      GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)


//����ָʾ��--------------------------------------------------------------------------------------
#define  Cal_LED_task_EN      1                   //����ָʾ������ʹ��   1����  0�ر�

//extern u16 Calendar_LED ;   //����ָʾ��

#define Cal_LED_T         100                      //����LEDָʾʱ��    ��λ�� MS    ��� 16383 MS

#define Cal_LED_FLAT     (Calendar_LED&0x8000)    //����ָʾ�Ʊ��
#define SET_Cal_LED       Calendar_LED |= 0x8000  //���ñ��
#define CRL_Cal_LED       Calendar_LED &=~0x8000  //������

#define Cal_RUN_FLAT     (Calendar_LED&0x4000)  
#define SET_Cal_RUN       Calendar_LED |= 0x4000  //���ñ��
#define CRL_Cal_RUN       Calendar_LED &=~0x4000  //������

#define SET_Cal_T         Calendar_LED = (Calendar_LED&(~0x3fff))|(Cal_LED_T&0x3fff)      //װ�ؿ���ָʾ��ʱ�����

#define Cal_LED_NUM      (Calendar_LED&0x3fff)   //ʣ�����
#define Cal_LED_DEC       Calendar_LED --       //ʱ����ļ�һ


void Exit_Init(void);
void SD_SACN_Init(void);




#endif
