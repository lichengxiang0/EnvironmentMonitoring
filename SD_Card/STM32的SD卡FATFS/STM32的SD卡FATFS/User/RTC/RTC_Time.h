/****************************************Copyright (c)****************************************************
** File name:               RTC_Time.h
** Descriptions:            None
** Created by:            
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
*********************************************************************************************************/

#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

/* Private function prototypes -----------------------------------------------*/
void RTC_Init(void);
void Time_Regulate(void);


#endif

//RTC_Init();//RTC初始化  在main函数中初始化RTC即可

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
