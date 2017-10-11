#ifndef __M_DATA_H_
#define __M_DATA_H_	 
#include "sys.h"

#define    GPS_PLAY_LEN        4    //GPS��λ��������


#define    LOGO_YW          0                         //LOGO  Ӣ��  
#define    NAOZ_BJ          1                         //���ӱ���
#define    SD_ERR           2                         //SD���γ��������
#define    SD_INIT          3                         //SD��ʼ�����ֽ���
#define    UTC_INIT         4                         //���ڻ�ȡUTCʱ������
#define    UTC_OK           5                         //UTCʱ��У׼���ݳɹ�
#define    USART_INIT       6                         //�������ý���
#define    SYS_INITT        7                         //ϵͳ��ʼ��   װ��
#define    RC_S             8                         //RTC���ó�ʼ��
#define    RC_E             9                         //RTC�������
#define    File_S          10                         //�ļ�������ʼ
#define    File_E          11                         //�ļ��������
#define    PLAY_INTT       12                         //��ʾ��ʼ������
#define    FILE_ERR        13                         //�����ļ�ʧ��
#define    GPS_ERR         14                         //GPS�忨����
#define    GPS_OK          15                         //GPS��λ�ɹ�
#define    GPS_CONFIG      16                         //GPS��������
#define    GPS_broadcast   17                         //GPS��������

extern const u8 movie_data[] ;


extern const u8 GPS_Cartoon[GPS_PLAY_LEN][1024] ;  //GPS���� ��ģ
	
#endif


