#include "Txt.h"
#include "Rtc.h"
#include "stdio.h"
#include "Malloc.h"  
#include "string.h"
#include "SRAM_DATA.h"

//ͨ��ʱ���ȡ�ļ���
//������sd������,��֧��flash disk����
//��ϳ�:����"0:20120321210633.txt"���ļ���
void Moving_Station_New_File(u8 *pname)  //GPS �ļ�������
{	 
	RTC_Get();//����ʱ��
 	sprintf((char*)pname,"0:/%s/Moving_Station_%04d%02d%02d%02d%02d%02d.txt",Folder_Name,calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
} 

void Base_Station_New_File(u8 *pname)  //GPS �ļ�������
{	 
	RTC_Get();//����ʱ��
 	sprintf((char*)pname,"0:/%s/Base_Station_%04d%02d%02d%02d%02d%02d.txt",Folder_Name,calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
} 

void Aerial_Point_New_File(u8 *pname)
{
	RTC_Get();//����ʱ��
 	sprintf((char*)pname,"0:/%s/Aerial_Point_%04d%02d%02d%02d%02d%02d.txt",Folder_Name,calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
}
void Icm20602_New_File(u8 *pname)  //GPS �ļ�������
{	 
	RTC_Get();//����ʱ��
 	sprintf((char*)pname,"0:/%s/Icm20602_%04d%02d%02d%02d%02d%02d.txt",Folder_Name,calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
} 
void laser_New_File(u8 *pname)  //GPS �ļ�������
{	 
	RTC_Get();//����ʱ��
 	sprintf((char*)pname,"0:/%s/laster_%04d%02d%02d%02d%02d%02d.txt",Folder_Name,calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
} 
void New_Folder(u8 *pname)
{
	RTC_Get();//����ʱ��
	sprintf((char*)pname,"%04d%02d%02d%02d%02d%02d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
}

















