#include "Exit.h"
#include "delay.h"
#include "LED.h"
#include "RTC.H"
#include "Cam_FIFO.h"
#include "SRAM_DATA.h"
#include "soft_iic.h"
//#incldue "stm32_bsp_time.h"

/**************************ʵ�ֺ���********************************************
*����ԭ��:	   	void Exit_Init(void)
*��������:	   	���հ��������жϳ�ʼ��
*���������  		��
*���������  		��
*******************************************************************************/
void Exit_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);//ʹ��PORTAʱ��,ʹ�ܸ��ù���ʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PB1
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(KMM_BUS,KMM_PIN) ; //����
	
	//GPIOB.1 �ж����Լ��жϳ�ʼ������
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	EXTI_ClearITPendingBit(EXTI_Line1); //���LINE1�ϵ��жϱ�־λ 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;	//��ռ���ȼ�1�� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:	   	void SD_SACN_Init(void)
*��������:	   	SD���������ʼ��
*���������  		��
*���������  		��
*******************************************************************************/
void SD_SACN_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTAʱ��,ʹ�ܸ��ù���ʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  GPIO_SetBits(GPIOA,GPIO_Pin_0);   //����
}

u16 Calendar_LED = 0;   //����ָʾ��
void Calendar_LED_TAsk(void)    //����ָʾ������
{
  if(Cal_LED_FLAT)
	{ 
	   CRL_Cal_LED;  //�������ָʾ���
		 SET_Cal_T;    //װ��ʱ��
		 SET_Cal_RUN;  //�������б��
	}
 
	if(Cal_RUN_FLAT)
	{
	 if(Cal_LED_NUM)
	 {
		 Cal_LED_DEC;	  //��������һ
		 LED0_ON;       //��������ָʾ��
	 }
	 else
	 {
		 LED0_OFF;          //�رտ���ָʾ��
	   Calendar_LED = 0;  //������б��
	 }
	}
}

void TIM2_IRQHandler(void)   //1MS �ж�һ��
{
	if(TIM2->SR & TIM_IT_Update)		
	{     
		TIM2->SR = ~TIM_FLAG_Update;
		#if Cal_LED_task_EN     //��������˿���ָʾ������
		   Calendar_LED_TAsk();	//����ָʾ������
    #endif
	}
}

_calendar_obj  RETURN_OneSelf_TYP(_calendar_obj *data)
{
  return   *data ;
}


#define CAMM_IO_T_Nuber    5     //ɨ������  ��λ ms

u8  CAMM_IO_TIME_Nuber = 0;      //���������б�״̬ ����
_calendar_obj Cam_Save_time;	   //����ʱ���¼����


void EXTI1_IRQHandler(void)
{
 _calendar_obj Cam_Save_TIME;  	//����ʱ����ʱ��¼����
 Cal_RTC_Get(&Cam_Save_TIME);   //��ȡ����ʱ��   ��¼
	if(KMM == 0)
	{
   if(CAMM_IO_TIME_Nuber == 0) //֮ǰ���Ű��µ���ʱ�������
	 {
	   Cam_Save_time = RETURN_OneSelf_TYP(&Cam_Save_TIME) ;   //���浱ǰ��¼��ʱ������
	   CAMM_IO_TIME_Nuber =  CAMM_IO_T_Nuber ;   //����ɨ��ʱ��
	   TIM4->CNT = 0 ;   //�����ʱ�� 4����  
	 }
  }
	
	EXTI_ClearITPendingBit(EXTI_Line1); //���LINE1�ϵ��жϱ�־λ 
}

u8 ICM20602_TIM4_FLAG=0;
u16 ICM20602_CNT=0;
u16  JG_CEJU_CNT=0;
u8 Jg_CeJu_Flag=0;
/**************************ʵ�ֺ���********************************************
*����ԭ��:	   	void TIM4_IRQHandler(void)
*��������:	   	��������״̬��ʱ�б���������
*���������  		��
*���������  		��
*******************************************************************************/
void TIM4_IRQHandler(void)   // 1 MS�ж�һ��
{
 if(TIM4->SR & TIM_IT_Update)		
 {     
	TIM4->SR = ~TIM_FLAG_Update;
	   HAL_IncTick();
	 JG_CEJU_CNT++;
	 if(JG_CEJU_CNT==1000)
	 {
	   JG_CEJU_CNT=0;
		 Jg_CeJu_Flag=1;
	 }
	 
	  ICM20602_CNT++;
	 if(ICM20602_CNT==9000)
	 {
		 ICM20602_CNT=0;
	   ICM20602_TIM4_FLAG=1;
	 }
//if((CAMM_IO_TIME_Nuber > 0)&&( KMM == 0 ))  //�������Ҫɨ�� ��Ϊ��Ч��ƽ
//{
// CAMM_IO_TIME_Nuber --;       //ɨ�������һ
// if(CAMM_IO_TIME_Nuber == 0)  //���ɨ������   ����ΪΪ��Ч����  д��FIFO
// {
//	if(Cam_FIFO_WRITE(TIME_FIFO,&Cam_Save_time)==Cam_FIFO_OK)   //ʱ��д�� FIFO  
//  {
//   #if Cal_LED_task_EN  //��������˿���ָʾ������
//	   SET_Cal_LED;   //���ÿ���ָʾ��������
//   #else
//		 LED0_ON;       //��������ָʾ�� 	
//   #endif
//   }					
//  }	
// }
// else  //����
// {
//	CAMM_IO_TIME_Nuber = 0;
// }
 }
}

