#include "GPS_DATE.h"
#include "Gps.h"
#include "GPS_STA.h"
#include "Led.h"
#include "SRAM_DATA.h"
#include "RTC.h"

void UTC_BUF_clear_Init(MY_UTC_typ *UTC_BUF)       //清除所以数据及标记
{
  UTC_BUF->UTC_NUM = 0;           //写指针清零
	UTC_BUF->UTC_SEC = 0xff;        //为不可能值
	UTC_BUF->UTC_STA = UTC_FREE;    //空闲
}

u16  UTC_BUF_Valid_NUM(MY_UTC_typ *UTC_BUF)         //返回当前缓存有效数据长度
{
  return (u16)UTC_BUF->UTC_NUM;
}

UTC_STA_ENUM UTC_W_BUF_Byte(MY_UTC_typ *UTC_BUF,u8 date)                //写数据
{
   if(UTC_BUF_Valid_NUM(UTC_BUF)>=UTC_BUF_LEN)
	 {
	   return UTC_ERR;                               //已满
	 }
   
   UTC_BUF->UTC_BUF[UTC_BUF->UTC_NUM] = date;
	 

   UTC_BUF->UTC_NUM ++ ;

   return UTC_FREE;     //返回空闲  成功
}

u8 *  UTC_BUF_RETURN(MY_UTC_typ *UTC_BUF)           //返回UTC缓存指针地址
{
   return (u8 *)(&UTC_BUF->UTC_BUF) ;
}

void  SET_UTC_BUF_OVER(MY_UTC_typ *UTC_BUF)         //在缓存尾加入结束符
{
  UTC_BUF->UTC_BUF[UTC_BUF_LEN] = '\0';    //实际长度为 宏 加 1
}

u8 RETURN_LAST_SEC(MY_UTC_typ *UTC_BUF)             //返回上次校准的UTC秒数据
{
  return UTC_BUF->UTC_SEC;
}

u8 UTC_TIME_Adjust(u8 *UTC_BUF)     //UTC数据校准 
{
//	if((MY_NMEA_GNRMC_Analysis(UTC_TIME,(u8*)UTC_BUF)==0)&&(GPS_PV==0))	//GPS UTC时间信息解析   如果解析正确
	if((MY_NMEA_GNRMC_Analysis(UTC_TIME,(u8*)UTC_BUF)==0))	
	{
		return  UTC_FREE;   //返回空闲  成功
	}
  return  UTC_ERR;   //返回错误
}


u8 UTC_BUF_TIME_Adjust(MY_UTC_typ *UTC_BUF)    //系统UTC时间校准  返回 0 可直接赋值UTC
{
	u8 sta = UTC_ERR;
  if((UTC_BUF->UTC_STA & UTC_Comple) == 0)  //如果没有收到一帧完整的数据
	{
	  return  UTC_ERR;   //返回错误
	}
	UTC_BUF->UTC_STA &=~UTC_Comple;  //清除一帧数据完成标记
	UTC_BUF->UTC_STA |= UTC_BUSY ;    //标记忙
	if(UTC_BUF->UTC_STA & UTC_OK_FIR)   //如果之前已经成功解析了一帧UTC时间数据了
	{
	  if(UTC_TIME_Adjust(UTC_BUF_RETURN(UTC_BUF))==UTC_FREE )   //如果解析成功
		{
		 if((RETURN_LAST_SEC(UTC_BUF)==59)&&( UTC_TIME->sec==0))
		 {
		  UTC_BUF_clear_Init(UTC_BUF);   //全部清除
			sta =  UTC_FREE;      //成功
		 }
		 else if((RETURN_LAST_SEC(UTC_BUF)+1)== UTC_TIME->sec)
		 {
			UTC_BUF_clear_Init(UTC_BUF);   //全部清除
			sta =  UTC_FREE;      //成功
		 }
		 else  if(RETURN_LAST_SEC(UTC_BUF)!= UTC_TIME->sec)  //舍弃
		 {
			UTC_BUF_clear_Init(UTC_BUF);   //全部清除
			sta =  UTC_ERR;             //错误
		 }
		}
		else
		{
		  sta =  UTC_OK_FIR;            //返回解析一次
		}
	}
	else   //没有解析过   第一次解析
	{
	 if(UTC_TIME_Adjust(UTC_BUF_RETURN(UTC_BUF))==UTC_FREE)   //如果解析成功
	 {
		  UTC_BUF->UTC_NUM = 0;                //写指针清零
		  UTC_BUF->UTC_SEC  = UTC_TIME->sec ;   //保存此次UTC时间秒数据
	    UTC_BUF->UTC_STA |= UTC_OK_FIR ;     //设置解析完成一次标记
	    sta =  UTC_OK_FIR ;         //返回解析一次
	 }
	}
	UTC_BUF->UTC_STA &=~UTC_BUSY ;    //清除忙标记
	return  sta;
}

void UTC_BUF_TIME_Adjust_Task(MY_UTC_typ *UTC_BUF,u8 RX_BUF)   //UTC校准任务
{ 
	if((UTC_BUF->UTC_STA&UTC_BUSY)==0)   //如果UTC缓存正在解析
	{
		if(UTC_BUF->UTC_STA&UTC_Gp_F)   //如果收到帧头
		{		
		 if(UTC_W_BUF_Byte(UTC_BUF,RX_BUF)!=UTC_FREE) //（缓存加一）  写入缓存	  如果写入不成功说明已经接收到一帧完整数据了
		 {	
		   UTC_BUF->UTC_STA |=UTC_Comple;            //设置接收完成标记
       UTC_BUF->UTC_STA&=~UTC_Gp_F	;            //清除接收到帧头标记			
       SET_UTC_BUF_OVER(UTC_BUF);				         //加入结束字符	
	   }
	  }
		else if(RX_BUF =='G')  //如果收到G 字符  可能是帧头
		{
	    UTC_BUF->UTC_NUM = 0;      //写指针清零   状态及缓存不处理
			UTC_W_BUF_Byte(UTC_BUF,RX_BUF);  //写入缓存
		}
		else if(UTC_BUF_Valid_NUM(UTC_BUF)>0)  //已经接收到G字符
		{
			if(((RX_BUF =='P')||(RX_BUF =='N'))&&(UTC_BUF_Valid_NUM(UTC_BUF)==1))  //缓存的第1个元素 为  N  或者  P
		  {
		    UTC_W_BUF_Byte(UTC_BUF,RX_BUF);  //写入缓存	
		  }
		  else if((RX_BUF =='R')&&(UTC_BUF_Valid_NUM(UTC_BUF)==2)) //缓存的第2个元素 为 R
		  {
		    UTC_W_BUF_Byte(UTC_BUF,RX_BUF);  //写入缓存	
		  }
		  else if((RX_BUF =='M')&&(UTC_BUF_Valid_NUM(UTC_BUF)==3)) //缓存的第3个元素 为 M
		  {
		    UTC_W_BUF_Byte(UTC_BUF,RX_BUF);  //写入缓存
		  }
		  else if((RX_BUF =='C')&&(UTC_BUF_Valid_NUM(UTC_BUF)==4)) //缓存的第4个元素 为 C
		  {
		    UTC_W_BUF_Byte(UTC_BUF,RX_BUF);    //写入缓存
				UTC_BUF->UTC_STA &=~UTC_Comple;    //清除接收完成标记     已收到帧头  不允许解析
				UTC_BUF->UTC_STA |= UTC_Gp_F  ;    //设置接收到帧头标记
		  }	
		}
	}
}

void UTC_TIME_AMEND(MY_UTC_TIME_typ *UTC_TIME)   //把UTC 时间修正 0至8时日期不对问题
{
//时间数据小时修正-----------------------------------------
	UTC_TIME->hour  +=   8;          //时区偏差	
	if(UTC_TIME->hour>=24)           //此时为 凌晨0点至8点之间
	{
	  UTC_TIME->hour -=24;
		UTC_TIME->sday++;    //日期加一
		if(UTC_TIME->smon == 2 )  //如果是2月  需要判断润平年
		{
		  if(Is_Leap_Year(UTC_TIME->syear) ==1 ) //如果是闰年
			{
			   if(UTC_TIME->sday>29)    //有2月29日
				 {
				   UTC_TIME->sday = 1;
					 UTC_TIME->smon = 3;    //改变成3月1日
				 }	 
			}
		  else   //平年
			{
			 if(UTC_TIME->sday>28)    //没有2月29日
			 {
				   UTC_TIME->sday = 1;
					 UTC_TIME->smon = 3;    //改变成3月1日
			 }			
			}
		}
	  else    //其它月份   不是2月份
		{
		  if(UTC_TIME->sday>mon_table[UTC_TIME->smon])  //如果已经到了下一个月
			{
			   UTC_TIME->sday = 1 ;
				 UTC_TIME->smon ++ ;  //月加一
				if(UTC_TIME->smon>12)
				{
				  UTC_TIME->smon = 1 ;
					UTC_TIME->syear++  ; //年加一				
				}
			}	
		}
	}	
}


//分析GNRMC或者GPRMC信息
//MY_UTC_TIME_typ :  UTC时间信息结构体
//buf:接收到的GPS数据缓冲区首地址
u8 MY_NMEA_GNRMC_Analysis(MY_UTC_TIME_typ *UTC_TIME,u8 *buf)     
{
	u8 dx;			 
	u8 posx;     
	u32 temp;	 
	posx=NMEA_Comma_Pos(buf,1);								//得到UTC时间
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(buf+posx,&dx)/NMEA_Pow(10,dx);	 	//得到UTC时间,去掉ms
		UTC_TIME->hour=(temp/10000+8)%24;  //时间加8  对 24取余   实际大小为 0 至23
		UTC_TIME->min=(temp/100)%100;
		UTC_TIME->sec=temp%100;	 	 
	}
  else  return 1;   //时间解析错误	

	posx=NMEA_Comma_Pos(buf,9);								//得到UTC日期
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(buf+posx,&dx);		 				//得到UTC日期
		UTC_TIME->sday=temp/10000;
		UTC_TIME->smon=(temp/100)%100;
		UTC_TIME->syear=2000+temp%100;	 	 
	}
 else return 1;   //时间解析错误
	
 UTC_TIME_AMEND(UTC_TIME);   //UTC时间修正  修正凌晨0点至8点 日期不对问题
	return 0;  //解析正确
}


//GPS 时间数据解析-----------------------------------------------------------------------------------------------------------------

u8 MY_NMEA_TIMEA_Analysis(MY_GPS_TIME_typ *GPS_TIME,u8 *buf)     //解析 TIMEA 此帧数据  得到UTC时间 及GPS时间偏差
{
  u8 dx;			 
	u8 posx;     
	int temp;	 
	posx=NMEA_Comma_Pos(buf,4);								//偏移到状态标记地址   FINE 为此帧数据正确
	if(posx!=0XFF)
	{
	  if(((buf[posx]=='F')&&(buf[posx+1]=='I')&&(buf[posx+2]=='N')&&(buf[posx+3]=='E'))==0)  //如果不为 FINE 字符串则错误
		{
		   return 1 ;   //此帧数据错误
		}			
	}
  else  return 1;   //时间解析错误	

	posx=NMEA_Comma_Pos(buf,12);								//得到闰秒偏移
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(buf+posx,&dx);
		temp/=(int)NMEA_Pow(10,dx);  //得到闰秒偏移值 	 
	}
 else return 1;   //时间解析错误	

	posx=NMEA_Comma_Pos(buf,13);								//得到UTC年时间
	if(posx!=0XFF)
	{
		GPS_TIME->syear = NMEA_Str2num(buf+posx,&dx);  //得到UTC年时间	 
	}
 else return 1;   //时间解析错误	

	posx=NMEA_Comma_Pos(buf,14);								//得到UTC月时间
	if(posx!=0XFF)
	{
		GPS_TIME->smon =NMEA_Str2num(buf+posx,&dx);  	//得到UTC月时间	 
	}
  else return 1;   //时间解析错误	

	posx=NMEA_Comma_Pos(buf,15);								//得到UTC 日时间
	if(posx!=0XFF)
	{
		GPS_TIME->sday=NMEA_Str2num(buf+posx,&dx); //得到UTC 日时间	 
	}
 else return 1;   //时间解析错误		
	
	posx=NMEA_Comma_Pos(buf,16);								//得到UTC 小时时间
	if(posx!=0XFF)
	{
		GPS_TIME->hour=NMEA_Str2num(buf+posx,&dx); 		//得到UTC 小时时间 
	}
 else return 1;   //时间解析错误	
	
	posx=NMEA_Comma_Pos(buf,17);								//得到UTC 分时间
	if(posx!=0XFF)
	{
		GPS_TIME->min=NMEA_Str2num(buf+posx,&dx); //得到UTC 分时间	 
	}
 else return 1;   //时间解析错误	
	
	posx=NMEA_Comma_Pos(buf,18);								//得到UTC 秒时间
	if(posx!=0XFF)
	{
		GPS_TIME->sec=NMEA_Str2num(buf+posx,&dx)/1000; //得到UTC 秒时间	 
	}
 else return 1;   //时间解析错误	
	
  GPS_TIME_AMEND(GPS_TIME,temp);      //时间数据修正  UTC数据修正成GPS时间  并修正凌晨0点至8点日期错误
	
	return 0;  //解析正确	
}

void GPS_TIME_AMEND(MY_GPS_TIME_typ *GPS_TIME,int gps_utc_offset)   //把UTC 时间修正成 GPS时间 包括0至8时日期不对问题
{
  GPS_TIME->sec   -=  gps_utc_offset ;       //秒偏移修正 

//时间数据秒修正-----------------------------------
	if(GPS_TIME->sec>=60)
	{
	  GPS_TIME->sec =  GPS_TIME->sec - 60 ;
		GPS_TIME->min++;
		if(GPS_TIME->min>=60)
		{
		  GPS_TIME->min = 0;
			GPS_TIME->hour ++;
		}
	}
	
//时间数据小时修正-----------------------------------------
	GPS_TIME->hour  +=   8;          //时区偏差	
	if(GPS_TIME->hour>=24)           //此时为 凌晨0点至8点之间
	{
	  GPS_TIME->hour -=24;
		GPS_TIME->sday++;    //日期加一
		if(GPS_TIME->smon == 2 )  //如果是2月  需要判断润平年
		{
		  if(Is_Leap_Year(GPS_TIME->syear) ==1 ) //如果是闰年
			{
			   if(GPS_TIME->sday>29)    //有2月29日
				 {
				   GPS_TIME->sday = 1;
					 GPS_TIME->smon = 3;    //改变成3月1日
				 }	 
			}
		  else   //平年
			{
			 if(GPS_TIME->sday>28)    //没有2月29日
			 {
				   GPS_TIME->sday = 1;
					 GPS_TIME->smon = 3;    //改变成3月1日
			 }			 
			}
		}
	  else    //其它月份   不是2月份
		{
		  if(GPS_TIME->sday>mon_table[GPS_TIME->smon])  //如果已经到了下一个月
			{
			   GPS_TIME->sday = 1 ;
				 GPS_TIME->smon ++ ;  //月加一
				if(GPS_TIME->smon>12)
				{
				  GPS_TIME->smon = 1 ;
					GPS_TIME->syear++  ; //年加一				
				}
			}	
		}
	}	
}

void GPS_BUF_clear_Init(MY_GPS_typ *GPS_BUF)       //清除所有数据及标记
{
  GPS_BUF->GPS_NUM = 0;           //写指针清零
	GPS_BUF->GPS_SEC = 0xff;        //为不可能值
	GPS_BUF->GPS_STA = GPS_FREE;    //空闲
}

u16  GPS_BUF_Valid_NUM(MY_GPS_typ *GPS_BUF)         //返回当前缓存有效数据长度
{
  return (u16)GPS_BUF->GPS_NUM;
}

GPS_STA_ENUM GPS_W_BUF_Byte(MY_GPS_typ *GPS_BUF,u8 date)                //写数据
{
   if(GPS_BUF_Valid_NUM(GPS_BUF)>=GPS_BUF_LEN)
	 {
	   return GPS_ERRR;                               //已满
	 }
   
   GPS_BUF->GPS_BUFF[GPS_BUF->GPS_NUM] = date;
	 

   GPS_BUF->GPS_NUM ++ ;

   return GPS_FREE;     //返回空闲  成功
}

u8 * GPS_BUF_RETURN(MY_GPS_typ *GPS_BUF)           //返回GPS缓存指针地址
{
   return (u8 *)(&GPS_BUF->GPS_BUFF) ;
}

void  SET_GPS_BUF_OVER(MY_GPS_typ *GPS_BUF)         //在缓存尾加入结束符
{
  GPS_BUF->GPS_BUFF[GPS_BUF_LEN] = '\0';    //实际长度为 宏 加 1
}

u8 RETURN_LAST_SEC_GPS(MY_GPS_typ *GPS_BUF)             //返回上次校准的UTC秒数据
{
  return GPS_BUF->GPS_SEC;
}

u8 GPS_TIME_Adjust(u8 *GPS_BUF)     //UTC数据校准 
{
//	if((MY_NMEA_TIMEA_Analysis(GPS_TIME,(u8*)GPS_BUF)==0)&&(GPS_PV==0))	//GPS UTC时间信息解析   如果解析正确
	if((MY_NMEA_TIMEA_Analysis(GPS_TIME,(u8*)GPS_BUF)==0))
	{
		return  GPS_FREE;   //返回空闲  成功
	}
  return  GPS_ERRR;   //返回错误
}

void GPS_BUF_TIME_Adjust_Task(MY_GPS_typ *GPS_BUF,u8 RX_BUF)   //UTC校准任务
{ 
	if((GPS_BUF->GPS_STA&GPS_BUSY)==0)   //如果GPS缓存正在解析
	{
		if(GPS_BUF->GPS_STA&UTC_Gp_F)   //如果收到帧头
		{
     if(GPS_BUF->GPS_STA&UTC_OVER)	//如果已经收到0X0D  后面再收到0X0A 则表示结束
		 {
		  if(RX_BUF==0X0A)
			{
			 GPS_BUF->GPS_STA |=GPS_Comple;            //设置接收完成标记
       GPS_BUF->GPS_STA&=~GPS_Gp_F	;            //清除接收到帧头标记			
			}
			else  //错误
			{
			 GPS_BUF_clear_Init(GPS_BUF);    //清除所有标记
			}
		 }
		 else  //没有收到 0X0D
		 {	  
       if(RX_BUF==0X0D)  //收到第一个结束标记
			 {
			   GPS_BUF->GPS_STA |= UTC_OVER ;  //设置接收到0X0D标记	 
			 }
       else
			 {		 
		    if(GPS_W_BUF_Byte(GPS_BUF,RX_BUF)!=GPS_FREE) //（缓存加一）  写入缓存	  如果写入不成功说明缓存已经写满了   重新接收
		    {	
					GPS_BUF_clear_Init(GPS_BUF);    //清除所有标记
	      }
		   }
	   }
	  }
		else if(RX_BUF =='T')  //如果收到T字符  可能是帧头
		{
	    GPS_BUF->GPS_NUM = 0;      //写指针清零   状态及缓存不处理
			GPS_W_BUF_Byte(GPS_BUF,RX_BUF);  //写入缓存
		}
		else if(GPS_BUF_Valid_NUM(GPS_BUF)>0)  //已经接收到T字符
		{
			if((RX_BUF =='I')&&(GPS_BUF_Valid_NUM(GPS_BUF)==1))  //缓存的第1个元素 为 I
		  {
		    GPS_W_BUF_Byte(GPS_BUF,RX_BUF);  //写入缓存	
		  }
		  else if((RX_BUF =='M')&&(GPS_BUF_Valid_NUM(GPS_BUF)==2)) //缓存的第2个元素 为 M
		  {
		    GPS_W_BUF_Byte(GPS_BUF,RX_BUF);  //写入缓存	
		  }
		  else if((RX_BUF =='E')&&(GPS_BUF_Valid_NUM(GPS_BUF)==3)) //缓存的第3个元素 为 E
		  {
		    GPS_W_BUF_Byte(GPS_BUF,RX_BUF);  //写入缓存
		  }
		  else if((RX_BUF =='A')&&(GPS_BUF_Valid_NUM(GPS_BUF)==4)) //缓存的第4个元素 为 A
		  {
		    GPS_W_BUF_Byte(GPS_BUF,RX_BUF);    //写入缓存
				GPS_BUF->GPS_STA &=~GPS_Comple;    //清除接收完成标记     已收到帧头  不允许解析
				GPS_BUF->GPS_STA |= GPS_Gp_F  ;    //设置接收到帧头标记
		  }	
		}
	}
}

u8 GPS_BUF_TIME_Adjust(MY_GPS_typ *GPS_BUF)    //系统UTC时间校准  返回 0 可直接赋值UTC
{
	u8 sta = GPS_ERRR;
  if((GPS_BUF->GPS_STA & GPS_Comple) == 0)  //如果没有收到一帧完整的数据
	{
	  return  GPS_ERRR;   //返回错误
	}
	GPS_BUF->GPS_STA &=~GPS_Comple;  //清除一帧数据完成标记
	GPS_BUF->GPS_STA |= GPS_BUSY ;   //标记忙
	
  sta =  GPS_TIME_Adjust(GPS_BUF_RETURN(GPS_BUF));   //解析时间数据
	GPS_BUF->GPS_STA &=~GPS_BUSY ;    //清除忙标记
	GPS_BUF_clear_Init(GPS_BUF);    //清除所有标记

	return  sta;
}

