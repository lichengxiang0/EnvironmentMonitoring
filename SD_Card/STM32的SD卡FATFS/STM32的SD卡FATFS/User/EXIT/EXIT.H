#ifndef _EXIT_H_
#define _EXIT_H_
#include"config.h"
//初始化EXIT之前需把GPIO初始化为输入模式GPIOinput_Init();
void AFIOexit_Init(void);
void EXIT_Init(void);
void NVICexit_Init(void);
#endif
/*void GPIOinput_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;//定义端口初始化所用的结构体变量
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//启动GPIOC的时钟
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;//选择0口(GPIO_Pin_0;//选择0口)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//设置端口为上拉输入模式(GPIO_Mode_IN_FLOATING; //选择输入模式为浮空输入)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置端口速度为50MHZ，在把端口设置为输入模式是实际上未用到此参数
  GPIO_Init(GPIOC, &GPIO_InitStructure);//按以上设置进行初始化端口
}

void EXTI0_IRQHandler(void)//0引脚为外部中断0线，{void EXTI9_5_IRQHandler(void)}
{
   //点亮LED灯
   //GPIO_SetBits(GPIOA,GPIO_Pin_0);
   //清除中断标志位，防止持续进入中断
     EXTI_ClearITPendingBit(EXTI_Line0);//EXTI_ClearITPendingBit(EXTI_Line0);
}
*/
