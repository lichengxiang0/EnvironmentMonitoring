#include"EXIT.H"
void AFIOexit_Init(void)
{ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//启动AFIO的时钟
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);//选择中断管脚PC0(GPIO_PinSource0;//选择中断管脚PC0)
}


void EXIT_Init(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;//定义外部中断初始化参数的结构体
  EXTI_ClearITPendingBit(EXTI_Line0);//清除外部中断挂起标志(EXTI_Line0;//清除外部中断挂起标志0)
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;   //选择中断线路0(EXTI_Line0;   //选择中断线路0)
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//设置为中断请求，非事件请求
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//设置中断触发方式为下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; //外部中断使能
  EXTI_Init(&EXTI_InitStructure);//初始化外部中断
}


void NVICexit_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;//定义NVIC的结构体变量
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//选择中断分组1
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//选择中断通道0（EXTI0_IRQn;  //选择中断通道0）
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;//抢占式中断优先级设置为0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;//响应式中断优先级设置为1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能中断
  NVIC_Init(&NVIC_InitStructure);
}
