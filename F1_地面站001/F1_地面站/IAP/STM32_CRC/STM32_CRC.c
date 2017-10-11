#include "STM32_CRC.h"



void STM32_CRC_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);      //����CRC ʱ��
  CRC->CR |= 0X01 ;                                      //��λ
}


u32 ST32_CRC_DATA_Str(u32 *BUFF ,u16  len)
{
  CRC->CR |= 0X01 ;                                      //��λ
	while((CRC->CR&0x01) ==1) ;   //�ȴ���λ���
	
	while(len--)
	{
	 CRC->DR = *BUFF ;
	 BUFF ++ ;
	}
  return (u32)CRC->DR ;
}

