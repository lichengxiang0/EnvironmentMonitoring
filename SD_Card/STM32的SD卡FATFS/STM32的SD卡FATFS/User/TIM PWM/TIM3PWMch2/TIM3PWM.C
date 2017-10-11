#include"TIM3PWM.H"
void PWM3IOA_config()
{
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);//开启GPIOA的时钟和复用功能
//GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);//部分映射，将TIM3_CH2映射到PB5

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//选择引脚7                                                      
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//输出频率最大50MHz
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出 
GPIO_Init(GPIOA,&GPIO_InitStructure);
}

 
void PWM3TIM3_config()
{
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//开启TIM3的时钟
TIM_DeInit(TIM3);//重新将Timer3设置为缺省值
TIM_InternalClockConfig(TIM3);//采用内部时钟给TIM3提供时钟源
TIM_TimeBaseStructure.TIM_Prescaler =7200-1;//预分频系数为7200，7200预分频，此时TIMER的频率为10kHz
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置时钟分割
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//设置计数器模式为向上计数模式
TIM_TimeBaseStructure.TIM_Period = 10000-1;//设置计数溢出大小，每计10000个数就产生一个更新事件，即PWM的输出频率为1Hz
TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//将配置应用到TIM3中
}

 
void PWM3_config(int dutyfactor)
{
TIM_OCInitTypeDef TimOCInitStructure;
TIM_OCStructInit(&TimOCInitStructure);//设置缺省值
TimOCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM模式1输出
TimOCInitStructure.TIM_Pulse = dutyfactor * 10000 / 100;//设置占空比，占空比=(CCRx/ARR)*100%或(TIM_Pulse/TIM_Period)*100%
TimOCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//TIM输出比较极性高
TimOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//使能输出状态
TIM_OC2Init(TIM3, &TimOCInitStructure);//TIM3的CH2输出
TIM_CtrlPWMOutputs(TIM3,ENABLE);//设置TIM3的PWM输出为使能
}


//TIM_Cmd(TIM3, ENABLE);//使能TIM3计时器，开始输出PWM

