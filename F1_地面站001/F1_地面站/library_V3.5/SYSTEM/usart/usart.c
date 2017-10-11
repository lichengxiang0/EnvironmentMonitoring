#include "usart.h"	
#include "sys.h"

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
  
void Uart1_Init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 //USART1_TX   PA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
 //USART1_RX	  PA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  //Usart1 NVIC 配置

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
 //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
  USART_Cmd(USART1, ENABLE);                    //使能串口 
}
void Uart2_Init(u32 bound)
{
 //GPIO端口设置
 GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//使能USART1，GPIOA时钟
   //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
	// RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
//USART1_TX   PA.2
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
 GPIO_Init(GPIOA, &GPIO_InitStructure);
  
//USART1_RX	  PA.3
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
 GPIO_Init(GPIOA, &GPIO_InitStructure);  

 //Usart1 NVIC 配置

 NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
 
//USART 初始化设置
USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

 USART_Init(USART2, &USART_InitStructure); //初始化串口
// USART_ITConfig(USART2, USART_IT_TC, DISABLE);//开启中断
 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
 USART_Cmd(USART2, ENABLE);                    //使能串口 

//GPIO_InitTypeDef GPIO_InitStructure;
//USART_InitTypeDef USART_InitStructure;
//NVIC_InitTypeDef NVIC_InitStructure;
//
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
//
////串口2对应引脚复用映射
////GPIO_PinRemapConfig(GPIO_Remap_USART2,GPIOA,GPIO_PinSource2,); //GPIOA2复用为USART2
////GPIO_PinRemapConfig(GPIOA,GPIO_PinSource3,GPIO_Remap_USART2); //GPIOA3复用为USART2
//
////USART1端口配置
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA9与GPIOA10
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用功能
//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
////GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
////GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
//GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA2，PA3
//
//  //USART2 初始化设置
//USART_InitStructure.USART_BaudRate = bound;//波特率设置
//USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
// USART_Init(USART2, &USART_InitStructure); //初始化串口1
//
// USART_Cmd(USART2, ENABLE);  //使能串口2
//
//USART_ClearFlag(USART2, USART_FLAG_TC|USART_FLAG_RXNE);   //清除发送接收完成中断
//
//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启接收中断
//
////Usart2 NVIC 配置
// NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级2
//NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级2
//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}


void USART1_SEND_Byte(u8 data)
{
	while((USART1->SR&0X40)==0) {} ;//等待发送完成  
   USART1->DR = (u8) data;
}

void Uart1_send_str(u8 * buf,u16 len)
{
  while(len--)
	{
	   USART1_SEND_Byte(*buf);
		 buf++;	 
	}
}

void USART2_SEND_Byte(u8 data)
{
	while((USART2->SR&0X40)==0) {} ;//等待发送完成  
   USART2->DR = (u8) data;
}

void Uart2_send_str(u8 * buf,u16 len)
{
  while(len--)
	{
	   USART2_SEND_Byte(*buf);
		 buf++;	 
	}
}

