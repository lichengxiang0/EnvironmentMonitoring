#ifndef _OLED_BMP_Font_h_
#define _OLED_BMP_Font_h_

#include "OLED_BMP_CONFIG.h"
#include "OLED_BMP.h"
/*****************************ȡģ˵��*****************************************************/   
 //     ����         ����ʽ            �������																
 //    C51��ʽ    ע��ǰ׺�� /*"   ����ǰ׺��0x    ���ݺ�׺��,   �к�׺:, ��β׺: */  
 //ע�⣡ ������  ����ͼƬȡģ��������ȡģ��ֻ���� PCoLCD2002���  
/******************************************************************************************/


__packed typedef struct
{
	const u8 BMP_ID[BMP_ID_SIZE+1];       //ͼƬ����
  const u8 BMP_font[1024];  //ͼƬ������ģ
}BMP_FONT_typ;   //�����½ṹ����

typedef const BMP_FONT_typ BMP_FONT_CODE;  //�����½ṹ����������洢�ռ�

extern BMP_FONT_CODE BMP_FONT[];   //����ṹ������  ����������洢�ռ�





/*************************************************************
                      Ӧ�ò�
**************************************************************/
#if FLASH_DATA_SET_EN   //�ļ�ʹ��

//��OLED_BMP_FONT��ͼƬ����ȫ��д��FLASH��  
//����FLASH�ڲ�ͼƬ��ַ����  ����0 �ɹ�  ����ʧ��
// numΪ��ȷд�����Ƭ��
u8  FLASH_Write_font(u16 *num) ;


//д��ͼƬ���ݵ�FLASH   ����0 д��ɹ�   ����ʧ��
//�����д��ַû�в���  ��д��ʧ��  �˳�
u8  FLASH_Write_bmp(bmp_code_typ *BMP_code);

void OLED_BMP_Erase(void);            //����FLASHͼƬ������������



#endif

#endif
