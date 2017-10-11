#ifndef __EXIT_H_
#define __EXIT_H_	 
#include "sys.h"


#define  KMM_BUS     GPIOB
#define  KMM_PIN     GPIO_Pin_1

#define  KMM         GPIO_ReadInputDataBit(KMM_BUS,KMM_PIN)


#define SD_SACN      GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)


//快门指示灯--------------------------------------------------------------------------------------
#define  Cal_LED_task_EN      1                   //快门指示灯任务使能   1开启  0关闭

//extern u16 Calendar_LED ;   //快门指示灯

#define Cal_LED_T         100                      //快门LED指示时间    单位： MS    最大 16383 MS

#define Cal_LED_FLAT     (Calendar_LED&0x8000)    //快门指示灯标记
#define SET_Cal_LED       Calendar_LED |= 0x8000  //设置标记
#define CRL_Cal_LED       Calendar_LED &=~0x8000  //清除标记

#define Cal_RUN_FLAT     (Calendar_LED&0x4000)  
#define SET_Cal_RUN       Calendar_LED |= 0x4000  //设置标记
#define CRL_Cal_RUN       Calendar_LED &=~0x4000  //清除标记

#define SET_Cal_T         Calendar_LED = (Calendar_LED&(~0x3fff))|(Cal_LED_T&0x3fff)      //装载快门指示灯时间节拍

#define Cal_LED_NUM      (Calendar_LED&0x3fff)   //剩余节拍
#define Cal_LED_DEC       Calendar_LED --       //时间节拍减一


void Exit_Init(void);
void SD_SACN_Init(void);




#endif
