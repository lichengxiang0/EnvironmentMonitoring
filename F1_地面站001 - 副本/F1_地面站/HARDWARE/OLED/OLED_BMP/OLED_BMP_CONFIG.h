#ifndef _OLED_BMP_CONFIG_H_
#define _OLED_BMP_CONFIG_H_

#include "w25qxx.h"
#include "Oled.h"
#include <string.h>     //�ַ�������

// ���ڵ��� ʹ��------------------------------------------------------
#define  UART_TEST__EN          0      //Ϊ1ʱ�������ڵ���    �����������ͷ�ļ�

// FLASHͼƬ�����޸� ʹ��---------------------------------------------
#define  FLASH_DATA_SET_EN      1       //Ϊ1ʱ �����޸�FLASHͼƬ����

//����д��FLASH ͼƬ���ݸ���   �����  FLASH_DATA_SET_EN ����0 ��Ч
#define  BMP_FONT_NUM           21       //��ǰ����ͼƬ����  ����OLED_BMP_FONT.C ��ͼƬ���ݸ�����Ӧ  ����ͼƬ������������д��ͼƬΪ����



// �����޸�FLASH ͼƬ���� ʹ��---------------------------
#if FLASH_DATA_SET_EN

#include "OLED_BMP.h"
//����д��FLASH ͼƬ���ݸ���   �����  FLASH_DATA_SET_EN ����0 ��Ч
#define  BMP_FONT_MAX           BMP_FONT_NUM       //��ǰ����ͼƬ����

#endif

//--------------------------------��ֲ�޸�-----------------------------
//���ݽṹ����---------------------------------------------------------
//typedef  u8                   BMP_u8;
//#define  BMP_CODE             const            
//typedef  BMP_u8  BMP_CODE     BMP_code_u8;
//typedef  u16                  BMP_u16;
//typedef  u32                  u32;


#define FLASH_Volume        ((u32)2)                                                            //FLASH����  ��λ: M
#define FLASH_Sector_SIZE   ((u32)4096)                                                         //FLASH������С   ��λ�� �ֽ�
#define BMP_SIZE            ((u32)200*1024)                                                     //FLASH����ͼƬ�����ֽڴ�С
#define BMP_ALL_Sector      (BMP_SIZE/FLASH_Sector_SIZE)                                        //FLASH����ͼƬ����������

#define FLASH_ALL_Adress    ((u32)FLASH_Volume*1024*1024)                                       //FLASH���ֽڵ�ַ
#define OLED_BMP_START      (FLASH_ALL_Adress-BMP_SIZE)                                         //FLASHͼƬ����ָ�뿪ʼ��ַ   
#define FLASH_START_Sector  (OLED_BMP_START/FLASH_Sector_SIZE)                                  //FLASHͼƬ����ָ�뿪ʼ��ַ��Ӧ����

#define FLASH_FLAT_SIZE     (sizeof(bmp_code_typ)-(sizeof(const char *)*2))                     //FLASH����ָ��д���С
	
#define FLASH_BMP_NUBL      ((u32)(BMP_SIZE - FLASH_Sector_SIZE )/1024)                         //FLASH����ܴ��ͼƬ����
#define FLASH_Pointer_MAX   (FLASH_Sector_SIZE/FLASH_FLAT_SIZE)                                 //FLASHͼƬ����ָ���������  
	
#define OLED_CODE_Adress    ((u32)(FLASH_START_Sector+1)*FLASH_Sector_SIZE)                     //FLASHͼƬ��������д���׵�ַ
#define OLED_CODE_Sector    (FLASH_START_Sector+1)                                              //FLASHͼƬ��������д���׵�ַ��Ӧ����

//-------------------------------------------------------------------------------------------------------------

//���ڼ��ʹ��------------------------------------------
#if UART_TEST__EN
#include "Usart_Driver.h"     //��������ͷ�ļ�

//��ӳ�����º���----------------------------------
#define   UartInit()     Uart1_Init(115200)    
#define   Uart_printf    printf
   
#endif




#endif



