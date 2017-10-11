#ifndef __GPS_STA_H_
#define __GPS_STA_H_	 
#include "sys.h"

#define GPS_PV_RCC         RCC_APB2Periph_GPIOB

#define GPS_PV_           GPIOB                   //PV
#define GPS_PV_PIN        GPIO_Pin_7


#define GPS_PV          GPIO_ReadInputDataBit(GPS_PV_ , GPS_PV_PIN)


#define GPS_COMM_CONFIG_Nuber        6                   //GPS配置指令集长度

extern const u8 *GPS_REST_CMM           ;               //GPS恢复成出厂设置命令
extern const u8 *GPS_TIME_CMM           ;               //GPS配置输出GPS时间数据
extern const u8 *GPS_UNLOGALL_CMM       ;               //GPS配置关闭所有串口打印输出
extern const u8 *GPS_REST_CMM           ;               //GPS复位命令
extern const u8 *GPS_SAVE_CMM           ;               //保存GPS配置
extern const u8 *GPS_CMM_5HZ[GPS_COMM_CONFIG_Nuber] ;   // 5 HZ  配置命令集 



void GPS_PV_Init(void);

u8 GPS_CONFIG_RESET(void) ;    //GPS复位   返回 0成功   1失败
u8 GPS_CONFIG_5HZ(void) ;      //配置成 5HZ   返回0成功   1失败
u8 GPS_CONFIG_TIMEA(void) ;    //串口输出时间数据 0成功    1失败
u8 GPS_UNLOGALL_CONFIG(void) ; //GPS关闭所有输出   0成功    1失败
u8 GPS_SAVE_CONFIG(void)  ;    //保存配置				    
#endif
