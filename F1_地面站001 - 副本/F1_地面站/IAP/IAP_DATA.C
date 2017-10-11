#include "IAP_DATA.H"
#include "Malloc.h"
#include "sdio_sdcard.h"
#include "OLED_GUI.h"
#include "STM32_CRC.h"
#include "stmflash.h"

const u8  *Updata_Path  ="0:/Updata/RTK01_DMZ.bin";     //更新文件路径

void Write_FLASH_APP_CRC_DATA(IAP_CRC_DATA_TYP * IAP_CRC_DAT )  //写FLASH数据
{
	FLASH_Unlock();						//解锁
  STMFLASH_Write_NoCheck(APP_DATA_START_Adress,(u16 * )IAP_CRC_DAT,sizeof(IAP_CRC_DATA_TYP)/2); //写入FLASH
 	FLASH_Lock();//上锁
}

void ErasePage_FLASH_APP_CRC_DATA(void)    //擦除IAP CRC数据
{
	FLASH_Unlock();						//解锁
  FLASH_ErasePage(APP_DATA_START_Adress);//擦除这个扇区
  FLASH_Lock();//上锁
}

void ErasePage_FLASH_APP_FLASH(u32 APP_Size)    //擦除用户代码 FLASH 数据 扇区
{
	u16  i =  APP_START_PAGE ;    //得到用户起始扇区
	u16  PAGE = 0 ;               //扇区数量
	PAGE = APP_Size/SECTION_PAGE_SIZE ;   //得到整数
	if((APP_Size%SECTION_PAGE_SIZE) >0)   //如果有超过的  但是还没有到达一个扇区的
	{
	  PAGE ++ ;   //扇区数加一
	}
	
	FLASH_Unlock();						//解锁
	for( ; i<( APP_START_PAGE+PAGE ) ; i ++)
	{
    FLASH_ErasePage(Section_Start_Adress(i));//擦除这个扇区
	}
	FLASH_Lock();//上锁
}

void Read_FLASH_APP_CRC_DATA(IAP_CRC_DATA_TYP * IAP_CRC_DAT )   //读取保存在FLASH 中的校验值
{
  STMFLASH_Read(APP_DATA_START_Adress,(u16 * )IAP_CRC_DAT,sizeof(IAP_CRC_DATA_TYP)/2);
}



//得到用户代码 校验值
u8 Read_USER_FLASH_DATA_CRC(IAP_CRC_DATA_TYP * IAP_CRC_DAT )    //返回0 成功    1失败
{
	u16  i = 0 ;                            //计数
	u16  CRC_Number      = 0 ;              //完整扇区校验数    把升级文件 以2048字节为一段    一段段校验
	u8   *iap_buffer     = 0 ;              //数据缓存 
	Read_FLASH_APP_CRC_DATA(IAP_CRC_DAT);   //得到保存在FLASH 中校验数据
	if(IAP_CRC_DAT->IAP_CRC_DATA_HEAD != IAP_CRC_HEAD )   //如果头不对
	{
	  return 1 ;  //错误
	}
  iap_buffer = (u8 * ) mymalloc( CRC_SECTION_SIZE ) ;  //申请内存      申请一个CRC段大小
//获取用户代码校验值-------------------------------------------------
	CRC_Number = IAP_CRC_DAT->File_SIZE/CRC_SECTION_SIZE ;              //得到分段数
  for( i = 0 ; i <CRC_Number ; i ++)                                  //得到分段CRC值
	{
	 STMFLASH_Read(IAP_CRC_DAT->APP_STAR_ADDR+i*CRC_SECTION_SIZE ,(u16 *)iap_buffer,CRC_SECTION_SIZE/2) ; //读取2048个字节
	 IAP_CRC_DAT->DATA_CRC[i]	 = ST32_CRC_DATA_Str((u32 *)iap_buffer,CRC_SECTION_SIZE/4);             //计算CRC值                                                        //地址偏移
	}
	CRC_Number = IAP_CRC_DAT->File_SIZE%CRC_SECTION_SIZE ;                                        //得到剩余数
  STMFLASH_Read(IAP_CRC_DAT->APP_STAR_ADDR+i*CRC_SECTION_SIZE,(u16 *)iap_buffer,CRC_Number/2) ; //读取剩余字节		        
	IAP_CRC_DAT->DATA_CRC[i] = ST32_CRC_DATA_Str((u32 *)iap_buffer,CRC_Number/4);                 //计算CRC值
	myfree(iap_buffer);                                                                           //释放内存
	return 0 ;                                                                                    //返回成功
}

u8  Bijiao_CRC_DATA(IAP_CRC_DATA_TYP * IAP_CRC_DAT0,IAP_CRC_DATA_TYP * IAP_CRC_DAT1)  //比较   相同为 0   不同为 1
{
	u16  i = 0 ; 
	u8 * data0 = (u8*)IAP_CRC_DAT0 ;
	u8 * data1 = (u8*)IAP_CRC_DAT1 ;
	for(i = 0 ; i < sizeof(IAP_CRC_DATA_TYP); i++ )
  {
	  if( (*data0) !=(*data1) )
		{
		 return 1 ;
		}
		data0++;
		data1++;
	}
  return 0 ;
}


void SYS_SCAN_Destruct_Exe(void)  //检测设备是否需要自毁
{
	u16    Destruct_FLAT    =   SYS_Destruct_DR ;
  if( Destruct_FLAT == IAP_CRC_HEAD )  //如果是自毁标记
	{
	  ErasePage_FLASH_APP_CRC_DATA() ;        //擦除IAP CRC数据
    ErasePage_FLASH_APP_FLASH(30*1024)  ;   //擦除FLASH 数据 扇区    擦除30K 代码
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
    PWR_BackupAccessCmd(ENABLE);	                                            //使能后备寄存器访问 
		SYS_Destruct_DR = 0XFFFF;                                                 //清除自毁标记
	}
}

void SET_SYS_Destruct(void)    //设置设备自毁
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
  PWR_BackupAccessCmd(ENABLE);	                                            //使能后备寄存器访问 
  SYS_Destruct_DR = IAP_CRC_HEAD ;                                          //写入自毁标记
	Sys_Soft_Reset();                                                         //设备复位
}


void Scan_SYS_Bootloader_Lock(void)  //检测用户代码是否为Bootloader烧录  如果不是设备自毁
{
  IAP_CRC_DATA_TYP * FLASH_CRC_DATA ;   //FLASH 内保存的CRC数据
	IAP_CRC_DATA_TYP * USER_CRC_DATA  ;   //用户代码校验数据
	FLASH_CRC_DATA  =(IAP_CRC_DATA_TYP*)mymalloc(sizeof(IAP_CRC_DATA_TYP));		//申请内存	
	USER_CRC_DATA  =(IAP_CRC_DATA_TYP*)mymalloc(sizeof(IAP_CRC_DATA_TYP));		//申请内存	
  if( Read_USER_FLASH_DATA_CRC(USER_CRC_DATA) == 0 )   //读取用户数据 CRC
	{
	  Read_FLASH_APP_CRC_DATA(FLASH_CRC_DATA);  //读取FLASH内 保存的校验数据
		if( Bijiao_CRC_DATA(USER_CRC_DATA,FLASH_CRC_DATA) > 0)   //如果校验错误
		{
		 	myfree(USER_CRC_DATA);    //释放内存
	 	  myfree(FLASH_CRC_DATA);  
		  SET_SYS_Destruct();       //设置设备自毁		
		}
	}
	else     //读取失败
	{
	 	myfree(USER_CRC_DATA);    //释放内存
	 	myfree(FLASH_CRC_DATA);  
		SET_SYS_Destruct();       //设置设备自毁	
	}
  myfree(USER_CRC_DATA);    //释放内存
	myfree(FLASH_CRC_DATA);  
}



