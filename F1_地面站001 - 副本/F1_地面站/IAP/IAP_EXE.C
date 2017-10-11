#include "IAP_EXE.H"
#include "sys.h"
#include "delay.h"
#include "sys.h"
#include "Led.h"
#include "Oled.h"
#include "Malloc.h"
#include "sdio_sdcard.h"
#include <string.h>
#include "OLED_GUI.h"
#include "STM32_CRC.h"
#include "stmflash.h"
#include "IAP_PLAY.H"
#include "exfuns.h"

///////////////////////////////公共文件区,使用malloc的时候////////////////////////////////////////////
typedef void (*AppFun)(void);                 //指向函数的指针


AppFun    AppStart ;                          //APP MAIN()
	
IAP_CRC_DATA_TYP   *APP_File_DATA   ;      //APP文件   校验数据
IAP_CRC_DATA_TYP  * FLASH_DATA      ;      //FLASH 保存的校验数据 
 


void SYS_IAP_Init(void)  //IAP 硬件初始化
{
	SystemInit(); 			 					//系统时钟初始化为72M	  SYSCLK_FREQ_72MHz
	delay_init();	    	 				  //延时函数初始化	 
	NVIC_Configuration(); 	 			//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	OLED_Init();                  //OLED初始化
	mallco_dev.init();            //内存管理初始化
	exfuns_init();                //文件操作变量内存申请
	STM32_CRC_Init();             //CRC 初始化
}



void APP_STAR_FUN(u32 AppAddr)  //运行APP
{
	if(((*(vu32*)AppAddr)&0x2ffe0000) == 0x20000000)
	{
		AppStart = (AppFun)(*(vu32*)(AppAddr+4));
		MSR_MSP(*(vu32*)AppAddr);
		AppStart();
	}
}


void SYS_FLASH_Update(void)   //烧录FLASH   成功 0    错误 1
{
	u32  addrx = 0 ,endaddr = 0 ;               //起始地址   终止地址
	u8  *iap_buffer = 0 ;                       //数据缓存  
	br = f_open(file,(const TCHAR *)Updata_Path,FA_OPEN_EXISTING|FA_READ);   //打开文件
  if(br != FR_OK )
	{
		return ;
	}
	OLED_Init();                      //OLED 初始化
	OLED_ShowString(16,48,(const u8 *)"Updating ...",16,1,2);		
	OLED_ShowString(80,32,(const u8 *)"DMZ",16,1,2);		
  ErasePage_FLASH_APP_CRC_DATA();                        //擦除IAP 数据 
	ErasePage_FLASH_APP_FLASH(APP_File_DATA->File_SIZE);   //擦除用户FLASH 数据
  addrx = APP_File_DATA->APP_STAR_ADDR;                  //起始地址
  endaddr = APP_File_DATA->APP_END_ADDR;                 //结束的地址为起始地址加文件大小
	iap_buffer = (u8 * ) mymalloc(SECTION_PAGE_SIZE ) ;    //申请内存   2048 字节
	while(1)
	{
		br = f_read(file,iap_buffer,SECTION_PAGE_SIZE,&bw);//读取2048个字节
		if((br != FR_OK )|| (bw == 0 ))//如果读取失败或者读取完成
		{
		  break;  //退出  写入完成
		}
		
		FLASH_Unlock();						//解锁
		STMFLASH_Write_NoCheck(addrx,(u16*)iap_buffer,bw/2);   //写入FLASH      不检查是否擦除  因为之前已经擦除了
		FLASH_Lock();//上锁
		
		addrx += bw ;                                          //地址偏移
		
		IAP_EXE_PALY_prog(32,32,endaddr-APP_START_ADDR,addrx-APP_START_ADDR );		//更新进度
		if(addrx>=endaddr)                                                        //写完了
		{
			break;
		}
	 }
	 myfree(iap_buffer);  //释放内存
	 f_close(file);  //关闭文件
}



void RTK01_IAP_EXE(void)  //执行IAP
{
 APP_File_DATA  = (IAP_CRC_DATA_TYP*)mymalloc(sizeof(IAP_CRC_DATA_TYP));		//申请内存
 FLASH_DATA     = (IAP_CRC_DATA_TYP*)mymalloc(sizeof(IAP_CRC_DATA_TYP));		//申请内存
 if(SD_Init() ==0) //SD卡初始化成功
 {
 	if(f_mount(fs[0],"0:",1) == FR_OK)  //挂载文件系统成功
	{  
	 br = f_open(file,(const TCHAR *)Updata_Path,FA_READ);   //已读方式打开文件
   
	 if(br != FR_OK ) //如果打开失败
	 {
	   f_mount(fs[0],"0:",0) ;  //解挂文件系统
		 return ;                 //退出
	 } 
	 if( (file->fsize <(20*1024))||(file->fsize >(SECTION_PAGE_MAX*SECTION_PAGE_SIZE)) ||(file->fsize%4 > 0)) //如果文件大小小于20K 或者超过FLASH大小 或者 大小不是4的倍数 则文件有问题
	 {
	  f_close(file);   //关闭文件
	 	f_mount(fs[0],"0:",0) ;    //解挂文件系统
		return ;                 //退出
	 }
	 f_close(file);   //关闭文件
	 
//有BIN 文件  可能需要更新-------------------------------------------
  if(Read_FILE_DATA_CRC(APP_File_DATA) == 0 )         //得到文件信息  及 校验值
	{
	  Read_FLASH_APP_CRC_DATA(FLASH_DATA);              //得到FLASH 中 的校验数据
	 //比对数据   看是否需要更新-------------------------------------
	 if( Bijiao_CRC_DATA(APP_File_DATA,FLASH_DATA) >0 ) //如果不相同  则需要更新
	 {
	   SYS_FLASH_Update() ;      //烧录FLASH
		 f_mount(fs[0],"0:",0) ;   //解挂文件系统
		 if( br == FR_OK )         //如果操作正常
		 {
	     Write_FLASH_APP_CRC_DATA(APP_File_DATA);   //文件信息 校验数据 写入FLASH
			 delay_ms(500);
		 }
	 }
	 else
	 {
	 	f_mount(fs[0],"0:",0) ;  //解挂文件系统
	  return ;                 //退出
	 }
	}
 	else
	{
	 f_mount(fs[0],"0:",0) ;    //解挂文件系统
	 return ;                   //退出	
	 }
  }
 }
}

//得到文件信息  及 校验值
u8 Read_FILE_DATA_CRC(IAP_CRC_DATA_TYP * IAP_CRC_DAT )    //返回0 成功    1失败
{
	u16  i = 0 ;                  //计数
	u16  CRC_Number  = 0 ;        //完整扇区校验数    把升级文件 以2048字节为一段    一段段校验
	u8   *iap_buffer = 0 ;        //数据缓存 
  iap_buffer = (u8 * ) mymalloc( CRC_SECTION_SIZE ) ;  //申请内存      申请一个CRC校验段大小
  br = f_open(file,(const TCHAR *)Updata_Path,FA_OPEN_EXISTING|FA_READ);   //已读方式打开文件
	if(br == FR_OK)    //如果打开成功
	{
		IAP_CRC_DAT->IAP_CRC_DATA_HEAD   = IAP_CRC_HEAD ;                                          //标记头
	  IAP_CRC_DAT->File_SIZE           =  file->fsize ;                                          //得到文件大小
	  IAP_CRC_DAT->APP_STAR_ADDR       =  APP_START_ADDR ;                                       //APP起始地址
	  IAP_CRC_DAT->APP_END_ADDR        =  IAP_CRC_DAT->APP_STAR_ADDR + IAP_CRC_DAT->File_SIZE ;  //APP结束地址
		
		//获取文件校验值-------------------------------------------------
		CRC_Number = file->fsize/CRC_SECTION_SIZE ;             //得到分段数
    for( i = 0 ; i <CRC_Number ; i ++)                       //得到分段CRC值
		{
      br = f_lseek(file,i * CRC_SECTION_SIZE );             //移动指针读写指针
	    br = f_read(file,iap_buffer,CRC_SECTION_SIZE,&bw);    //读取2048个字节
		  if((br == FR_OK)&&(bw == CRC_SECTION_SIZE ))          //如果打开成功
		  {
			  IAP_CRC_DAT->DATA_CRC[i]	 = ST32_CRC_DATA_Str((u32 *)iap_buffer,CRC_SECTION_SIZE/4);   //计算CRC值
	 	  }
		  else
		  {
			  myfree(iap_buffer);  //释放内存
	      f_close(file);       //关闭文件
	      return 1;  //返回错误
	    }		
		}
		CRC_Number = file->fsize%CRC_SECTION_SIZE ;                //得到剩余数
		if( CRC_Number > 0)
		{
      br = f_lseek(file,i * CRC_SECTION_SIZE );                //移动指针读写指针
	    br = f_read(file,iap_buffer,CRC_Number,&bw);             //读取剩余字节		
	    if((br == FR_OK)&&(bw == CRC_Number ))                   //如果打开成功
		  {
			  IAP_CRC_DAT->DATA_CRC[i] = ST32_CRC_DATA_Str((u32 *)iap_buffer,CRC_Number/4);   //计算CRC值
	 	  }
			else
			{
			  myfree(iap_buffer);  //释放内存
	      f_close(file);       //关闭文件
	      return 1;  //返回错误	
			}
	  }
	}
	myfree(iap_buffer);  //释放内存
	f_close(file);       //关闭文件
	if(br == FR_OK)
	{
	  return 0 ;   //返回成功
	}
	return 1 ;    //返回失败
}







