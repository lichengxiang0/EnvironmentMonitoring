#ifndef _FILE_INI_H_
#define _FILE_INI_H_
#include "sys.h"




#define   Satellite_Broadcast_FLAT               ((u16)0x1234)    //卫星广播标记
#define   Satellite_Broadcast_Reg                  BKP->DR41          //卫星广播寄存器



void NO_CONFIG_FILE_EXE(void)  ;



void Wait_Satellite_Broadcast(void) ; //等待卫星广播





#endif

