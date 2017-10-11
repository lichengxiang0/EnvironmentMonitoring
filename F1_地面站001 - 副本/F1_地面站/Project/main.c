#include "delay.h"
#include "sys.h"
#include "USART1_DMA.h"
#include "Led.h"
#include "Oled.h"
#include "Malloc.h"
#include "Rtc.h"
#include "exfuns.h"
#include "Exit.h"
#include "Txt.h"
#include "Gps.h"
#include "PLAY_DATA.H"
#include "sdio_sdcard.h"
#include "SRAM_DATA.h"
#include "GPS_STA.h"
#include "OLED_GUI.h"
#include "GPS_DATE.h"
#include <string.h>
#include "file_ini.h"
#include "Cam_FIFO.h"
#include "OLED_BMP.h"
#include "OLED_TXT.h"
#include "fontupd.h"
#include "IAP_DATA.H"
#include "soft_iic.h"
#include "stdio.h"
  extern u8 ICM20602_TIM4_FLAG;
  uint16_t	i2c_start_cnt,	i2c_end_cnt;
  float			i2c_period;
  float	main_while_period;  //test
  uint32_t	ul_main_while_last_micros;  //test
  //u8 ICM20602_BUFF[10]={1,2,3,4,5,6,7,8,9,10};
	const u8 JG_Buffer[8] = {0x01,0x03,0x00,0x00,0x00,0x08,0x44,0x0C};
  extern u8 *Write_Zt_Buff;
  extern u8 *Write_Jg_Buff;
//  const u8 *JG_DANDIANCEJU  = "MS:single\n";//���β���
  extern u8 Jg_CeJu_Flag;
  extern 	float f_euler_array[3];
  
  extern u8 DISTANCE_BUF_TO_SD;
  extern u8 DISTANCE_BUF[21];
  
  extern u8 buff_len;
  
  int main(void)
  {
  	 int i;
  	//MY_NVIC_SetVectorTable(FLASH_BASE,APP_START_ADDR-FLASH_BASE);    //�ж�������ƫ��
  //	Scan_SYS_Bootloader_Lock();   //����û������Ƿ�ΪBootloader��¼  ��������豸�Ի�
  	delay_init();	    	 			  	//��ʱ������ʼ��	 
  	NVIC_Configuration(); 	 			//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
  	mallco_dev.init();	   	      //��ʼ���ڴ��
  	OLED_Init();                  //OLED��ʼ��
  	LED_Init();                   //LED��ʼ��
  
//  	I2C_Configuration();           //i2c��ʼ��
//  	 Init_Icm20602_Soft();          //icm20602��ʼ������
      Uart2_Init(9600);	//���ڳ�ʼ������ռ���ȼ�0�������ȼ�2
//    GPS_PV_Init();                //GPS PV���ż��  
  	SD_SACN_Init();               //SD��������
  	exfuns_init();		            //Ϊfatfs��ر��������ڴ�
  	
    //ָ�뻺������-----------------------------------------------------------------
  if(SRAM_DATA_Init()==1)   //ֻҪ��һ������ʧ�� �ʹ���
  {
    OLED_BUF_Clear();
    OLED_ShowString(0,32,"SRAM Error!",16,1,2);
    while(1)
    {
  	LED1 =! LED1; 	//��ʼ��ʧ�ܣ�LED��˸��ʾ
      delay_ms(300);
    }
  }
  //----�����������---------------------------------------------------------------
  //	
  Movie_Show_Img(LOGO_YW);//��ʾLOGO
  delay_ms(3000);
  //
  //��ʾ�汾����Ȩ----------------------------
  //Play_RTK_Copyright();
  delay_ms(3000) ;
  //
  //���SD�����ʼ����SD��δ���룬��ѭ����LED��˸��ʾ
  if(SD_Init()||(SD_SACN!=0)||(KMM!=1))   //��һ��ɨ��SD���Ƿ����   ��ⲻ��SD��  
  {
    Movie_Show_Img(SD_ERR);   //SD ������ͼƬ��ʾ   				
    while(SD_Init()||(SD_SACN!=0)||(KMM!=1))
    {
  	  OLED_Gram_Inverse();   //�Դ淴ɫ ��ʾ		
      LED0 =! LED0;//DS0��˸
      delay_ms(300);
    }
  }
  
  f_mount(fs[0],"0:",1)	;				//����SD��
  SD_Init() ;
  ///SD����ʼ���ɹ���ʾ����ʾSD������
  Movie_Show_Img(SD_INIT) ;   //SD�� ���ֳ�ʼ������
  OLED_ShowNum(40,16,SDCardInfo.CardCapacity>>20,5,16,1);//��ʾSD������
  OLED_ShowString(80,16," MB",16,1,1);
  switch(SDCardInfo.CardType)
  {
  	case SDIO_STD_CAPACITY_SD_CARD_V1_1:
  		OLED_ShowString(0,0,"Type:SDSC V1.1",16,1,1);
  	break;
  	
  	case SDIO_STD_CAPACITY_SD_CARD_V2_0:
  		OLED_ShowString(0,0,"Type:SDSC V2.0",16,1,1);
  	break;
  	
  	case SDIO_HIGH_CAPACITY_SD_CARD:
  		OLED_ShowString(0,0,"Type:SDHC V2.0",16,1,1);
  	break;
  	
  	case SDIO_MULTIMEDIA_CARD:
  		OLED_ShowString(0,0,"Type:MMC Card",16,1,1);
  	break;
  }	 
  delay_ms(5000);
  
  //��ʼ���ֿ⼰FLASHͼƬ���-------------------------------------------
  //if(font_init()>0)		  //��ʼ���ֿ�  ʧ�ܷ��� 1
  //{
  //  if(update_font()>0)   
  //	{
  //	   while(1) ;    //SD����û���ֿ��ļ� �����ڴ�����ʧ��  ���� SD���γ�
  //	}
  //}
  //FLASH�ڲ�ͼƬ��飬Ҳ�ɲ����----------------------------------------
  //
  //--------------------------------------------------------------------	
  //�����ڲ�RTC---------------------------------------------------	
  Movie_Show_Img(RC_S);  //RTC�������	
  while(RTC_Init())  //RTC��ʼ������ռ���ȼ�2�������ȼ�1  
  {
  	LED1 =! LED1; 	//��ʼ��ʧ�ܣ�LED��˸��ʾ
  	OLED_BUF_Clear();  //����Դ�
  	OLED_ShowString(0,16,"32.768KHZ Error",16,1,2);   //����ԭ�� ֻ�о�������
  	delay_ms(300);
  };
  delay_ms(500);
  Movie_Show_Img(RC_E);  //RTC�������
  delay_ms(500);
  //
  //SD�� �����ļ� ��������   ���GPS�忨�Ƿ�����----------------------------------------------
//   NO_CONFIG_FILE_EXE();	 
  //�ϵ��һ��RTCʱ��GPSʱ������У׼����----------------------------------------------------
  //
  USART_Cmd(USART1, DISABLE);                    //�رմ���
  LED0=0;
  LED1=0;
  Movie_Show_Img(GPS_OK);     //GPSʱ��У׼���ݳɹ�   ��λ�ɹ�����
  delay_ms(1000);
  //
  Movie_Show_Img(File_S) ;    //�ļ�������ʼ����
  delay_ms(500);
  //
  //��������GPS�켣���ļ��ͺ��ĵ�ʱ���ļ����ļ��У���ʱ������---------------------------------
  New_Folder(Folder_Name);  //�����ļ���     �������ʱ������
  SD_Res_STA=f_mkdir((const TCHAR*)Folder_Name);  //�����ļ���
  
  //������¼GPS�켣���ļ�,�ļ���Moving_Station_+ʱ��
  Base_Station_New_File(Moving_Station_File_Name); //�����ļ���
  SD_Res_STA = f_open(file, (const TCHAR*)Moving_Station_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//���ļ�  ���û���ļ� �򴴽��ļ�  
  SD_Res_STA = f_lseek(file,file ->fsize); //�ƶ�ָ�뵽��β 
  SD_Res_STA = f_close(file);   //�رմ򿪵��ļ�
  //
  //
  Icm20602_New_File(Icm20602_File_Name); //�����ļ���
  SD_Res_STA = f_open(file, (const TCHAR*)Icm20602_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//���ļ�  ���û���ļ� �򴴽��ļ�  
  SD_Res_STA = f_lseek(file,file ->fsize); //�ƶ�ָ�뵽��β 
  SD_Res_STA = f_close(file);   //�رմ򿪵��ļ�
  
  
  laser_New_File(laser_File_Name); //�����ļ���
  SD_Res_STA = f_open(file, (const TCHAR*)laser_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//���ļ�  ���û���ļ� �򴴽��ļ�  
  SD_Res_STA = f_lseek(file,file ->fsize); //�ƶ�ָ�뵽��β 
  SD_Res_STA = f_close(file);   //�رմ򿪵��ļ�
  
  //�ļ���������ж�------------------------------------------------------
  if(SD_Res_STA!=FR_OK)
  {
    Movie_Show_Img(FILE_ERR) ;    //�ļ�����ʧ�ܽ���
  	LED0 = 1;   //Ϩ��
    while(1)
  	{
  		if(SD_SACN==0)   //SD������
  		{
  		  delay_ms(1000);
  			if(SD_SACN==0)
  			{
  			  Sys_Soft_Reset();  //ϵͳ�����λ		
  			}	
  		}
  	  OLED_Gram_Inverse();   //�Դ淴ɫ ��ʾ	
  		LED1 =! LED1; 	//��ʼ��ʧ�ܣ�LED��˸��ʾ
  		delay_ms(300);
  	}
  }
  //---------------------------------------------------------------------
  Movie_Show_Img(File_E) ;    //�ļ�������ɽ���
  delay_ms(500);
  //
  //��ʼ������-------------------------------------------------------
  //LED0_OFF;                               //�رտ���ָʾ��
  //Write_Counter  = 0;                     //SD��дGPS���ݴ�������
  //Write_Counter_CAMM = 0 ;                //���մ�������
  //
  //ϵͳ����ָ�뻺������-----------------------------------------------------------------
  if(SYS_START_Init()==1)   //ֻҪ��һ������ʧ�� �ʹ���
  {
  	OLED_BUF_Clear();
  	OLED_ShowString(0,16,"SYSTEN TART",16,1,2);
  	OLED_ShowString(0,32,"SRAM Error!",16,1,2);
    while(1)
  	{
  	 	LED1 =! LED1; 	//��ʼ��ʧ�ܣ�LED��˸��ʾ
  		delay_ms(300);
  	}
  }
  
  PLAY_DATE_Init();                       //��ʼ����ʾ����	
  Cam_FIFO_CLR(TIME_FIFO);                //��տ���ʱ���¼FIFO
     TIM3_Configuration();
     Tim4_Init(1000-1);  //1MHz��ʱ�� 1000 �ж����� 1mS   ��������״̬��ʱ��� �������������� �����ڿ�������ָʾ��
     
  USART1_RXD_DMA_Init();                  //����1 DMA���ճ�ʼ��
  USART_Cmd(USART1, ENABLE);              //��������
   while(1)
   {
  	
  ///GPS����д��SD��--------------------------------
  if(RX_DMA_FLAT > 0 ) 	//���DMA������һ���������
  {
  //GPS����д���ļ�--------------------------------------------------------------------------------------------		 
   SD_Res_STA = f_open(file, (const TCHAR*)Moving_Station_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//���ļ�
   SD_Res_STA = f_lseek(file,file ->fsize); //�ƶ�ָ�뵽��β
   if(RX_DMA_Nuber == 0 ) //������ڿ�ʼд����0  ˵������1�������
   {
  	 SD_Res_STA = f_write(file,RX_DMA_BUFF1,DMA_BUFF_SIZE,&br);			 //д���ļ�   ���� 1 ����д��SD��
   }
   else  //˵������ 0 ���������
   {
  	 SD_Res_STA = f_write(file,RX_DMA_BUFF0,DMA_BUFF_SIZE,&br);			 //д���ļ�	  д���� 0  ����д��SD��
   }
   	SD_Res_STA = f_close(file);   //�رմ򿪵��ļ�
    CLR_RX_DMA_UP ;    //���д���
   	LED1 =! LED1; 
  	Write_Counter++;       //дSD������ 
  } 
      PLAY_DATE_TASK();   //��ʾ����
  
  
    i2c_start_cnt = TIM_GetCounter(TIM3);
  //  if(ICM20602_TIM4_FLAG==1)   //��̬����������
  // {	  	
  	//  ICM20602_TIM4_FLAG=0;
     	Task_Get_Imu_Sensor_Data(); 
    	/*��̬����*/
  		Task_Get_Imu_Attitude();
  		
    		i2c_end_cnt = TIM_GetCounter(TIM3); 
  	
  		if( i2c_end_cnt < i2c_start_cnt )
  		{ 
  			i2c_period =  ((float)(i2c_end_cnt + (0xffff - i2c_start_cnt ) ) / 1000000.0f);
  		}
  		else	
  		{
  			i2c_period =  ((float)(i2c_end_cnt - i2c_start_cnt ) / 1000000.0f);
  		}
  		
  		main_while_period = Get_Function_Period(&ul_main_while_last_micros);   //���㺯��ִ������s
  	
  //  Icm20602_New_File(Icm20602_File_Name); //�����ļ���
  	sprintf((char *)Write_Zt_Buff,"%.6f--%.6f---%.6f\r\n",f_euler_array[0],f_euler_array[1],f_euler_array[2]);
    SD_Res_STA = f_open(file, (const TCHAR*)Icm20602_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//���ļ�  ���û���ļ� �򴴽��ļ�  
    SD_Res_STA = f_lseek(file,file ->fsize); //�ƶ�ָ�뵽��β 
    SD_Res_STA = f_write(file,Write_Zt_Buff,strlen((char *)Write_Zt_Buff),&br);			 //д���ļ�   ���� 1 ����д��SD��
  		SD_Res_STA = f_close(file);   //�رմ򿪵��ļ�
  
     
  		/*����ת������*/
  	//	Task_Get_Coordinate_Transform_Matrix();
  	 	/*����У׼*/
  		//Task_Gimbal_Cal_Loop();
  		
  		/*��ʱ����ŷ���ǺͿ�ܽ�*/
  	//	Board_Gimbal_Send_Euler_Angle_And_Frame_Angle();
  // }
  	
  			if(Jg_CeJu_Flag==1)
  			{  
  			  Jg_CeJu_Flag=0;
//  				Uart2_send_str((u8*)JG_Buffer,strlen((const char *)JG_Buffer));
					Uart2_send_str((u8*)JG_Buffer,8);
        		
  		 }
  	if(DISTANCE_BUF_TO_SD==1)
  	{
  	   DISTANCE_BUF_TO_SD=0;
//  		
//  	 
//  		  if(buff_len==5)
//  			{
//  		sprintf((char *)Write_Jg_Buff,"%c%c%c%c%c\r\n",DISTANCE_BUF[0],DISTANCE_BUF[1],DISTANCE_BUF[2],DISTANCE_BUF[3],DISTANCE_BUF[4]);
//  				
//  			}
//  				else if(buff_len==6)
//  						{
//  		sprintf((char *)Write_Jg_Buff,"%c%c%c%c%c%c\r\n",DISTANCE_BUF[0],DISTANCE_BUF[1],DISTANCE_BUF[2],DISTANCE_BUF[3],DISTANCE_BUF[4],DISTANCE_BUF[5]);
//  				
//  			}
//  					
//  					else if(buff_len==7)
//  						{
			
			
			
  		sprintf((char *)Write_Jg_Buff,"%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\r\n",DISTANCE_BUF[0],DISTANCE_BUF[1],DISTANCE_BUF[2],DISTANCE_BUF[3],DISTANCE_BUF[4],DISTANCE_BUF[5],DISTANCE_BUF[6],DISTANCE_BUF[7],DISTANCE_BUF[8],DISTANCE_BUF[9],DISTANCE_BUF[10],DISTANCE_BUF[11],DISTANCE_BUF[12],DISTANCE_BUF[13],DISTANCE_BUF[14],DISTANCE_BUF[15],DISTANCE_BUF[16],DISTANCE_BUF[17],DISTANCE_BUF[18],DISTANCE_BUF[19],DISTANCE_BUF[20] );
//  				
//  			}	
  			//sprintf((char *)Write_Jg_Buff,"%c",DISTANCE_BUF[i]);
  	SD_Res_STA = f_open(file, (const TCHAR*)laser_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//���ļ�  ���û���ļ� �򴴽��ļ�  
    SD_Res_STA = f_lseek(file,file ->fsize); //�ƶ�ָ�뵽��β 
    SD_Res_STA = f_write(file,Write_Jg_Buff,strlen((char *)Write_Jg_Buff),&br);			 //д���ļ�   ���� 1 ����д��SD��
    SD_Res_STA = f_close(file);   //�رմ򿪵��ļ�
  	
  	}
  			
  			
  			
  //�ļ�����ʧ��-------------------------------------------------------------------------------------
     SYS_Scan_TF_Err();
  //--------------------------------------------------------------------------------------------------------------
   }
  }
  





