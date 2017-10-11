#include "GPIO.H"
void GPIOinput_init(void)
{RCC->APB2ENR|=(1<<(2+(0))); //开启GPIOA时钟
GPIOB->CRL&=~(0xF<<(4*(0))); //A0口模式清零
GPIOB->CRL|=(0x8<<(4*(0))); //A0口上下拉输入
GPIOB->ODR|=(1<<(0));//A0口上拉输入
}
//PIN=(((GPIOB->IDR)>>0)&1);//获取输入的数据
void GPIOoutput_init()
{RCC->APB2ENR|=(1<<(2+(0)));//打开GPIOA口的RCC时钟
GPIOA->CRH&=~(0xF<<(4*(0)));//清零GPIOA8的模式寄存器
GPIOA->CRH|=(0x1<<(4*(0)));//设置GPIOA8为10MHz推挽输出
}
//GPIOA->ODR|=(1<<8);//使GPIOA8输出高电平
//GPIOA->BSRR=1<<8;//置位GPIOA8
//GPIOA->BRR=1<<8;//复位GPIOA8 

void GPIOoutput_config(void)
{
 GPIO_InitTypeDef GPIO_InitStructure;//定义端口初始化所用的结构体变量 
  /* 激活GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//启动GPIOA的时钟
  /* Configure PA.00, PA.01 as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//选择8口
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置端口速度为50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//设置端口为推挽输出模式
  GPIO_Init(GPIOA, &GPIO_InitStructure);//按以上设置进行初始化A端口
}
//GPIO_SetBits(GPIOA, GPIO_Pin_8);//PA.8输出高电平
//GPIO_ResetBits(GPIOA, GPIO_Pin_8);//PA.8输出低电平

void GPIOinput_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;//定义端口初始化所用的结构体变量
  // Enable GPIOD clock //
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//启动GPIOB的时钟
  // Configure PB0 as intput//
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;//选择0口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//设置端口为上拉输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置端口速度为50MHZ，在把端口设置为输入模式是实际上未用到此参数
  GPIO_Init(GPIOB, &GPIO_InitStructure);//按以上设置进行初始化端口
}
//GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);//读取GPIOB8位输入值
