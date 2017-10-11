#include "STM32_CRC.h"



void STM32_CRC_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);      //开启CRC 时钟
  CRC->CR |= 0X01 ;                                      //复位
}


u32 ST32_CRC_DATA_Str(u32 *BUFF ,u16  len)
{
  CRC->CR |= 0X01 ;                                      //复位
	while((CRC->CR&0x01) ==1) ;   //等待复位完成
	
	while(len--)
	{
	 CRC->DR = *BUFF ;
	 BUFF ++ ;
	}
  return (u32)CRC->DR ;
}

