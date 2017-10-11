#include"TIM2.H"
void TIM2_config()
{TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//定义时钟的初始化结构体
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//打开定时器2的外设时钟
//TIM_DeInit(TIM2); //重新将Timer设置为缺省值
TIM_InternalClockConfig(TIM2);//采用内部时钟给TIM2提供时钟源
TIM_TimeBaseStructure.TIM_Prescaler = 36000-1;//预分频系数为36000-1，这样计数器时钟为72MHz/36000 = 2kHz,T=0.5ms
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置时钟分割为1
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//设置计数器模式为向上计数模式
TIM_TimeBaseStructure.TIM_Period = 2000 - 1;//设置计数溢出大小，每计2000个数就产生一个更新事件，2000*0.5ms=1000ms=1s
TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure); //将配置应用到TIM2中
TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除溢出中断标志
TIM_ARRPreloadConfig(TIM2, DISABLE);//禁止ARR预装载缓冲器
TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//开启TIM2的中断
}

 
void TIM2NVIC_config()
{
NVIC_InitTypeDef NVIC_InitStructure;
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//选择中断分组1
NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//选择TIM2的中断通道      
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占式中断优先级设置为1
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//响应式中断优先级设置为2
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能中断
NVIC_Init(&NVIC_InitStructure);
}

//TIM_Cmd(TIM2,ENABLE);//开启定时器2
/*
void TIM2_IRQHandler(void)
{
if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//检测是否发生溢出更新事件
{
TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);//清除TIM2的中断待处理位
}
//自己需添加的代码
}
*/

