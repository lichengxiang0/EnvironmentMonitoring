#include "file_ini.h"
#include "Malloc.h"
#include "SRAM_DATA.h"
#include "exfuns.h"
#include "Oled.h"
#include <string.h>
#include "delay.h"
#include "Led.h"
#include "GPS_STA.h"
#include "USART1_DMA.h"
#include "Exit.h"
#include "OLED_GUI.h"
#include "PLAY_DATA.H"
#include "stm32f10x_it.h" 
#include "SRAM_DATA.h"

#define   FILE_BUF_MAX            20         //�ļ��������С
#define   GPS_JUST_Nuber          5          //GPSУ׼����  �ٸ�ֵRTC
#define   GPS_CONFIG_Nuber_Size   10         //GPS�ϵ����ô���

void NO_CONFIG_FILE_EXE(void) 
{
	u32 baud = 115200 ;      //������
	u8  res  = 0 ;           //��������
//OLED��ʾ���ڳ�ʼ����ɣ��Լ���ӡ������
	Movie_Show_Img(USART_INIT); //�������ý���
	OLED_ShowNum(48,16,baud,6,16,1);	//��ʾ���ڲ�����  ��ʾ
	delay_ms(1500);

  Uart1_Init(baud);	//���ڳ�ʼ������ռ���ȼ�0�������ȼ�2

//�ȴ�GPS�忨��Ӧ-----------------------------------------------
	while(GPS_CONFIG_TIMEA()&&(res<=GPS_CONFIG_Nuber_Size))  //����GPS�ر��������  �����ô�ӡʱ������֡  ���ȴ� 5*12S
	{
		SYS_Scan_TF_Err();  //TF �����
	  res ++;
		delay_ms(100);
	}
	
  if(res>=GPS_CONFIG_Nuber_Size)
	{
		Movie_Show_Img(GPS_ERR);    //��ʾ�������
	  LED0 = 1;                      //Ϩ��
		while(1)
		{
		  LED1 =! LED1; 	//��ʼ��ʧ�ܣ�LED��˸��ʾ
		 delay_ms(300);
		}
	}
//--------------------------------------------------------------
//��ʼУ׼RTC  ��ȡ GPSʱ������ --------------------------------=
//�ϵ��һ��ʱ��У׼��GPS��λ------------------------------------------------------
//�ϵ����Ҫ�ȴ��㹻ʱ��  �ȴ������ȶ�---------------------------------------
	PLAY_GPS_Cartoon(0);                    //��ʾ����
	res = 0;
	GPS_BUF_clear_Init(GPS_BUF);   //ȫ�����
	LED0 = 1;
  LED1 = 0;  
	res = 0 ;               //��������
  SET_RTC_Nuber( GPS_JUST_Nuber ) ;    //У׼���� ��ֵ
	while(1)   //�ɹ�У׼����  ��ɺ���˳�
	{
//����UTCʱ�䲢У׼RTC------------------------------------------	
		if(RTC_CONFIG_FLAT==0) 	//�ȴ�У׼���
		{
        break;    //У׼��� �˳�
	  }
		else
		{
//����GPSʱ������ʱ���Ӷ���Ч��------------------------------------------
		  res = (res+GPS_PLAY_LEN+1)%GPS_PLAY_LEN ;  //�������ı���  ���ڽ���  ����Ϊ 5    ֵΪ 0 �� 4  
			LED0 = !LED0;
			LED1 = !LED1;
      PLAY_GPS_Cartoon(res);                    //��ʾ����
      delay_ms(700);
//---����Ч������---------------------------------------------------
	 }
	//SD�� �γ����-----------------------------------
    SYS_Scan_TF_Err();  //TF �����
	}
//RTC�ϵ�ʱ���һ�����ݷ���ʱ������У׼���   GPS��λ���-------------------------------------------------------------	
	 
	
//����Ƿ��Ѿ��յ��㲥----------------------------------------------------------	
  if(Satellite_Broadcast_Reg != Satellite_Broadcast_FLAT)    //���û���յ����ǹ㲥   ��Ҫ�ȴ��㲥
	{
	  Wait_Satellite_Broadcast();   //�ȴ����ǹ㲥����
	}
	
//�ϵ��һ��RTCʱ��GPSʱ������У׼����-------------------------------------------------------

	if( GPS_CONFIG_5HZ() != 0 ) 	 //����GPS    �������ʧ��
	{
		Movie_Show_Img(GPS_ERR);    //��ʾ�������
	  LED0 = 1;                      //Ϩ��
		while(1)
		{
		  LED1 =! LED1; 	//��ʼ��ʧ�ܣ�LED��˸��ʾ
		  delay_ms(300);
		}
	}
//GPS�忨�������---------------------------------------------------------
}



void Wait_Satellite_Broadcast(void)  //�ȴ����ǹ㲥
{
	u8  res  = 0 ;                   //��������
	u8  Play_buff[16] = { 0 } ;      //�Դ�
	Movie_Show_Img(GPS_broadcast);   //   ��ʼ�ȴ����ǹ㲥����
	
	RTC_WaitForLastTask();	         //�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_Set(2016,9,12,0,0,0);        //������ʼʱ��  ���ڷ���ȴ����ǹ㲥ʱ ʱ���ȡ
	RTC_WaitForLastTask();	         //�ȴ����һ�ζ�RTC�Ĵ�����д�������
	
	delay_ms(1500);
  
//�ȴ�ʱ�䵽��-----------------------------------------------
  while(1)
	{
	 //��ʾ��ǰ����ʱ��--------------------------------
		sprintf((char *)Play_buff,"    %02d:%02d:%02d    ",calendar.hour,calendar.min,calendar.sec) ;
		OLED_ShowString(0,0,Play_buff,16,1,1);
		
		//���ʱ���Ƿ񵽴�15����   ��ʼ���� ʱ������У׼----------------
		if( calendar.min>= 1)   //����15����
		{
		  if((res&0x80) == 0 )
			{
			  res |= 0x80 ;
				GPS_BUF_clear_Init(GPS_BUF);   //ȫ�����
        SET_RTC_Nuber( GPS_JUST_Nuber ) ;    //У׼���� ��ֵ
			}
		 if(RTC_CONFIG_FLAT == 0 )  //У׼���
		 {
			//д��㲥��� ------------------------
			 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
       PWR_BackupAccessCmd(ENABLE);	                                            //ʹ�ܺ󱸼Ĵ������� 
       Satellite_Broadcast_Reg = Satellite_Broadcast_FLAT ;                     //д���ѽ��չ㲥���
		   break;
		 }
		}
		SYS_Scan_TF_Err();  //TF �����
		delay_ms(300) ;
	}
}


