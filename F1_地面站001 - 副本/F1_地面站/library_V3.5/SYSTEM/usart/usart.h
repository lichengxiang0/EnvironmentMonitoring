#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 



void Uart1_Init(u32 bound);

void USART1_SEND_Byte(u8 data) ;

void Uart1_send_str(u8 * buf,u16 len) ;



void Uart2_Init(u32 bound);

void USART2_SEND_Byte(u8 data) ;

void Uart2_send_str(u8 * buf,u16 len) ;






#endif


