#ifndef _FILE_INI_H_
#define _FILE_INI_H_
#include "sys.h"




#define   Satellite_Broadcast_FLAT               ((u16)0x1234)    //���ǹ㲥���
#define   Satellite_Broadcast_Reg                  BKP->DR41          //���ǹ㲥�Ĵ���



void NO_CONFIG_FILE_EXE(void)  ;



void Wait_Satellite_Broadcast(void) ; //�ȴ����ǹ㲥





#endif

