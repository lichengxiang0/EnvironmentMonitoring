#include "SRAM_DATA.h"
#include "Malloc.h"    
#include "Led.h"
#include "sdio_sdcard.h"
#include "GPS_STA.h"
#include "OLED_GUI.h"
#include "OLED_BMP.h"
#include "OLED_TXT.h"
#include "Exit.h"
#include "ff.h"
#include "delay.h"



u8 *Folder_Name = 0;	                          //�����ļ�����    �����û������ļ�������
u8 *Moving_Station_File_Name = 0;	              //�����ļ���      GPS�����ļ�
u8 *Aerial_Point_File_Name = 0;	                //�����ļ���      ���ĵ�ʱ���ļ� 
u8 *Icm20602_File_Name = 0;                      //�����ļ���
u8 *laser_File_Name = 0;    

u8 *Write_Buff = 0;                           	//���ĵ�д�����ݻ�����
u8 *Write_Zt_Buff = 0;                           	//���ĵ�д�����ݻ�����
u8 *Write_Jg_Buff = 0;                           	//���ĵ�д�����ݻ�����

MY_GPS_TIME_typ  *GPS_TIME = 0   ;                 //GPSʱ�����ݻ���
MY_UTC_TIME_typ  *UTC_TIME = 0   ;                 //UTCʱ�����ݽṹ��

MY_FIFO_typ      *RX_BUF_FIFO = 0   ;              //���ڽ��ջ���FIFO

MY_GPS_typ       *GPS_BUF  =  0     ;              //GPSʱ�����ݽ�������
MY_UTC_typ       *UTC_BUF  =  0     ;              //UTCʱ�����ݽ�������

Cam_FIFO_typ     *TIME_FIFO =  0    ;              //����ʱ��� FIFO

u8  SD_Res_STA  =    0  ;                          //SD���ļ�����״̬  
u8  RTC_ADJUS = 0       ;                          //У׼ָʾ����

//˫�������------------------------------------------------------------------
u8  *RX_DMA_BUFF0  ;                               //˫����
u8  *RX_DMA_BUFF1  ;  
u8   RX_DMA_STA = 0; 
//---------------------------------------------------------------------------

u8 SRAM_DATA_Init(void)  //�û�����ָ�������ڴ�  ����0 �ɹ�   1ʧ��
{
 //ָ�뻺������-----------------------------------------------------------------
 //���ĵ����ݻ������ڴ����
//	Write_Buff = mymalloc(80);
//		Write_Zt_Buff = mymalloc(80);
//	  Write_Jg_Buff= mymalloc(80);
	//�����ļ���      ���ĵ�ʱ���ļ�   ���뻺��
//	Aerial_Point_File_Name = mymalloc(FIL_NAME_LEN);   
 //����40���ֽ��ڴ�,����"0:TEXT/TEXT20120321210633.txt"sprintf�õ��Ļ�������Ҫ�ĸ��ֽڶ���
//	Moving_Station_File_Name = mymalloc(FIL_NAME_LEN);
	
//	Icm20602_File_Name = mymalloc(FIL_NAME_LEN);
	
	laser_File_Name = mymalloc(FIL_NAME_LEN);
	
 //GPSʱ�����ݻ����ڴ�����----------------------------------------------------------------
//	GPS_TIME = mymalloc(sizeof(MY_GPS_TIME_typ));
	
 //FIFO���뻺��-------------------------------------------------------------------
//	 RX_BUF_FIFO = (MY_FIFO_typ *)mymalloc(sizeof(MY_FIFO_typ)) ;
	
 //GPSʱ�����ݽ����������뻺��-------------------------------------------------------------- 
//	 GPS_BUF = (MY_GPS_typ *)mymalloc(sizeof(MY_GPS_typ));

 //�����ļ�����         �����û������ļ�����  ���뻺��
	Folder_Name = mymalloc(FIL_NAME_LEN);

//	if((Write_Jg_Buff==0)||(Write_Zt_Buff==0)||(Write_Buff==0)||(Aerial_Point_File_Name==0)||(Moving_Station_File_Name==0)||(RX_BUF_FIFO==0)||(GPS_BUF==0)||(Folder_Name==0))   //ֻҪ��һ������ʧ�� �ʹ���
//	{
//	   return 1;
//	}
	return 0;
}


u8 SYS_START_Init(void)   //ϵͳ��ʼ����ʱ �ڴ�����  ���� 0 �ɹ�  1ʧ��
{
	//�ڴ��ͷ�----------------------------------------------------------------
	 myfree(Folder_Name) ;  //�ͷű����ļ����� �����û������ļ�����  �ͷŻ���
//	 myfree(RX_BUF_FIFO) ;  //�ͷ�FIFO����
//	 myfree(GPS_BUF) ;      //�ͷ�GPSʱ�����ݽ�������
	//----------------------------------------------------------------------------
	
 //����ʱ��� FIFO	 ���뻺��-----------------------------------------------------
//	 TIME_FIFO = (Cam_FIFO_typ *)mymalloc(sizeof(Cam_FIFO_typ));

//DMA˫���� �����ڴ�-------------------------------------------
//   RX_DMA_BUFF0 = (u8 *)mymalloc(DMA_BUFF_SIZE) ;
//   RX_DMA_BUFF1 = (u8 *)mymalloc(DMA_BUFF_SIZE) ;  

//	if((TIME_FIFO==0)||(RX_DMA_BUFF0==0)||(RX_DMA_BUFF1==0))   //ֻҪ��һ������ʧ�� �ʹ���
//	{
//	   return 1;
//	}
	return 0;
}



void SYS_Scan_TF_Err(void)   //TF ����������
{
 if((SD_Res_STA!=FR_OK)||(SD_SACN==1))  //���ļ�����ʧ��
 {
	LED0=1;
	LED1=0;
	Movie_Show_Img(SD_ERR);   //����ͼ����ʾ
	OLED_Gram_Update();    //��ʾ	
	while(1)
	{
	 if(SD_SACN==0)   //SD������
	 {
		 delay_ms(1500);
		 if(SD_SACN==0)
		 {
			Sys_Soft_Reset();  //ϵͳ�����λ		
		 }	
	 }
	  delay_ms(300);
	  OLED_Gram_Inverse();   //�Դ淴ɫ ��ʾ	
	  LED0=!LED0;
	  LED1=!LED1;	
	 }
  }
}



























