#include "GPS_DATE.h"
#include "Gps.h"
#include "GPS_STA.h"
#include "Led.h"
#include "SRAM_DATA.h"
#include "RTC.h"

void UTC_BUF_clear_Init(MY_UTC_typ *UTC_BUF)       //����������ݼ����
{
  UTC_BUF->UTC_NUM = 0;           //дָ������
	UTC_BUF->UTC_SEC = 0xff;        //Ϊ������ֵ
	UTC_BUF->UTC_STA = UTC_FREE;    //����
}

u16  UTC_BUF_Valid_NUM(MY_UTC_typ *UTC_BUF)         //���ص�ǰ������Ч���ݳ���
{
  return (u16)UTC_BUF->UTC_NUM;
}

UTC_STA_ENUM UTC_W_BUF_Byte(MY_UTC_typ *UTC_BUF,u8 date)                //д����
{
   if(UTC_BUF_Valid_NUM(UTC_BUF)>=UTC_BUF_LEN)
	 {
	   return UTC_ERR;                               //����
	 }
   
   UTC_BUF->UTC_BUF[UTC_BUF->UTC_NUM] = date;
	 

   UTC_BUF->UTC_NUM ++ ;

   return UTC_FREE;     //���ؿ���  �ɹ�
}

u8 *  UTC_BUF_RETURN(MY_UTC_typ *UTC_BUF)           //����UTC����ָ���ַ
{
   return (u8 *)(&UTC_BUF->UTC_BUF) ;
}

void  SET_UTC_BUF_OVER(MY_UTC_typ *UTC_BUF)         //�ڻ���β���������
{
  UTC_BUF->UTC_BUF[UTC_BUF_LEN] = '\0';    //ʵ�ʳ���Ϊ �� �� 1
}

u8 RETURN_LAST_SEC(MY_UTC_typ *UTC_BUF)             //�����ϴ�У׼��UTC������
{
  return UTC_BUF->UTC_SEC;
}

u8 UTC_TIME_Adjust(u8 *UTC_BUF)     //UTC����У׼ 
{
//	if((MY_NMEA_GNRMC_Analysis(UTC_TIME,(u8*)UTC_BUF)==0)&&(GPS_PV==0))	//GPS UTCʱ����Ϣ����   ���������ȷ
	if((MY_NMEA_GNRMC_Analysis(UTC_TIME,(u8*)UTC_BUF)==0))	
	{
		return  UTC_FREE;   //���ؿ���  �ɹ�
	}
  return  UTC_ERR;   //���ش���
}


u8 UTC_BUF_TIME_Adjust(MY_UTC_typ *UTC_BUF)    //ϵͳUTCʱ��У׼  ���� 0 ��ֱ�Ӹ�ֵUTC
{
	u8 sta = UTC_ERR;
  if((UTC_BUF->UTC_STA & UTC_Comple) == 0)  //���û���յ�һ֡����������
	{
	  return  UTC_ERR;   //���ش���
	}
	UTC_BUF->UTC_STA &=~UTC_Comple;  //���һ֡������ɱ��
	UTC_BUF->UTC_STA |= UTC_BUSY ;    //���æ
	if(UTC_BUF->UTC_STA & UTC_OK_FIR)   //���֮ǰ�Ѿ��ɹ�������һ֡UTCʱ��������
	{
	  if(UTC_TIME_Adjust(UTC_BUF_RETURN(UTC_BUF))==UTC_FREE )   //��������ɹ�
		{
		 if((RETURN_LAST_SEC(UTC_BUF)==59)&&( UTC_TIME->sec==0))
		 {
		  UTC_BUF_clear_Init(UTC_BUF);   //ȫ�����
			sta =  UTC_FREE;      //�ɹ�
		 }
		 else if((RETURN_LAST_SEC(UTC_BUF)+1)== UTC_TIME->sec)
		 {
			UTC_BUF_clear_Init(UTC_BUF);   //ȫ�����
			sta =  UTC_FREE;      //�ɹ�
		 }
		 else  if(RETURN_LAST_SEC(UTC_BUF)!= UTC_TIME->sec)  //����
		 {
			UTC_BUF_clear_Init(UTC_BUF);   //ȫ�����
			sta =  UTC_ERR;             //����
		 }
		}
		else
		{
		  sta =  UTC_OK_FIR;            //���ؽ���һ��
		}
	}
	else   //û�н�����   ��һ�ν���
	{
	 if(UTC_TIME_Adjust(UTC_BUF_RETURN(UTC_BUF))==UTC_FREE)   //��������ɹ�
	 {
		  UTC_BUF->UTC_NUM = 0;                //дָ������
		  UTC_BUF->UTC_SEC  = UTC_TIME->sec ;   //����˴�UTCʱ��������
	    UTC_BUF->UTC_STA |= UTC_OK_FIR ;     //���ý������һ�α��
	    sta =  UTC_OK_FIR ;         //���ؽ���һ��
	 }
	}
	UTC_BUF->UTC_STA &=~UTC_BUSY ;    //���æ���
	return  sta;
}

void UTC_BUF_TIME_Adjust_Task(MY_UTC_typ *UTC_BUF,u8 RX_BUF)   //UTCУ׼����
{ 
	if((UTC_BUF->UTC_STA&UTC_BUSY)==0)   //���UTC�������ڽ���
	{
		if(UTC_BUF->UTC_STA&UTC_Gp_F)   //����յ�֡ͷ
		{		
		 if(UTC_W_BUF_Byte(UTC_BUF,RX_BUF)!=UTC_FREE) //�������һ��  д�뻺��	  ���д�벻�ɹ�˵���Ѿ����յ�һ֡����������
		 {	
		   UTC_BUF->UTC_STA |=UTC_Comple;            //���ý�����ɱ��
       UTC_BUF->UTC_STA&=~UTC_Gp_F	;            //������յ�֡ͷ���			
       SET_UTC_BUF_OVER(UTC_BUF);				         //��������ַ�	
	   }
	  }
		else if(RX_BUF =='G')  //����յ�G �ַ�  ������֡ͷ
		{
	    UTC_BUF->UTC_NUM = 0;      //дָ������   ״̬�����治����
			UTC_W_BUF_Byte(UTC_BUF,RX_BUF);  //д�뻺��
		}
		else if(UTC_BUF_Valid_NUM(UTC_BUF)>0)  //�Ѿ����յ�G�ַ�
		{
			if(((RX_BUF =='P')||(RX_BUF =='N'))&&(UTC_BUF_Valid_NUM(UTC_BUF)==1))  //����ĵ�1��Ԫ�� Ϊ  N  ����  P
		  {
		    UTC_W_BUF_Byte(UTC_BUF,RX_BUF);  //д�뻺��	
		  }
		  else if((RX_BUF =='R')&&(UTC_BUF_Valid_NUM(UTC_BUF)==2)) //����ĵ�2��Ԫ�� Ϊ R
		  {
		    UTC_W_BUF_Byte(UTC_BUF,RX_BUF);  //д�뻺��	
		  }
		  else if((RX_BUF =='M')&&(UTC_BUF_Valid_NUM(UTC_BUF)==3)) //����ĵ�3��Ԫ�� Ϊ M
		  {
		    UTC_W_BUF_Byte(UTC_BUF,RX_BUF);  //д�뻺��
		  }
		  else if((RX_BUF =='C')&&(UTC_BUF_Valid_NUM(UTC_BUF)==4)) //����ĵ�4��Ԫ�� Ϊ C
		  {
		    UTC_W_BUF_Byte(UTC_BUF,RX_BUF);    //д�뻺��
				UTC_BUF->UTC_STA &=~UTC_Comple;    //���������ɱ��     ���յ�֡ͷ  ���������
				UTC_BUF->UTC_STA |= UTC_Gp_F  ;    //���ý��յ�֡ͷ���
		  }	
		}
	}
}

void UTC_TIME_AMEND(MY_UTC_TIME_typ *UTC_TIME)   //��UTC ʱ������ 0��8ʱ���ڲ�������
{
//ʱ������Сʱ����-----------------------------------------
	UTC_TIME->hour  +=   8;          //ʱ��ƫ��	
	if(UTC_TIME->hour>=24)           //��ʱΪ �賿0����8��֮��
	{
	  UTC_TIME->hour -=24;
		UTC_TIME->sday++;    //���ڼ�һ
		if(UTC_TIME->smon == 2 )  //�����2��  ��Ҫ�ж���ƽ��
		{
		  if(Is_Leap_Year(UTC_TIME->syear) ==1 ) //���������
			{
			   if(UTC_TIME->sday>29)    //��2��29��
				 {
				   UTC_TIME->sday = 1;
					 UTC_TIME->smon = 3;    //�ı��3��1��
				 }	 
			}
		  else   //ƽ��
			{
			 if(UTC_TIME->sday>28)    //û��2��29��
			 {
				   UTC_TIME->sday = 1;
					 UTC_TIME->smon = 3;    //�ı��3��1��
			 }			
			}
		}
	  else    //�����·�   ����2�·�
		{
		  if(UTC_TIME->sday>mon_table[UTC_TIME->smon])  //����Ѿ�������һ����
			{
			   UTC_TIME->sday = 1 ;
				 UTC_TIME->smon ++ ;  //�¼�һ
				if(UTC_TIME->smon>12)
				{
				  UTC_TIME->smon = 1 ;
					UTC_TIME->syear++  ; //���һ				
				}
			}	
		}
	}	
}


//����GNRMC����GPRMC��Ϣ
//MY_UTC_TIME_typ :  UTCʱ����Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
u8 MY_NMEA_GNRMC_Analysis(MY_UTC_TIME_typ *UTC_TIME,u8 *buf)     
{
	u8 dx;			 
	u8 posx;     
	u32 temp;	 
	posx=NMEA_Comma_Pos(buf,1);								//�õ�UTCʱ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(buf+posx,&dx)/NMEA_Pow(10,dx);	 	//�õ�UTCʱ��,ȥ��ms
		UTC_TIME->hour=(temp/10000+8)%24;  //ʱ���8  �� 24ȡ��   ʵ�ʴ�СΪ 0 ��23
		UTC_TIME->min=(temp/100)%100;
		UTC_TIME->sec=temp%100;	 	 
	}
  else  return 1;   //ʱ���������	

	posx=NMEA_Comma_Pos(buf,9);								//�õ�UTC����
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(buf+posx,&dx);		 				//�õ�UTC����
		UTC_TIME->sday=temp/10000;
		UTC_TIME->smon=(temp/100)%100;
		UTC_TIME->syear=2000+temp%100;	 	 
	}
 else return 1;   //ʱ���������
	
 UTC_TIME_AMEND(UTC_TIME);   //UTCʱ������  �����賿0����8�� ���ڲ�������
	return 0;  //������ȷ
}


//GPS ʱ�����ݽ���-----------------------------------------------------------------------------------------------------------------

u8 MY_NMEA_TIMEA_Analysis(MY_GPS_TIME_typ *GPS_TIME,u8 *buf)     //���� TIMEA ��֡����  �õ�UTCʱ�� ��GPSʱ��ƫ��
{
  u8 dx;			 
	u8 posx;     
	int temp;	 
	posx=NMEA_Comma_Pos(buf,4);								//ƫ�Ƶ�״̬��ǵ�ַ   FINE Ϊ��֡������ȷ
	if(posx!=0XFF)
	{
	  if(((buf[posx]=='F')&&(buf[posx+1]=='I')&&(buf[posx+2]=='N')&&(buf[posx+3]=='E'))==0)  //�����Ϊ FINE �ַ��������
		{
		   return 1 ;   //��֡���ݴ���
		}			
	}
  else  return 1;   //ʱ���������	

	posx=NMEA_Comma_Pos(buf,12);								//�õ�����ƫ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(buf+posx,&dx);
		temp/=(int)NMEA_Pow(10,dx);  //�õ�����ƫ��ֵ 	 
	}
 else return 1;   //ʱ���������	

	posx=NMEA_Comma_Pos(buf,13);								//�õ�UTC��ʱ��
	if(posx!=0XFF)
	{
		GPS_TIME->syear = NMEA_Str2num(buf+posx,&dx);  //�õ�UTC��ʱ��	 
	}
 else return 1;   //ʱ���������	

	posx=NMEA_Comma_Pos(buf,14);								//�õ�UTC��ʱ��
	if(posx!=0XFF)
	{
		GPS_TIME->smon =NMEA_Str2num(buf+posx,&dx);  	//�õ�UTC��ʱ��	 
	}
  else return 1;   //ʱ���������	

	posx=NMEA_Comma_Pos(buf,15);								//�õ�UTC ��ʱ��
	if(posx!=0XFF)
	{
		GPS_TIME->sday=NMEA_Str2num(buf+posx,&dx); //�õ�UTC ��ʱ��	 
	}
 else return 1;   //ʱ���������		
	
	posx=NMEA_Comma_Pos(buf,16);								//�õ�UTC Сʱʱ��
	if(posx!=0XFF)
	{
		GPS_TIME->hour=NMEA_Str2num(buf+posx,&dx); 		//�õ�UTC Сʱʱ�� 
	}
 else return 1;   //ʱ���������	
	
	posx=NMEA_Comma_Pos(buf,17);								//�õ�UTC ��ʱ��
	if(posx!=0XFF)
	{
		GPS_TIME->min=NMEA_Str2num(buf+posx,&dx); //�õ�UTC ��ʱ��	 
	}
 else return 1;   //ʱ���������	
	
	posx=NMEA_Comma_Pos(buf,18);								//�õ�UTC ��ʱ��
	if(posx!=0XFF)
	{
		GPS_TIME->sec=NMEA_Str2num(buf+posx,&dx)/1000; //�õ�UTC ��ʱ��	 
	}
 else return 1;   //ʱ���������	
	
  GPS_TIME_AMEND(GPS_TIME,temp);      //ʱ����������  UTC����������GPSʱ��  �������賿0����8�����ڴ���
	
	return 0;  //������ȷ	
}

void GPS_TIME_AMEND(MY_GPS_TIME_typ *GPS_TIME,int gps_utc_offset)   //��UTC ʱ�������� GPSʱ�� ����0��8ʱ���ڲ�������
{
  GPS_TIME->sec   -=  gps_utc_offset ;       //��ƫ������ 

//ʱ������������-----------------------------------
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
	
//ʱ������Сʱ����-----------------------------------------
	GPS_TIME->hour  +=   8;          //ʱ��ƫ��	
	if(GPS_TIME->hour>=24)           //��ʱΪ �賿0����8��֮��
	{
	  GPS_TIME->hour -=24;
		GPS_TIME->sday++;    //���ڼ�һ
		if(GPS_TIME->smon == 2 )  //�����2��  ��Ҫ�ж���ƽ��
		{
		  if(Is_Leap_Year(GPS_TIME->syear) ==1 ) //���������
			{
			   if(GPS_TIME->sday>29)    //��2��29��
				 {
				   GPS_TIME->sday = 1;
					 GPS_TIME->smon = 3;    //�ı��3��1��
				 }	 
			}
		  else   //ƽ��
			{
			 if(GPS_TIME->sday>28)    //û��2��29��
			 {
				   GPS_TIME->sday = 1;
					 GPS_TIME->smon = 3;    //�ı��3��1��
			 }			 
			}
		}
	  else    //�����·�   ����2�·�
		{
		  if(GPS_TIME->sday>mon_table[GPS_TIME->smon])  //����Ѿ�������һ����
			{
			   GPS_TIME->sday = 1 ;
				 GPS_TIME->smon ++ ;  //�¼�һ
				if(GPS_TIME->smon>12)
				{
				  GPS_TIME->smon = 1 ;
					GPS_TIME->syear++  ; //���һ				
				}
			}	
		}
	}	
}

void GPS_BUF_clear_Init(MY_GPS_typ *GPS_BUF)       //����������ݼ����
{
  GPS_BUF->GPS_NUM = 0;           //дָ������
	GPS_BUF->GPS_SEC = 0xff;        //Ϊ������ֵ
	GPS_BUF->GPS_STA = GPS_FREE;    //����
}

u16  GPS_BUF_Valid_NUM(MY_GPS_typ *GPS_BUF)         //���ص�ǰ������Ч���ݳ���
{
  return (u16)GPS_BUF->GPS_NUM;
}

GPS_STA_ENUM GPS_W_BUF_Byte(MY_GPS_typ *GPS_BUF,u8 date)                //д����
{
   if(GPS_BUF_Valid_NUM(GPS_BUF)>=GPS_BUF_LEN)
	 {
	   return GPS_ERRR;                               //����
	 }
   
   GPS_BUF->GPS_BUFF[GPS_BUF->GPS_NUM] = date;
	 

   GPS_BUF->GPS_NUM ++ ;

   return GPS_FREE;     //���ؿ���  �ɹ�
}

u8 * GPS_BUF_RETURN(MY_GPS_typ *GPS_BUF)           //����GPS����ָ���ַ
{
   return (u8 *)(&GPS_BUF->GPS_BUFF) ;
}

void  SET_GPS_BUF_OVER(MY_GPS_typ *GPS_BUF)         //�ڻ���β���������
{
  GPS_BUF->GPS_BUFF[GPS_BUF_LEN] = '\0';    //ʵ�ʳ���Ϊ �� �� 1
}

u8 RETURN_LAST_SEC_GPS(MY_GPS_typ *GPS_BUF)             //�����ϴ�У׼��UTC������
{
  return GPS_BUF->GPS_SEC;
}

u8 GPS_TIME_Adjust(u8 *GPS_BUF)     //UTC����У׼ 
{
//	if((MY_NMEA_TIMEA_Analysis(GPS_TIME,(u8*)GPS_BUF)==0)&&(GPS_PV==0))	//GPS UTCʱ����Ϣ����   ���������ȷ
	if((MY_NMEA_TIMEA_Analysis(GPS_TIME,(u8*)GPS_BUF)==0))
	{
		return  GPS_FREE;   //���ؿ���  �ɹ�
	}
  return  GPS_ERRR;   //���ش���
}

void GPS_BUF_TIME_Adjust_Task(MY_GPS_typ *GPS_BUF,u8 RX_BUF)   //UTCУ׼����
{ 
	if((GPS_BUF->GPS_STA&GPS_BUSY)==0)   //���GPS�������ڽ���
	{
		if(GPS_BUF->GPS_STA&UTC_Gp_F)   //����յ�֡ͷ
		{
     if(GPS_BUF->GPS_STA&UTC_OVER)	//����Ѿ��յ�0X0D  �������յ�0X0A ���ʾ����
		 {
		  if(RX_BUF==0X0A)
			{
			 GPS_BUF->GPS_STA |=GPS_Comple;            //���ý�����ɱ��
       GPS_BUF->GPS_STA&=~GPS_Gp_F	;            //������յ�֡ͷ���			
			}
			else  //����
			{
			 GPS_BUF_clear_Init(GPS_BUF);    //������б��
			}
		 }
		 else  //û���յ� 0X0D
		 {	  
       if(RX_BUF==0X0D)  //�յ���һ���������
			 {
			   GPS_BUF->GPS_STA |= UTC_OVER ;  //���ý��յ�0X0D���	 
			 }
       else
			 {		 
		    if(GPS_W_BUF_Byte(GPS_BUF,RX_BUF)!=GPS_FREE) //�������һ��  д�뻺��	  ���д�벻�ɹ�˵�������Ѿ�д����   ���½���
		    {	
					GPS_BUF_clear_Init(GPS_BUF);    //������б��
	      }
		   }
	   }
	  }
		else if(RX_BUF =='T')  //����յ�T�ַ�  ������֡ͷ
		{
	    GPS_BUF->GPS_NUM = 0;      //дָ������   ״̬�����治����
			GPS_W_BUF_Byte(GPS_BUF,RX_BUF);  //д�뻺��
		}
		else if(GPS_BUF_Valid_NUM(GPS_BUF)>0)  //�Ѿ����յ�T�ַ�
		{
			if((RX_BUF =='I')&&(GPS_BUF_Valid_NUM(GPS_BUF)==1))  //����ĵ�1��Ԫ�� Ϊ I
		  {
		    GPS_W_BUF_Byte(GPS_BUF,RX_BUF);  //д�뻺��	
		  }
		  else if((RX_BUF =='M')&&(GPS_BUF_Valid_NUM(GPS_BUF)==2)) //����ĵ�2��Ԫ�� Ϊ M
		  {
		    GPS_W_BUF_Byte(GPS_BUF,RX_BUF);  //д�뻺��	
		  }
		  else if((RX_BUF =='E')&&(GPS_BUF_Valid_NUM(GPS_BUF)==3)) //����ĵ�3��Ԫ�� Ϊ E
		  {
		    GPS_W_BUF_Byte(GPS_BUF,RX_BUF);  //д�뻺��
		  }
		  else if((RX_BUF =='A')&&(GPS_BUF_Valid_NUM(GPS_BUF)==4)) //����ĵ�4��Ԫ�� Ϊ A
		  {
		    GPS_W_BUF_Byte(GPS_BUF,RX_BUF);    //д�뻺��
				GPS_BUF->GPS_STA &=~GPS_Comple;    //���������ɱ��     ���յ�֡ͷ  ���������
				GPS_BUF->GPS_STA |= GPS_Gp_F  ;    //���ý��յ�֡ͷ���
		  }	
		}
	}
}

u8 GPS_BUF_TIME_Adjust(MY_GPS_typ *GPS_BUF)    //ϵͳUTCʱ��У׼  ���� 0 ��ֱ�Ӹ�ֵUTC
{
	u8 sta = GPS_ERRR;
  if((GPS_BUF->GPS_STA & GPS_Comple) == 0)  //���û���յ�һ֡����������
	{
	  return  GPS_ERRR;   //���ش���
	}
	GPS_BUF->GPS_STA &=~GPS_Comple;  //���һ֡������ɱ��
	GPS_BUF->GPS_STA |= GPS_BUSY ;   //���æ
	
  sta =  GPS_TIME_Adjust(GPS_BUF_RETURN(GPS_BUF));   //����ʱ������
	GPS_BUF->GPS_STA &=~GPS_BUSY ;    //���æ���
	GPS_BUF_clear_Init(GPS_BUF);    //������б��

	return  sta;
}

