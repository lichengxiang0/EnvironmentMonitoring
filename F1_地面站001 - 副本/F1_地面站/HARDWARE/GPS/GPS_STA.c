#include "GPS_STA.h"
#include <string.h>
#include "delay.h"
#include "USART1_DMA.h"
#include "MY_FIFO.H"
#include "SRAM_DATA.h"
#include "OLED_GUI.h"
#include "Oled.h"
#include "Led.h"
#include "M_DATA.H"



const u8 *GPS_CMM_5HZ[GPS_COMM_CONFIG_Nuber] ={            //5HZ ����ָ�
 "log com1 rangecmpb ontime 0.2\r\n"     ,          //���5Hz��ԭʼ�۲�����               
 "log com1 gpsephemb ontime 60\r\n"       ,          //���gps�����ļ�
 "log com1 gloephemerisb ontime 60\r\n"   ,          //������GLONASS�����ļ�
 "log com1 bd2ephemb ontime 60\r\n"       ,          //������������ļ�
 "log com1 bestposb ontime 0.2 \r\n"       ,          //�����λ1Hz��λ��� 
} ;


  
const u8 *GPS_REST_CMM           =  "freset\r\n" ;                //GPS�ָ��ɳ�����������
const u8 *GPS_TIME_CMM           =  "LOG TIMEA ONTIME 1\r\n"   ;  //GPS�������GPSʱ������
const u8 *GPS_UNLOGALL_CMM       =  "UNLOGALL COM1 TRUE\r\n"   ;  //GPS���ùر����д��ڴ�ӡ���
const u8 *GPS_SAVE_CMM           =  "saveconfig\r\n"           ;  //����GPS����

void GPS_PV_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(GPS_PV_RCC,ENABLE);//ʹ��ʱ��

	GPIO_InitStructure.GPIO_Pin  = GPS_PV_PIN;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //���ø�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
 	GPIO_Init(GPS_PV_, &GPIO_InitStructure);	
}

u8 GPS_CONFIG_RESET(void)  //GPS��λ   ���� 0�ɹ�   1ʧ��
{
	u8 i = 0 ;
	u8 TESTBUF[9] ={0};     //�������������
  Uart1_send_str((u8*)GPS_REST_CMM,strlen((const char *)GPS_REST_CMM));   //���͸�λ
	FIFO_CLR_BUF(RX_BUF_FIFO);        //���FIFO
	delay_ms(500);     //�ȴ�����ش����
	
	if(FIFO_BUF_Valid_NUM(RX_BUF_FIFO)<64)   //��λָ��ش��ֽ���  ��Լ����
	{
	  return 1 ;       //ʧ��
	}
	FIFO_CLR_BUF(RX_BUF_FIFO);        //���FIFO
	
	while(i<40)    //�ȴ���λ���   ���� [COM1] �ַ���  �ȴ� 20 S
	{
	 if(FIFO_BUF_Valid_NUM(RX_BUF_FIFO)>=8)  //�ַ�����С����
	 {
	   FIFO_READ_BUF(RX_BUF_FIFO,TESTBUF,8);  //��ȡ����
	   TESTBUF[8] = '\0';   //��ӽ�����
		 if(strcmp((const char*)TESTBUF,(const char*)"[COM1]\r\n")==0)   //�Ƚ�
		 {
       return 0 ;         //��λ�ɹ���
		 }
		 else
		 {
		   return 1 ;         //�������� ��ʧ��
		 }	
	 }
	 delay_ms(500);
	 i++;    //�˴��ɼ���ʾ ������Ч��
	}
	return 1 ;         //��ʱ ����ʧ��
}

#define GPS_REST_MAX   60   //��λʧ�ܴ���   ��Լ 12 S

u8 GPS_CONFIG_5HZ(void)   //���ó� 5HZ   ����0�ɹ�   1ʧ��
{
	u8 i = 0 ;
	while(GPS_UNLOGALL_CONFIG()&&(i<GPS_REST_MAX)) 
	{  
	   i ++;
		delay_ms(200) ;
	}
	if(i>=GPS_REST_MAX)
	{
	  return 1;  //ʧ��
	}
	
  for(i=0;i<GPS_COMM_CONFIG_Nuber;i++)
	{
	 Uart1_send_str((u8*)GPS_CMM_5HZ[i],strlen((const char *)GPS_CMM_5HZ[i]));
	}
	return 0 ;    //�ɹ�
}

u8 GPS_UNLOGALL_CONFIG(void)  //GPS�ر��������   0�ɹ�    1ʧ��
{
	FIFO_CLR_BUF(RX_BUF_FIFO);        //���FIFO
  Uart1_send_str((u8*)GPS_UNLOGALL_CMM,strlen((const char *)GPS_UNLOGALL_CMM)); //���͹ر��������
	delay_ms(500) ;  //�ȴ��������
	
	if(FIFO_BUF_Valid_NUM(RX_BUF_FIFO)>=30)  //ʵ����ӦΪ 32�ֽ�
	{
	  if((RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-4]==0X4F)&&(RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-3]==0X4B))  //������3�͵�4��  Ϊ OK �ַ�
	  {
		  return 0;  //���سɹ�
		}
	  return 1;
	}
	
	else if(FIFO_BUF_Valid_NUM(RX_BUF_FIFO)>=10)
	{
		if((RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-10]==0X4F)&&(RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-9]==0X4B))  //������3�͵�4��  Ϊ OK �ַ�
	  {
		  return 0;  //���سɹ�
		}
	  return 1;
	}
	return 1 ;
}

u8 GPS_CONFIG_TIMEA(void)  //�������ʱ������
{
  u8 i = 0;
	while(GPS_UNLOGALL_CONFIG()&&(i<GPS_REST_MAX))
	{  
	   i ++;
		delay_ms(200) ;
	}
	
	if(i>=GPS_REST_MAX)
	{
	  return 1;  //ʧ��
	}
//	GPS_SAVE_CONFIG();  //��������
	Uart1_send_str((u8*)GPS_TIME_CMM,strlen((const char *)GPS_TIME_CMM));
	return 0;  //�ɹ�
}

u8 GPS_SAVE_CONFIG(void)  //��������
{
	FIFO_CLR_BUF(RX_BUF_FIFO);         //���FIFO
  Uart1_send_str((u8*)GPS_SAVE_CMM,strlen((const char *)GPS_SAVE_CMM)); //���ͱ�������
	delay_ms(20) ;  //�ȴ��������
	
	if(FIFO_BUF_Valid_NUM(RX_BUF_FIFO)>=10)  
	{
	  if((RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-4]==0X4F)&&(RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-3]==0X4B))  //������3�͵�4��  Ϊ OK �ַ�
	  {
		  return 0;  //���سɹ�
		}
	  return 1;
	}
	return 1 ;
}
