#ifndef _OLED_BMP_H_
#define _OLED_BMP_H_
 /**********************************************************************************
FlashͼƬ����洢(���200k)
  (200*1024/4=50  511-50+1 = 462)  ��462�������� 2*1024/4 = 512����   �ֽڵ�ַΪ 1892352  = 462*4*1024
   ������һ���������ͼƬ����ָ�룬
   ��ʼ�������ֽڴ��ͼƬ������ڱ�Ǽ�У���С�ˣ���0X12345678��
   �����ֽڴ��ͼƬ���鿪ʼ��ַ��Ȼ��ʼ�������ָ��ṹ�塣
	 
	 �ṹ��ṹ 
   ��ʼ��ǣ�OX5A��һ���ֽڣ�
   ��С��У�飬У��������0X1234�� �����ֽ�
   �ڼ���ͼƬ�����ֽڣ�
   ͼƬ�������ʼ��ַ �ĸ��ֽ�
   ͼƬ���ְ˸��ֽڣ����4������GBK�룬��Ч����0X00���
   ������ǣ�OXA5��һ���ֽ�

typedef struct
{
   u8          start;      //ͼƬָ��ṹ����ʼ���  0X5A     1���ֽ�
	 u16         MOS_flaot;  //��С��У����   ��ȷֵΪ0X1234����0x3412     0x3412��Ҫת��  ����ֵ����   2���ֽ�
   u16         bmp_num;    //ͼƬ����                        2���ֽ�
   u32         bmp_aderr;  //ͼƬ������ʼ��ַ                 4���ֽ�
   u8          bmp_ID[8];  //ͼƬ���ֻ��д  GBK              8���ֽ�
   u8          end;        //�ֿ�ָ��ṹ��  ������� 0XA5     1���ֽ�
	 BMP_code_u8    *bmp_ID_code;//ͼƬ��������ָ��                  2���ֽ�
	 BMP_code_u8    *bmp_code;   //ͼƬ��������ָ��                  2���ֽ�
}bmp_code_typ;    //ͼƬָ��ṹ��    ��22���ֽ�   ������ֽ���Ϊ 22-2-2 = 18

   �ṹ��Ϊ22�ֽ�        ������ BMP_code_u8 ����Ϊ 2�ֽ�


#define FLASH_Volume        ((u32)2)                                          //FLASH����  ��λ: M
#define FLASH_Sector_SIZE   ((u32)4096)                                       //FLASH������С   ��λ�� �ֽ�
#define BMP_SIZE            ((u32)200*1024)                                   //FLASH����ͼƬ�����ֽڴ�С
#define BMP_ALL_Sector      (BMP_SIZE/FLASH_Sector_SIZE)                          //FLASH����ͼƬ����������

#define FLASH_ALL_Adress    ((u32)FLASH_Volume*1024*1024)                     //FLASH���ֽڵ�ַ
#define OLED_BMP_START      (FLASH_ALL_Adress-BMP_SIZE)                           //FLASHͼƬ����ָ�뿪ʼ��ַ   
#define FLASH_START_Sector  (OLED_BMP_START/FLASH_Sector_SIZE)                    //FLASHͼƬ����ָ�뿪ʼ��ַ��Ӧ����

#define FLASH_Pointer_MAX   (FLASH_Sector_SIZE/(sizeof(bmp_code_typ)-sizeof(const char *)*2))          //FLASHͼƬ����ָ���������           

#define OLED_CODE_Adress    ((u32)(FLASH_START_Sector+1)*FLASH_Sector_SIZE)   //FLASHͼƬ��������д���׵�ַ
#define OLED_CODE_Sector    (FLASH_START_Sector+1)                                //FLASHͼƬ��������д���׵�ַ��Ӧ����


  ��X��ͼƬ������ʼ��ַΪ = ͼƬ������ʼ��ַ+X*1024

***********************************************************************************/

#include "OLED_BMP_CONFIG.h"


#define   BMP_ID_SIZE          20        //ͼƬID ��󳤶�

__packed typedef struct                //�����ֽڶ���
{
   u8          start;                   //ͼƬָ��ṹ����ʼ���  0X5A     1���ֽ�
	 u16         MOS_flaot;               //��С��У����   ��ȷֵΪ0X1234����0x3412     0x3412��Ҫת��  ����ֵ����   2���ֽ�
   u16         bmp_num;                 //ͼƬ����                        2���ֽ�
   u32         bmp_aderr;               //ͼƬ������ʼ��ַ                 4���ֽ�
   u8          bmp_ID[BMP_ID_SIZE+1];     //ͼƬ���ֻ��д  GBK              8���ֽ�    �����һ Ϊһ������
   u8          end;                     //�ֿ�ָ��ṹ��  ������� 0XA5     1���ֽ�
	 const u8    *bmp_ID_code;            //ͼƬ��������ָ��                  2���ֽ�
	 const u8    *bmp_code;               //ͼƬ��������ָ��                  2���ֽ�
}bmp_code_typ;    //ͼƬָ��ṹ��    ��22���ֽ�   ������ֽ���Ϊ 22-2-2 = 18


extern bmp_code_typ BMP_Code;  //FLASH��ͼƬ����ṹָ�� 


   
	




u8  FLASH_BMP_play(u8 num);         //ָ������ͼƬ��ʾ    ���� 0�ɹ�  ����ʧ��

u16 FLASH_BMP_Scan(void);           //ɨ��FLASH��ͼƬ���� ����  0��ʾ��ͼƬ  ����ΪͼƬ����

u8  FLASH_BMP_ID_play(u8 *BMP_ID);  //��ʾFLASH��ָ��IDͼƬ

  
//�Ƚ����ַ��� str0����������������������ͬ�Ƚ����
//���� 0 ��ͬ  ������ͬ
u8 MY_STRCMP(const char * str0,const char *str1);

#endif


