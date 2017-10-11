#ifndef   __IAP_EXE_H__
#define   __IAP_EXE_H__

#include "SYS.H"
#include "ff.h"
#include "IAP_DATA.H"

extern IAP_CRC_DATA_TYP   *APP_File_DATA    ;      //APP文件   校验数据
extern IAP_CRC_DATA_TYP  * FLASH_DATA       ;      //FLASH 保存的校验数据 



void SYS_IAP_Init(void) ;         //IAP 硬件初始化
void RTK01_IAP_EXE(void);         //执行IAP
void APP_STAR_FUN(u32 AppAddr) ;  //运行APP	

//得到文件信息  及 校验值
u8 Read_FILE_DATA_CRC(IAP_CRC_DATA_TYP * IAP_CRC_DAT ) ;   //返回0 成功    1失败




#endif







