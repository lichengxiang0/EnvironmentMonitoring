#include "usart.h"	
#include "sys.h"

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
  
void Uart1_Init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 //USART1_TX   PA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
 //USART1_RX	  PA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  //Usart1 NVIC ����

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
 //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
}
void Uart2_Init(u32 bound)
{
 //GPIO�˿�����
 GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART1��GPIOAʱ��
   //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
	// RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
//USART1_TX   PA.2
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
 GPIO_Init(GPIOA, &GPIO_InitStructure);
  
//USART1_RX	  PA.3
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
 GPIO_Init(GPIOA, &GPIO_InitStructure);  

 //Usart1 NVIC ����

 NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
 
//USART ��ʼ������
USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

 USART_Init(USART2, &USART_InitStructure); //��ʼ������
// USART_ITConfig(USART2, USART_IT_TC, DISABLE);//�����ж�
 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
 USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

//GPIO_InitTypeDef GPIO_InitStructure;
//USART_InitTypeDef USART_InitStructure;
//NVIC_InitTypeDef NVIC_InitStructure;
//
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
//
////����2��Ӧ���Ÿ���ӳ��
////GPIO_PinRemapConfig(GPIO_Remap_USART2,GPIOA,GPIO_PinSource2,); //GPIOA2����ΪUSART2
////GPIO_PinRemapConfig(GPIOA,GPIO_PinSource3,GPIO_Remap_USART2); //GPIOA3����ΪUSART2
//
////USART1�˿�����
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA9��GPIOA10
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//���ù���
//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
////GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
////GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
//GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2��PA3
//
//  //USART2 ��ʼ������
//USART_InitStructure.USART_BaudRate = bound;//����������
//USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
//USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
// USART_Init(USART2, &USART_InitStructure); //��ʼ������1
//
// USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
//
//USART_ClearFlag(USART2, USART_FLAG_TC|USART_FLAG_RXNE);   //������ͽ�������ж�
//
//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���������ж�
//
////Usart2 NVIC ����
// NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����2�ж�ͨ��
//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�2
//NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�2
//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}


void USART1_SEND_Byte(u8 data)
{
	while((USART1->SR&0X40)==0) {} ;//�ȴ��������  
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
	while((USART2->SR&0X40)==0) {} ;//�ȴ��������  
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

