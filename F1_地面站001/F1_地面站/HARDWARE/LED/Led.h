#ifndef _LED_H_
#define _LED_H_
#include "stm32f10x.h"

#define LED_BUS_RCC       RCC_APB2Periph_GPIOB

#define LED0_              GPIOB                   //
#define LED0_PIN           GPIO_Pin_5

#define LED1_              GPIOB                  //
#define LED1_PIN           GPIO_Pin_6

#define LED0_OUT(x)         x? GPIO_SetBits(LED0_ , LED0_PIN) : GPIO_ResetBits(LED0_ , LED0_PIN)

#define LED1_OUT(x)         x? GPIO_SetBits(LED1_ , LED1_PIN) : GPIO_ResetBits(LED1_ , LED1_PIN)


#define LED0   PBout(5)		//	绿色    快门
#define LED1   PBout(6)		//	蓝色    GPS数据写入SD卡       用于检测故障

#define LED0_OFF   LED0 = 1
#define LED0_ON    LED0 = 0


#define LED1_OFF   LED1 = 1
#define LED1_ON    LED1 = 0

void LED_Init(void);

#endif
