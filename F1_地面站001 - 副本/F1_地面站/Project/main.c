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
//  const u8 *JG_DANDIANCEJU  = "MS:single\n";//单次测量
  extern u8 Jg_CeJu_Flag;
  extern 	float f_euler_array[3];
  
  extern u8 DISTANCE_BUF_TO_SD;
  extern u8 DISTANCE_BUF[21];
  
  extern u8 buff_len;
  
  int main(void)
  {
  	 int i;
  	//MY_NVIC_SetVectorTable(FLASH_BASE,APP_START_ADDR-FLASH_BASE);    //中断向量表偏移
  //	Scan_SYS_Bootloader_Lock();   //检测用户代码是否为Bootloader烧录  如果不是设备自毁
  	delay_init();	    	 			  	//延时函数初始化	 
  	NVIC_Configuration(); 	 			//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
  	mallco_dev.init();	   	      //初始化内存池
  	OLED_Init();                  //OLED初始化
  	LED_Init();                   //LED初始化
  
//  	I2C_Configuration();           //i2c初始化
//  	 Init_Icm20602_Soft();          //icm20602初始化配置
      Uart2_Init(9600);	//串口初始化，抢占优先级0，子优先级2
//    GPS_PV_Init();                //GPS PV引脚检测  
  	SD_SACN_Init();               //SD卡插入检测
  	exfuns_init();		            //为fatfs相关变量申请内存
  	
    //指针缓存申请-----------------------------------------------------------------
  if(SRAM_DATA_Init()==1)   //只要有一个申请失败 就错误
  {
    OLED_BUF_Clear();
    OLED_ShowString(0,32,"SRAM Error!",16,1,2);
    while(1)
    {
  	LED1 =! LED1; 	//初始化失败，LED闪烁提示
      delay_ms(300);
    }
  }
  //----缓存申请结束---------------------------------------------------------------
  //	
  Movie_Show_Img(LOGO_YW);//显示LOGO
  delay_ms(3000);
  //
  //显示版本及版权----------------------------
  //Play_RTK_Copyright();
  delay_ms(3000) ;
  //
  //检测SD卡与初始化，SD卡未插入，死循环，LED闪烁提示
  if(SD_Init()||(SD_SACN!=0)||(KMM!=1))   //第一次扫描SD卡是否存在   检测不到SD卡  
  {
    Movie_Show_Img(SD_ERR);   //SD 卡故障图片显示   				
    while(SD_Init()||(SD_SACN!=0)||(KMM!=1))
    {
  	  OLED_Gram_Inverse();   //显存反色 显示		
      LED0 =! LED0;//DS0闪烁
      delay_ms(300);
    }
  }
  
  f_mount(fs[0],"0:",1)	;				//挂载SD卡
  SD_Init() ;
  ///SD卡初始化成功提示，显示SD卡容量
  Movie_Show_Img(SD_INIT) ;   //SD卡 汉字初始化界面
  OLED_ShowNum(40,16,SDCardInfo.CardCapacity>>20,5,16,1);//显示SD卡容量
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
  
  //初始化字库及FLASH图片检查-------------------------------------------
  //if(font_init()>0)		  //初始化字库  失败返回 1
  //{
  //  if(update_font()>0)   
  //	{
  //	   while(1) ;    //SD卡内没有字库文件 或者内存申请失败  或者 SD卡拔出
  //	}
  //}
  //FLASH内部图片检查，也可不检查----------------------------------------
  //
  //--------------------------------------------------------------------	
  //配置内部RTC---------------------------------------------------	
  Movie_Show_Img(RC_S);  //RTC配置完成	
  while(RTC_Init())  //RTC初始化，抢占优先级2，子优先级1  
  {
  	LED1 =! LED1; 	//初始化失败，LED闪烁提示
  	OLED_BUF_Clear();  //清除显存
  	OLED_ShowString(0,16,"32.768KHZ Error",16,1,2);   //错误原因 只有晶振不起振
  	delay_ms(300);
  };
  delay_ms(500);
  Movie_Show_Img(RC_E);  //RTC配置完成
  delay_ms(500);
  //
  //SD卡 配置文件 配置任务   检测GPS板卡是否正常----------------------------------------------
//   NO_CONFIG_FILE_EXE();	 
  //上电第一次RTC时间GPS时间数据校准结束----------------------------------------------------
  //
  USART_Cmd(USART1, DISABLE);                    //关闭串口
  LED0=0;
  LED1=0;
  Movie_Show_Img(GPS_OK);     //GPS时间校准数据成功   定位成功界面
  delay_ms(1000);
  //
  Movie_Show_Img(File_S) ;    //文件创建开始界面
  delay_ms(500);
  //
  //创建保存GPS轨迹的文件和航拍点时间文件的文件夹，以时间命名---------------------------------
  New_Folder(Folder_Name);  //赋予文件名     保存快门时间数据
  SD_Res_STA=f_mkdir((const TCHAR*)Folder_Name);  //创建文件夹
  
  //创建记录GPS轨迹的文件,文件名Moving_Station_+时间
  Base_Station_New_File(Moving_Station_File_Name); //赋予文件名
  SD_Res_STA = f_open(file, (const TCHAR*)Moving_Station_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//打开文件  如果没有文件 则创建文件  
  SD_Res_STA = f_lseek(file,file ->fsize); //移动指针到结尾 
  SD_Res_STA = f_close(file);   //关闭打开的文件
  //
  //
  Icm20602_New_File(Icm20602_File_Name); //赋予文件名
  SD_Res_STA = f_open(file, (const TCHAR*)Icm20602_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//打开文件  如果没有文件 则创建文件  
  SD_Res_STA = f_lseek(file,file ->fsize); //移动指针到结尾 
  SD_Res_STA = f_close(file);   //关闭打开的文件
  
  
  laser_New_File(laser_File_Name); //赋予文件名
  SD_Res_STA = f_open(file, (const TCHAR*)laser_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//打开文件  如果没有文件 则创建文件  
  SD_Res_STA = f_lseek(file,file ->fsize); //移动指针到结尾 
  SD_Res_STA = f_close(file);   //关闭打开的文件
  
  //文件操作结果判定------------------------------------------------------
  if(SD_Res_STA!=FR_OK)
  {
    Movie_Show_Img(FILE_ERR) ;    //文件创建失败界面
  	LED0 = 1;   //熄灭
    while(1)
  	{
  		if(SD_SACN==0)   //SD卡插入
  		{
  		  delay_ms(1000);
  			if(SD_SACN==0)
  			{
  			  Sys_Soft_Reset();  //系统软件复位		
  			}	
  		}
  	  OLED_Gram_Inverse();   //显存反色 显示	
  		LED1 =! LED1; 	//初始化失败，LED闪烁提示
  		delay_ms(300);
  	}
  }
  //---------------------------------------------------------------------
  Movie_Show_Img(File_E) ;    //文件创建完成界面
  delay_ms(500);
  //
  //初始化变量-------------------------------------------------------
  //LED0_OFF;                               //关闭快门指示灯
  //Write_Counter  = 0;                     //SD卡写GPS数据次数清零
  //Write_Counter_CAMM = 0 ;                //拍照次数清零
  //
  //系统运行指针缓存申请-----------------------------------------------------------------
  if(SYS_START_Init()==1)   //只要有一个申请失败 就错误
  {
  	OLED_BUF_Clear();
  	OLED_ShowString(0,16,"SYSTEN TART",16,1,2);
  	OLED_ShowString(0,32,"SRAM Error!",16,1,2);
    while(1)
  	{
  	 	LED1 =! LED1; 	//初始化失败，LED闪烁提示
  		delay_ms(300);
  	}
  }
  
  PLAY_DATE_Init();                       //初始化显示界面	
  Cam_FIFO_CLR(TIME_FIFO);                //清空快门时间记录FIFO
     TIM3_Configuration();
     Tim4_Init(1000-1);  //1MHz定时器 1000 中断周期 1mS   快门引脚状态延时检测 用于消抖及干扰 及用于控制拍照指示灯
     
  USART1_RXD_DMA_Init();                  //串口1 DMA接收初始化
  USART_Cmd(USART1, ENABLE);              //开启串口
   while(1)
   {
  	
  ///GPS数据写入SD卡--------------------------------
  if(RX_DMA_FLAT > 0 ) 	//如果DMA缓存有一个接收完成
  {
  //GPS数据写入文件--------------------------------------------------------------------------------------------		 
   SD_Res_STA = f_open(file, (const TCHAR*)Moving_Station_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//打开文件
   SD_Res_STA = f_lseek(file,file ->fsize); //移动指针到结尾
   if(RX_DMA_Nuber == 0 ) //如果现在开始写缓存0  说明缓存1接收完成
   {
  	 SD_Res_STA = f_write(file,RX_DMA_BUFF1,DMA_BUFF_SIZE,&br);			 //写入文件   缓存 1 数据写入SD卡
   }
   else  //说明缓存 0 接收完成了
   {
  	 SD_Res_STA = f_write(file,RX_DMA_BUFF0,DMA_BUFF_SIZE,&br);			 //写入文件	  写缓存 0  数据写入SD卡
   }
   	SD_Res_STA = f_close(file);   //关闭打开的文件
    CLR_RX_DMA_UP ;    //清除写标记
   	LED1 =! LED1; 
  	Write_Counter++;       //写SD卡次数 
  } 
      PLAY_DATE_TASK();   //显示任务
  
  
    i2c_start_cnt = TIM_GetCounter(TIM3);
  //  if(ICM20602_TIM4_FLAG==1)   //姿态传感器数据
  // {	  	
  	//  ICM20602_TIM4_FLAG=0;
     	Task_Get_Imu_Sensor_Data(); 
    	/*姿态更新*/
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
  		
  		main_while_period = Get_Function_Period(&ul_main_while_last_micros);   //计算函数执行周期s
  	
  //  Icm20602_New_File(Icm20602_File_Name); //赋予文件名
  	sprintf((char *)Write_Zt_Buff,"%.6f--%.6f---%.6f\r\n",f_euler_array[0],f_euler_array[1],f_euler_array[2]);
    SD_Res_STA = f_open(file, (const TCHAR*)Icm20602_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//打开文件  如果没有文件 则创建文件  
    SD_Res_STA = f_lseek(file,file ->fsize); //移动指针到结尾 
    SD_Res_STA = f_write(file,Write_Zt_Buff,strlen((char *)Write_Zt_Buff),&br);			 //写入文件   缓存 1 数据写入SD卡
  		SD_Res_STA = f_close(file);   //关闭打开的文件
  
     
  		/*坐标转化处理*/
  	//	Task_Get_Coordinate_Transform_Matrix();
  	 	/*处理校准*/
  		//Task_Gimbal_Cal_Loop();
  		
  		/*定时反馈欧拉角和框架角*/
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
  	SD_Res_STA = f_open(file, (const TCHAR*)laser_File_Name, FA_OPEN_ALWAYS | FA_WRITE); 	//打开文件  如果没有文件 则创建文件  
    SD_Res_STA = f_lseek(file,file ->fsize); //移动指针到结尾 
    SD_Res_STA = f_write(file,Write_Jg_Buff,strlen((char *)Write_Jg_Buff),&br);			 //写入文件   缓存 1 数据写入SD卡
    SD_Res_STA = f_close(file);   //关闭打开的文件
  	
  	}
  			
  			
  			
  //文件操作失败-------------------------------------------------------------------------------------
     SYS_Scan_TF_Err();
  //--------------------------------------------------------------------------------------------------------------
   }
  }
  





