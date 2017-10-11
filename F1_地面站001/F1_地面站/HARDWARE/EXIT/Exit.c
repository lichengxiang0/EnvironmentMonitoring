#include "Exit.h"
#include "delay.h"
#include "LED.h"
#include "RTC.H"
#include "Cam_FIFO.h"
#include "SRAM_DATA.h"
#include "soft_iic.h"
//#incldue "stm32_bsp_time.h"

/**************************实现函数********************************************
*函数原型:	   	void Exit_Init(void)
*功　　能:	   	拍照按键触发中断初始化
*输入参数：  		无
*输出参数：  		无
*******************************************************************************/
void Exit_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);//使能PORTA时钟,使能复用功能时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PB1
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(KMM_BUS,KMM_PIN) ; //上拉
	
	//GPIOB.1 中断线以及中断初始化配置
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE1上的中断标志位 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;	//抢占优先级1， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
}

/**************************实现函数********************************************
*函数原型:	   	void SD_SACN_Init(void)
*功　　能:	   	SD卡插入检查初始化
*输入参数：  		无
*输出参数：  		无
*******************************************************************************/
void SD_SACN_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA时钟,使能复用功能时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  GPIO_SetBits(GPIOA,GPIO_Pin_0);   //上拉
}

u16 Calendar_LED = 0;   //快门指示灯
void Calendar_LED_TAsk(void)    //快门指示灯任务
{
  if(Cal_LED_FLAT)
	{ 
	   CRL_Cal_LED;  //清除快门指示标记
		 SET_Cal_T;    //装载时间
		 SET_Cal_RUN;  //设置运行标记
	}
 
	if(Cal_RUN_FLAT)
	{
	 if(Cal_LED_NUM)
	 {
		 Cal_LED_DEC;	  //节拍数减一
		 LED0_ON;       //点亮快门指示灯
	 }
	 else
	 {
		 LED0_OFF;          //关闭开门指示灯
	   Calendar_LED = 0;  //清除所有标记
	 }
	}
}

void TIM2_IRQHandler(void)   //1MS 中断一次
{
	if(TIM2->SR & TIM_IT_Update)		
	{     
		TIM2->SR = ~TIM_FLAG_Update;
		#if Cal_LED_task_EN     //如果开启了快门指示灯任务
		   Calendar_LED_TAsk();	//快门指示灯任务
    #endif
	}
}

_calendar_obj  RETURN_OneSelf_TYP(_calendar_obj *data)
{
  return   *data ;
}


#define CAMM_IO_T_Nuber    5     //扫描周期  单位 ms

u8  CAMM_IO_TIME_Nuber = 0;      //快门引脚判别状态 变量
_calendar_obj Cam_Save_time;	   //航拍时间记录变量


void EXTI1_IRQHandler(void)
{
 _calendar_obj Cam_Save_TIME;  	//航拍时间临时记录变量
 Cal_RTC_Get(&Cam_Save_TIME);   //获取航拍时间   记录
	if(KMM == 0)
	{
   if(CAMM_IO_TIME_Nuber == 0) //之前快门按下的延时检测完了
	 {
	   Cam_Save_time = RETURN_OneSelf_TYP(&Cam_Save_TIME) ;   //保存当前记录的时间数据
	   CAMM_IO_TIME_Nuber =  CAMM_IO_T_Nuber ;   //设置扫描时间
	   TIM4->CNT = 0 ;   //清除定时器 4计数  
	 }
  }
	
	EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE1上的中断标志位 
}

u8 ICM20602_TIM4_FLAG=0;
u16 ICM20602_CNT=0;
u16  JG_CEJU_CNT=0;
u8 Jg_CeJu_Flag=0;
/**************************实现函数********************************************
*函数原型:	   	void TIM4_IRQHandler(void)
*功　　能:	   	拍照引脚状态延时判别，消除干扰
*输入参数：  		无
*输出参数：  		无
*******************************************************************************/
void TIM4_IRQHandler(void)   // 1 MS中断一次
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
//if((CAMM_IO_TIME_Nuber > 0)&&( KMM == 0 ))  //如果有需要扫描 且为有效电平
//{
// CAMM_IO_TIME_Nuber --;       //扫描次数减一
// if(CAMM_IO_TIME_Nuber == 0)  //如果扫描完了   则认为为有效数据  写入FIFO
// {
//	if(Cam_FIFO_WRITE(TIME_FIFO,&Cam_Save_time)==Cam_FIFO_OK)   //时间写入 FIFO  
//  {
//   #if Cal_LED_task_EN  //如果开启了快门指示灯任务
//	   SET_Cal_LED;   //设置快门指示灯任务标记
//   #else
//		 LED0_ON;       //点亮快门指示灯 	
//   #endif
//   }					
//  }	
// }
// else  //干扰
// {
//	CAMM_IO_TIME_Nuber = 0;
// }
 }
}

