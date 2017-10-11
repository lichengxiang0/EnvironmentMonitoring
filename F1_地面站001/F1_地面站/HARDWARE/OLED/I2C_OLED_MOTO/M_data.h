#ifndef __M_DATA_H_
#define __M_DATA_H_	 
#include "sys.h"

#define    GPS_PLAY_LEN        4    //GPS定位动画节拍


#define    LOGO_YW          0                         //LOGO  英文  
#define    NAOZ_BJ          1                         //闹钟背景
#define    SD_ERR           2                         //SD卡拔出警告界面
#define    SD_INIT          3                         //SD初始化汉字界面
#define    UTC_INIT         4                         //正在获取UTC时间数据
#define    UTC_OK           5                         //UTC时间校准数据成功
#define    USART_INIT       6                         //串口配置界面
#define    SYS_INITT        7                         //系统初始化   装逼
#define    RC_S             8                         //RTC配置初始化
#define    RC_E             9                         //RTC配置完成
#define    File_S          10                         //文件创建开始
#define    File_E          11                         //文件创建完成
#define    PLAY_INTT       12                         //显示初始化界面
#define    FILE_ERR        13                         //创建文件失败
#define    GPS_ERR         14                         //GPS板卡故障
#define    GPS_OK          15                         //GPS定位成功
#define    GPS_CONFIG      16                         //GPS配置桌面
#define    GPS_broadcast   17                         //GPS配置桌面

extern const u8 movie_data[] ;


extern const u8 GPS_Cartoon[GPS_PLAY_LEN][1024] ;  //GPS动画 字模
	
#endif


