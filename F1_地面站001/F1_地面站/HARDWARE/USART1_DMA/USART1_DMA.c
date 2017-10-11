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
	DMA_InitTypeDef   DMA_InitStructure ;   //DMA配置结构体  全局变量
	NVIC_InitTypeDef  NVIC_InitStruct ;
  //串口收DMA配置    
  //启动DMA时钟  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
  //DMA1通道5配置  
  DMA_DeInit(DMA1_Channel5);  
 //外设地址  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);  
 //内存地址  
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;  
 //dma传输方向单向  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
 //设置DMA在传输时缓冲区的长度  
  DMA_InitStructure.DMA_BufferSize = 0;  
 //设置DMA的外设地址不递增
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
 //设置DMA的内存递增模式  
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
 //外设数据字长  
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
 //内存数据字长  
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
 //设置DMA的传输模式  
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;    //不为回环模式
 //设置DMA的优先级别  
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
 //设置DMA的2个memory中的变量互相访问  
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   //非储存器到储存器模式
  DMA_Init(DMA1_Channel5,&DMA_InitStructure);  
	
	//采用DMA方式接收  
  USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);            //开启串口DMA接收模式
  DMA_ClearFlag(DMA1_FLAG_TC5);                           //清除通道5传输完成标志
	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC|DMA_IT_TE,ENABLE); //开启传输完成 中断   传输错误中断
	NVIC_InitStruct.NVIC_IRQChannel  = DMA1_Channel5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd =   ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  //最高优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority  =  0 ;
 	NVIC_Init(&NVIC_InitStruct);//DMA中断配置
	
//关闭通道5  
  DMA_Cmd(DMA1_Channel5,DISABLE);  
}
void DMA_FIFO_CONFIG(u32 MemoryBaseAddr,u16 LEN)  //设置存储器地址 及DMA 长度  开启DMA
{
	DMA_Cmd(DMA1_Channel5,DISABLE);      //关闭DMA

  DMA1_Channel5->CMAR   =  MemoryBaseAddr ;   //重新装载存储器地址
  DMA1_Channel5->CNDTR  =  LEN ;  	          //装载缓存长度 
	
 	DMA_Cmd(DMA1_Channel5,ENABLE);      //开启DMA
}


void USART1_RXD_DMA_Init(void)
{
	RX_DMA_STA = 0 ;  //清零
	SET_RX_DMA_BUF0 ;  //设置缓存 0
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);                        //关闭接收中断
	FIFO_CLR_BUF(RX_BUF_FIFO);                                             //清空FIFO
	
  USART1_RXD_DMA_CONFIG() ;                                              //初始化 串口1 DMA 接收配置   单次传输  传输完成中断
  DMA_FIFO_CONFIG((u32)RX_DMA_BUFF0 ,DMA_BUFF_SIZE) ;                    //设置存储器地址 及DMA 长度  开启DMA
}


void USART1_IRQHandler(void)   
{
	u8 com_data =0;
	//接收中断 (接收寄存器非空)----------------------------------------
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
	 com_data= USART1->DR;
	
//UTC时间校准任务------------------------------------------
  if(RTC_CONFIG_FLAT)	 //需要校准		     
	{
    GPS_BUF_TIME_Adjust_Task(GPS_BUF,com_data);   //运行GPS时间校准任务		
		if(GPS_BUF_TIME_Adjust(GPS_BUF)==GPS_FREE)  //时间解析	 
		{
			RTC_Set(GPS_TIME->syear,GPS_TIME->smon,GPS_TIME->sday,GPS_TIME->hour,GPS_TIME->min,GPS_TIME->sec);  //更新RTC时间
	    RTC_CONFIG_Nuber_ADD;        //校准次数减一		
		}
	}
	else
	{
	//写入FIFO------------------------------------------------
		FIFO_WRITE_Byte(RX_BUF_FIFO,com_data);  //写入FIFO
	}
 }
}




void DMA1_Channel5_IRQHandler(void)    //串口DMA接收完成中断
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC5)!=RESET)	//判断通道5传输完成
	{
		DMA_ClearFlag(DMA1_FLAG_TC5|DMA1_FLAG_HT5);                               //清除通道5传输完成标志 清除传输过半标记
		if( RX_DMA_FLAT == 0 )   //如果之前缓存的速度处理完了
		{
     SET_RX_DMA_UP ;          //设置缓存更新完成标记
		 if( RX_DMA_Nuber == 0 )  //如果之前传输的是第0个缓存
		 { 
			SET_RX_DMA_BUF1 ;   //指示第一个缓存开始接收
		  DMA_FIFO_CONFIG((u32)RX_DMA_BUFF1 ,DMA_BUFF_SIZE) ;   //设置存储器地址 及DMA 长度  开启DMA
		 }
		 else
		 {
			SET_RX_DMA_BUF0 ;   //指示第0个缓存开始接收
		  DMA_FIFO_CONFIG((u32)RX_DMA_BUFF0 ,DMA_BUFF_SIZE) ;   //设置存储器地址 及DMA 长度  开启DMA		 
		 }
	  }
	  else   //DMA已经接收完成 但是上次缓存数据还没处理完
	  {
	    Config_Counter ++;        //设置SD卡写速度过慢标记   数据丢失
		 if( RX_DMA_Nuber == 0 )    //如果之前传输的是第0个缓存
		 { 
			SET_RX_DMA_BUF1 ;         //指示第一个缓存开始接收
		  DMA_FIFO_CONFIG((u32)RX_DMA_BUFF1 ,DMA_BUFF_SIZE) ;   //设置存储器地址 及DMA 长度  开启DMA
		 }
		 else
		 {
			SET_RX_DMA_BUF0 ;         //指示第0个缓存开始接收
		  DMA_FIFO_CONFIG((u32)RX_DMA_BUFF0 ,DMA_BUFF_SIZE) ;   //设置存储器地址 及DMA 长度  开启DMA		 
		 }
	  }
	}

	if(DMA_GetFlagStatus(DMA1_FLAG_TE5)!=RESET)   //传输错误中断
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
	//接收中断 (接收寄存器非空)----------------------------------------
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
	   juli_data= USART2->DR;
		 DISTANCE_BUF[DATA_CNT++]= juli_data;
		
		 if(DISTANCE_BUF[DATA_CNT-1]==0x17)
		 {
			  
		   if(DISTANCE_BUF[DATA_CNT-2]==0x07)//如果收到0x6d,0x0a.就将数据存到sd卡
			 {
				 DATA_CNT=0;
			   DISTANCE_BUF_TO_SD=1;
//				 buff_len=DATA_CNT-1;
			 }
		  	
			
		 }
		
		
  }
}


