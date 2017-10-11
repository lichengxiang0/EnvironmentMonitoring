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

#define   FILE_BUF_MAX            20         //文件读缓存大小
#define   GPS_JUST_Nuber          5          //GPS校准次数  再赋值RTC
#define   GPS_CONFIG_Nuber_Size   10         //GPS上电配置次数

void NO_CONFIG_FILE_EXE(void) 
{
	u32 baud = 115200 ;      //波特率
	u8  res  = 0 ;           //动画节拍
//OLED提示串口初始化完成，以及打印波特率
	Movie_Show_Img(USART_INIT); //串口配置界面
	OLED_ShowNum(48,16,baud,6,16,1);	//显示串口波特率  显示
	delay_ms(1500);

  Uart1_Init(baud);	//串口初始化，抢占优先级0，子优先级2

//等待GPS板卡响应-----------------------------------------------
	while(GPS_CONFIG_TIMEA()&&(res<=GPS_CONFIG_Nuber_Size))  //配置GPS关闭所有输出  并配置打印时间数据帧  最多等待 5*12S
	{
		SYS_Scan_TF_Err();  //TF 卡检测
	  res ++;
		delay_ms(100);
	}
	
  if(res>=GPS_CONFIG_Nuber_Size)
	{
		Movie_Show_Img(GPS_ERR);    //显示错误界面
	  LED0 = 1;                      //熄灭
		while(1)
		{
		  LED1 =! LED1; 	//初始化失败，LED闪烁提示
		 delay_ms(300);
		}
	}
//--------------------------------------------------------------
//开始校准RTC  获取 GPS时间数据 --------------------------------=
//上电第一次时间校准及GPS定位------------------------------------------------------
//上电后需要等待足够时间  等待数据稳定---------------------------------------
	PLAY_GPS_Cartoon(0);                    //显示动画
	res = 0;
	GPS_BUF_clear_Init(GPS_BUF);   //全部清除
	LED0 = 1;
  LED1 = 0;  
	res = 0 ;               //动画节拍
  SET_RTC_Nuber( GPS_JUST_Nuber ) ;    //校准次数 赋值
	while(1)   //成功校准次数  完成后才退出
	{
//解析UTC时间并校准RTC------------------------------------------	
		if(RTC_CONFIG_FLAT==0) 	//等待校准完成
		{
        break;    //校准完成 退出
	  }
		else
		{
//解析GPS时间数据时增加动画效果------------------------------------------
		  res = (res+GPS_PLAY_LEN+1)%GPS_PLAY_LEN ;  //动画节拍变量  周期节拍  周期为 5    值为 0 至 4  
			LED0 = !LED0;
			LED1 = !LED1;
      PLAY_GPS_Cartoon(res);                    //显示动画
      delay_ms(700);
//---动画效果结束---------------------------------------------------
	 }
	//SD卡 拔出检测-----------------------------------
    SYS_Scan_TF_Err();  //TF 卡检测
	}
//RTC上电时间第一次数据分析时间数据校准完成   GPS定位完成-------------------------------------------------------------	
	 
	
//检测是否已经收到广播----------------------------------------------------------	
  if(Satellite_Broadcast_Reg != Satellite_Broadcast_FLAT)    //如果没有收到卫星广播   则要等待广播
	{
	  Wait_Satellite_Broadcast();   //等待卫星广播命令
	}
	
//上电第一次RTC时间GPS时间数据校准结束-------------------------------------------------------

	if( GPS_CONFIG_5HZ() != 0 ) 	 //配置GPS    如果配置失败
	{
		Movie_Show_Img(GPS_ERR);    //显示错误界面
	  LED0 = 1;                      //熄灭
		while(1)
		{
		  LED1 =! LED1; 	//初始化失败，LED闪烁提示
		  delay_ms(300);
		}
	}
//GPS板卡配置完成---------------------------------------------------------
}



void Wait_Satellite_Broadcast(void)  //等待卫星广播
{
	u8  res  = 0 ;                   //动画节拍
	u8  Play_buff[16] = { 0 } ;      //显存
	Movie_Show_Img(GPS_broadcast);   //   开始等待卫星广播界面
	
	RTC_WaitForLastTask();	         //等待最近一次对RTC寄存器的写操作完成
	RTC_Set(2016,9,12,0,0,0);        //设置起始时间  用于方便等待卫星广播时 时间获取
	RTC_WaitForLastTask();	         //等待最近一次对RTC寄存器的写操作完成
	
	delay_ms(1500);
  
//等待时间到达-----------------------------------------------
  while(1)
	{
	 //显示当前运行时间--------------------------------
		sprintf((char *)Play_buff,"    %02d:%02d:%02d    ",calendar.hour,calendar.min,calendar.sec) ;
		OLED_ShowString(0,0,Play_buff,16,1,1);
		
		//检测时间是否到达15分钟   开始设置 时间数据校准----------------
		if( calendar.min>= 1)   //到达15分钟
		{
		  if((res&0x80) == 0 )
			{
			  res |= 0x80 ;
				GPS_BUF_clear_Init(GPS_BUF);   //全部清除
        SET_RTC_Nuber( GPS_JUST_Nuber ) ;    //校准次数 赋值
			}
		 if(RTC_CONFIG_FLAT == 0 )  //校准完成
		 {
			//写入广播标记 ------------------------
			 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
       PWR_BackupAccessCmd(ENABLE);	                                            //使能后备寄存器访问 
       Satellite_Broadcast_Reg = Satellite_Broadcast_FLAT ;                     //写入已接收广播标记
		   break;
		 }
		}
		SYS_Scan_TF_Err();  //TF 卡检测
		delay_ms(300) ;
	}
}


