#include "USART1_DMA.h"
#include "ff.h"
#include "exfuns.h"
#include "stm32f10x_it.h"
#include "Led.h"
#include "SRAM_DATA.h"
#include "PLAY_DATA.H"
#include "GPS_DATE.h"
#include "Gps.h"
#include "SRAM_DATA.h"
#include "PLAY_DATA.H"




void USART1_RXD_DMA_CONFIG(void)
{
	DMA_InitTypeDef   DMA_InitStructure ;   //DMA���ýṹ��  ȫ�ֱ���
	NVIC_InitTypeDef  NVIC_InitStruct ;
  //������DMA����    
  //����DMAʱ��  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
  //DMA1ͨ��5����  
  DMA_DeInit(DMA1_Channel5);  
 //�����ַ  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  
 //�ڴ��ַ  
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;  
 //dma���䷽����  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
 //����DMA�ڴ���ʱ�������ĳ���  
  DMA_InitStructure.DMA_BufferSize = 0;  
 //����DMA�������ַ������
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
 //����DMA���ڴ����ģʽ  
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
 //���������ֳ�  
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
 //�ڴ������ֳ�  
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
 //����DMA�Ĵ���ģʽ  
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;    //��Ϊ�ػ�ģʽ
 //����DMA�����ȼ���  
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
 //����DMA��2��memory�еı����������  
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   //�Ǵ�������������ģʽ
  DMA_Init(DMA1_Channel5,&DMA_InitStructure);  
	
	//����DMA��ʽ����  
  USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);            //��������DMA����ģʽ
  DMA_ClearFlag(DMA1_FLAG_TC5);                           //���ͨ��5������ɱ�־
	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC|DMA_IT_TE,ENABLE); //����������� �ж�   ��������ж�
	NVIC_InitStruct.NVIC_IRQChannel  = DMA1_Channel5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd =   ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  //������ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority  =  0 ;
 	NVIC_Init(&NVIC_InitStruct);//DMA�ж�����
	
//�ر�ͨ��5  
  DMA_Cmd(DMA1_Channel5,DISABLE);  
}
void DMA_FIFO_CONFIG(u32 MemoryBaseAddr,u16 LEN)  //���ô洢����ַ ��DMA ����  ����DMA
{
	DMA_Cmd(DMA1_Channel5,DISABLE);      //�ر�DMA

  DMA1_Channel5->CMAR   =  MemoryBaseAddr ;   //����װ�ش洢����ַ
  DMA1_Channel5->CNDTR  =  LEN ;  	          //װ�ػ��泤�� 
	
 	DMA_Cmd(DMA1_Channel5,ENABLE);      //����DMA
}


void USART1_RXD_DMA_Init(void)
{
	RX_DMA_STA = 0 ;  //����
	SET_RX_DMA_BUF0 ;  //���û��� 0
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);                        //�رս����ж�
	FIFO_CLR_BUF(RX_BUF_FIFO);                                             //���FIFO
	
  USART1_RXD_DMA_CONFIG() ;                                              //��ʼ�� ����1 DMA ��������   ���δ���  ��������ж�
  DMA_FIFO_CONFIG((u32)RX_DMA_BUFF0 ,DMA_BUFF_SIZE) ;                    //���ô洢����ַ ��DMA ����  ����DMA
}


void USART1_IRQHandler(void)   
{
	u8 com_data =0;
	//�����ж� (���ռĴ����ǿ�)----------------------------------------
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
	 com_data= USART1->DR;
	
//UTCʱ��У׼����------------------------------------------
  if(RTC_CONFIG_FLAT)	 //��ҪУ׼		     
	{
    GPS_BUF_TIME_Adjust_Task(GPS_BUF,com_data);   //����GPSʱ��У׼����		
		if(GPS_BUF_TIME_Adjust(GPS_BUF)==GPS_FREE)  //ʱ�����	 
		{
			RTC_Set(GPS_TIME->syear,GPS_TIME->smon,GPS_TIME->sday,GPS_TIME->hour,GPS_TIME->min,GPS_TIME->sec);  //����RTCʱ��
	    RTC_CONFIG_Nuber_ADD;        //У׼������һ		
		}
	}
	else
	{
	//д��FIFO------------------------------------------------
		FIFO_WRITE_Byte(RX_BUF_FIFO,com_data);  //д��FIFO
	}
 }
}




void DMA1_Channel5_IRQHandler(void)    //����DMA��������ж�
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC5)!=RESET)	//�ж�ͨ��5�������
	{
		DMA_ClearFlag(DMA1_FLAG_TC5|DMA1_FLAG_HT5);                               //���ͨ��5������ɱ�־ ������������
		if( RX_DMA_FLAT == 0 )   //���֮ǰ������ٶȴ�������
		{
     SET_RX_DMA_UP ;          //���û��������ɱ��
		 if( RX_DMA_Nuber == 0 )  //���֮ǰ������ǵ�0������
		 { 
			SET_RX_DMA_BUF1 ;   //ָʾ��һ�����濪ʼ����
		  DMA_FIFO_CONFIG((u32)RX_DMA_BUFF1 ,DMA_BUFF_SIZE) ;   //���ô洢����ַ ��DMA ����  ����DMA
		 }
		 else
		 {
			SET_RX_DMA_BUF0 ;   //ָʾ��0�����濪ʼ����
		  DMA_FIFO_CONFIG((u32)RX_DMA_BUFF0 ,DMA_BUFF_SIZE) ;   //���ô洢����ַ ��DMA ����  ����DMA		 
		 }
	  }
	  else   //DMA�Ѿ�������� �����ϴλ������ݻ�û������
	  {
	    Config_Counter ++;        //����SD��д�ٶȹ������   ���ݶ�ʧ
		 if( RX_DMA_Nuber == 0 )    //���֮ǰ������ǵ�0������
		 { 
			SET_RX_DMA_BUF1 ;         //ָʾ��һ�����濪ʼ����
		  DMA_FIFO_CONFIG((u32)RX_DMA_BUFF1 ,DMA_BUFF_SIZE) ;   //���ô洢����ַ ��DMA ����  ����DMA
		 }
		 else
		 {
			SET_RX_DMA_BUF0 ;         //ָʾ��0�����濪ʼ����
		  DMA_FIFO_CONFIG((u32)RX_DMA_BUFF0 ,DMA_BUFF_SIZE) ;   //���ô洢����ַ ��DMA ����  ����DMA		 
		 }
	  }
	}

	if(DMA_GetFlagStatus(DMA1_FLAG_TE5)!=RESET)   //��������ж�
	{
	  DMA_ClearFlag(DMA1_FLAG_TE5);           
	  Config_Counter +=20000 ;
	}
}



u8 DATA_CNT=0;
u8 DISTANCE_BUF[21]={0};
u8 DISTANCE_BUF_TO_SD=0;
u8 buff_len=0;
void USART2_IRQHandler(void)   
{
	u8 juli_data=0;
	//�����ж� (���ռĴ����ǿ�)----------------------------------------
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
	   juli_data= USART2->DR;
		 DISTANCE_BUF[DATA_CNT++]= juli_data;
		
		 if(DISTANCE_BUF[DATA_CNT-1]==0x17)
		 {
			  
		   if(DISTANCE_BUF[DATA_CNT-2]==0x07)//����յ�0x6d,0x0a.�ͽ����ݴ浽sd��
			 {
				 DATA_CNT=0;
			   DISTANCE_BUF_TO_SD=1;
//				 buff_len=DATA_CNT-1;
			 }
		  	
			
		 }
		
		
  }
}


