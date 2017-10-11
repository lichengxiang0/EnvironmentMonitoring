#include "Led.h"


/**************************实现函数********************************************
*函数原型:	   	void LED_Init(void)
*功　　能:	   	LED初始化，PD2，低电平点亮
*输入参数：  		无
*输出参数：  		无
*******************************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //使能端口时钟

	GPIO_InitStructure.GPIO_Pin = LED0_PIN;	    		 //LED0端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(LED0_, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = LED1_PIN;	    		 //LED1端口配置
	GPIO_Init(LED1_, &GPIO_InitStructure);	
	

	LED0_OUT(1);    //关闭
	LED1_OUT(1);    //关闭
}
