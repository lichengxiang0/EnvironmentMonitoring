#ifndef _EXIT_H_
#define _EXIT_H_
#include"config.h"
void IWDG_Configuration(void);//函数声明
#endif
  /*if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST)!=RESET)//查看是否是看门狗复位
  {
  IWDG_ReloadCounter(); //手动喂狗
  //RCC_ClearFlag();//清除复位标志
   }*/
