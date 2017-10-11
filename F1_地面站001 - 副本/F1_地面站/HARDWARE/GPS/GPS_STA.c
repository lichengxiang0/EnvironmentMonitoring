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



const u8 *GPS_CMM_5HZ[GPS_COMM_CONFIG_Nuber] ={            //5HZ 配置指令集
 "log com1 rangecmpb ontime 0.2\r\n"     ,          //输出5Hz的原始观测数据               
 "log com1 gpsephemb ontime 60\r\n"       ,          //输出gps星历文件
 "log com1 gloephemerisb ontime 60\r\n"   ,          //输出输出GLONASS星历文件
 "log com1 bd2ephemb ontime 60\r\n"       ,          //输出北斗星历文件
 "log com1 bestposb ontime 0.2 \r\n"       ,          //输出定位1Hz定位语句 
} ;


  
const u8 *GPS_REST_CMM           =  "freset\r\n" ;                //GPS恢复成出厂设置命令
const u8 *GPS_TIME_CMM           =  "LOG TIMEA ONTIME 1\r\n"   ;  //GPS配置输出GPS时间数据
const u8 *GPS_UNLOGALL_CMM       =  "UNLOGALL COM1 TRUE\r\n"   ;  //GPS配置关闭所有串口打印输出
const u8 *GPS_SAVE_CMM           =  "saveconfig\r\n"           ;  //保存GPS配置

void GPS_PV_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(GPS_PV_RCC,ENABLE);//使能时钟

	GPIO_InitStructure.GPIO_Pin  = GPS_PV_PIN;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //设置浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
 	GPIO_Init(GPS_PV_, &GPIO_InitStructure);	
}

u8 GPS_CONFIG_RESET(void)  //GPS复位   返回 0成功   1失败
{
	u8 i = 0 ;
	u8 TESTBUF[9] ={0};     //串口命令反馈缓存
  Uart1_send_str((u8*)GPS_REST_CMM,strlen((const char *)GPS_REST_CMM));   //发送复位
	FIFO_CLR_BUF(RX_BUF_FIFO);        //清空FIFO
	delay_ms(500);     //等待命令回传完成
	
	if(FIFO_BUF_Valid_NUM(RX_BUF_FIFO)<64)   //复位指令回传字节数  大约估计
	{
	  return 1 ;       //失败
	}
	FIFO_CLR_BUF(RX_BUF_FIFO);        //清空FIFO
	
	while(i<40)    //等待复位完成   接收 [COM1] 字符串  等待 20 S
	{
	 if(FIFO_BUF_Valid_NUM(RX_BUF_FIFO)>=8)  //字符串最小长度
	 {
	   FIFO_READ_BUF(RX_BUF_FIFO,TESTBUF,8);  //读取缓存
	   TESTBUF[8] = '\0';   //添加结束符
		 if(strcmp((const char*)TESTBUF,(const char*)"[COM1]\r\n")==0)   //比较
		 {
       return 0 ;         //复位成功！
		 }
		 else
		 {
		   return 1 ;         //如果不相等 则失败
		 }	
	 }
	 delay_ms(500);
	 i++;    //此处可加显示 及动画效果
	}
	return 1 ;         //超时 返回失败
}

#define GPS_REST_MAX   60   //复位失败次数   大约 12 S

u8 GPS_CONFIG_5HZ(void)   //配置成 5HZ   返回0成功   1失败
{
	u8 i = 0 ;
	while(GPS_UNLOGALL_CONFIG()&&(i<GPS_REST_MAX)) 
	{  
	   i ++;
		delay_ms(200) ;
	}
	if(i>=GPS_REST_MAX)
	{
	  return 1;  //失败
	}
	
  for(i=0;i<GPS_COMM_CONFIG_Nuber;i++)
	{
	 Uart1_send_str((u8*)GPS_CMM_5HZ[i],strlen((const char *)GPS_CMM_5HZ[i]));
	}
	return 0 ;    //成功
}

u8 GPS_UNLOGALL_CONFIG(void)  //GPS关闭所有输出   0成功    1失败
{
	FIFO_CLR_BUF(RX_BUF_FIFO);        //清空FIFO
  Uart1_send_str((u8*)GPS_UNLOGALL_CMM,strlen((const char *)GPS_UNLOGALL_CMM)); //发送关闭所有输出
	delay_ms(500) ;  //等待接收完成
	
	if(FIFO_BUF_Valid_NUM(RX_BUF_FIFO)>=30)  //实际响应为 32字节
	{
	  if((RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-4]==0X4F)&&(RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-3]==0X4B))  //倒数第3和第4个  为 OK 字符
	  {
		  return 0;  //返回成功
		}
	  return 1;
	}
	
	else if(FIFO_BUF_Valid_NUM(RX_BUF_FIFO)>=10)
	{
		if((RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-10]==0X4F)&&(RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-9]==0X4B))  //倒数第3和第4个  为 OK 字符
	  {
		  return 0;  //返回成功
		}
	  return 1;
	}
	return 1 ;
}

u8 GPS_CONFIG_TIMEA(void)  //串口输出时间数据
{
  u8 i = 0;
	while(GPS_UNLOGALL_CONFIG()&&(i<GPS_REST_MAX))
	{  
	   i ++;
		delay_ms(200) ;
	}
	
	if(i>=GPS_REST_MAX)
	{
	  return 1;  //失败
	}
//	GPS_SAVE_CONFIG();  //保存配置
	Uart1_send_str((u8*)GPS_TIME_CMM,strlen((const char *)GPS_TIME_CMM));
	return 0;  //成功
}

u8 GPS_SAVE_CONFIG(void)  //保存配置
{
	FIFO_CLR_BUF(RX_BUF_FIFO);         //清空FIFO
  Uart1_send_str((u8*)GPS_SAVE_CMM,strlen((const char *)GPS_SAVE_CMM)); //发送保存配置
	delay_ms(20) ;  //等待接收完成
	
	if(FIFO_BUF_Valid_NUM(RX_BUF_FIFO)>=10)  
	{
	  if((RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-4]==0X4F)&&(RX_BUF_FIFO->FIFO_BUFF[FIFO_BUF_Valid_NUM(RX_BUF_FIFO)-3]==0X4B))  //倒数第3和第4个  为 OK 字符
	  {
		  return 0;  //返回成功
		}
	  return 1;
	}
	return 1 ;
}
