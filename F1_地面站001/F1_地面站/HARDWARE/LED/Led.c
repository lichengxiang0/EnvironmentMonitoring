#include "Led.h"


/**************************ʵ�ֺ���********************************************
*����ԭ��:	   	void LED_Init(void)
*��������:	   	LED��ʼ����PD2���͵�ƽ����
*���������  		��
*���������  		��
*******************************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //ʹ�ܶ˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = LED0_PIN;	    		 //LED0�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(LED0_, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = LED1_PIN;	    		 //LED1�˿�����
	GPIO_Init(LED1_, &GPIO_InitStructure);	
	

	LED0_OUT(1);    //�ر�
	LED1_OUT(1);    //�ر�
}
