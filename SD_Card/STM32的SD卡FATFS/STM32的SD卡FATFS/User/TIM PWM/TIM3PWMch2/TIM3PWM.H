#ifndef _TIM3PWM_H_
#define _TIM3PWM_H_
#include"config.h"
void PWM3IOA_config(void); 
void PWM3TIM3_config(void);
void PWM3_config(int dutyfactor);//占空比的值50为50%
#endif
//TIM_Cmd(TIM3, ENABLE);//使能TIM3计时器，开始输出PWM
