#include "OLED_BMP.h"
#include "OLED_GUI.h"



/***********************************************************************
typedef struct
{
   u8          start;      //ͼƬָ��ṹ����ʼ���  0X5A     1���ֽ�
	 u16         MOS_flaot;  //��С��У����   ��ȷֵΪ0X123sizeof(const char *)*2����0x3sizeof(const char *)*212     0x3sizeof(const char *)*212��Ҫת��  ����ֵ����   2���ֽ�
   u16         bmp_num;    //ͼƬ����                        2���ֽ�
   BMP_u32         bmp_aderr;  //ͼƬ������ʼ��ַ                 sizeof(const char *)*2���ֽ�
   u8          bmp_ID[8];  //ͼƬ���ֻ��д  GBK              8���ֽ�
   u8          end;        //�ֿ�ָ��ṹ��  ������� 0XA5     1���ֽ�
	 BMP_code_u8    *bmp_ID_code;//ͼƬ��������ָ��                  2���ֽ�
	 BMP_code_u8    *bmp_code;   //ͼƬ��������ָ��                  2���ֽ�
}bmp_code_typ;    //ͼƬָ��ṹ��    ��22���ֽ�   ������ֽ���Ϊ 22-2-2 = 18
*************************************************************************/

bmp_code_typ  BMP_Code={                  //FLASH��ͼƬ����ṹָ��
                        0X5A,             //����ͷ
                        0X1234,           //��С��У����
                        0,                //ͼƬ����
                        OLED_CODE_Adress, //ͼƬ������ʼ��ַ
	                      "0",              //ͼƬID
	                      0xA5,             //�������
	                      0,  //ͼƬID����ָ��
	                      0   //ͼƬ����ָ��
                        }; 

//д��ͼƬ���ݵ�FLASH   ����0 д��ɹ�   ����ʧ��
//�����д��ַû�в���  ��ֱ���˳�
u8 FLASH_Write_bmp(bmp_code_typ *BMP_code)
{
  SPI_Flash_Write((u8*)&BMP_Code,OLED_BMP_START+BMP_code->bmp_num*(FLASH_FLAT_SIZE),FLASH_FLAT_SIZE); //д��ָ��
 
  SPI_Flash_Write((u8*)BMP_code->bmp_code,BMP_code->bmp_aderr,1024);
	
	return 0;    //���سɹ�
}


u8  FLASH_BMP_play(u8 num)  //ָ������  ͼƬ��ʾ    ���� 0�ɹ�  ����ʧ��
{
  u8  code_buf[1024]={0};   //�������ݻ���

  SPI_Flash_Read((u8*)&BMP_Code,OLED_BMP_START+num*(FLASH_FLAT_SIZE),FLASH_FLAT_SIZE);  //��ȡָ��ͼƬ����ָ��ṹ��
  
  if((BMP_Code.start==0x5A)&&(BMP_Code.end==0XA5))   //�Ƿ����
	{ 
	 if(BMP_Code.MOS_flaot==0x1234)   //������д�С��ת��    
	 {
		SPI_Flash_Read(code_buf,BMP_Code.bmp_aderr,1024);  //��ȡFLASH  ������
		 
		 Movie_Show_BMP(code_buf);   //��ʾͼƬ                               	
    return 0;                                          //���سɹ�
   } 
	 else if(BMP_Code.MOS_flaot==0x3412)     //����д�С��ת��    ͼƬ���� �� ͼƬ�������ݵ�ַ  ��ת�� 
	 {
		 BMP_Code.bmp_aderr = ((BMP_Code.bmp_aderr & 0xff000000) >> 24)\
                        | ((BMP_Code.bmp_aderr & 0x00ff0000) >>  8)\
                        | ((BMP_Code.bmp_aderr & 0x0000ff00) <<  8)\
                        | ((BMP_Code.bmp_aderr & 0x000000ff) << 24);     //������ʼ��ַ���д�С��ת��
		 
		SPI_Flash_Read(code_buf,BMP_Code.bmp_aderr,1024);  //��ȡFLASH  ������
    Movie_Show_BMP(code_buf);                          //��ʾͼƬ   
    return 0;                                          //���سɹ�
   }                                           
  }
	return 1;                                            //����ʧ��
}

u16 FLASH_BMP_Scan(void)       //ɨ��FLASH��ͼƬ���� ����  0��ʾ��ͼƬ  ����ΪͼƬ����
{
	u16  Scan_num=0;   //ɨ�����
	Scan_num = FLASH_FLAT_SIZE;
	Scan_num = FLASH_Pointer_MAX;
	Scan_num = FLASH_BMP_NUBL;
	Scan_num = 0;
#if UART_TEST__EN
  UartInit();                //��ʼ������
#endif

	while((Scan_num<=FLASH_Pointer_MAX)&&(Scan_num<=FLASH_BMP_NUBL))
	{
	  SPI_Flash_Read((u8*)&BMP_Code,OLED_BMP_START+Scan_num*(FLASH_FLAT_SIZE),FLASH_FLAT_SIZE);  //������д��ַ����
    if((BMP_Code.start==0x5A)&&(BMP_Code.end==0XA5))   //�Ƿ����
		{
			Scan_num++;                     //��һ����ַ
			
#if UART_TEST__EN
			Uart_printf("\r\n ͼƬ����Ϊ�� %d ",BMP_Code.bmp_num);	
			Uart_printf("\r\n ͼƬID Ϊ��  %s ",BMP_Code.bmp_ID);	
#endif
    }
		else
		{
#if UART_TEST__EN
			Uart_printf("\r\n ������ɣ�");
#endif
      break;
    }
  }
#if UART_TEST__EN
 Uart_printf("\r\n ��������%d��ͼƬ \r\n",Scan_num);
#endif
	
 return Scan_num;
}

u8  FLASH_BMP_ID_play(u8 *BMP_ID) //��ʾFLASH��ָ��IDͼƬ  ����0�ɹ�   1ʧ��
{
  u16 BMP_MAX = FLASH_BMP_Scan();   //��ǰFLASH ͼƬ����

#if UART_TEST__EN
	Uart_printf(" �������鳤��Ϊ %d \r\n",strlen((const char *)BMP_ID));
#endif

	while(BMP_MAX--)     //�Ӻ���ǰ��ѯ
  {
#if UART_TEST__EN
	Uart_printf(" ��ǰɨ������Ϊ %d \r\n",BMP_MAX);
#endif
		
    SPI_Flash_Read((u8*)&BMP_Code,OLED_BMP_START+BMP_MAX*(FLASH_FLAT_SIZE),FLASH_FLAT_SIZE);  //��ȡָ������ͼƬָ������
    if(MY_STRCMP((const char*)BMP_ID,(const char*)BMP_Code.bmp_ID)==0)
	  {
		  if(BMP_Code.MOS_flaot==0x1234)   //������д�С��ת��    
	    {
        #if UART_TEST__EN
	          Uart_printf(" ��ӦͼƬ������Ϊ %d \r\n",BMP_Code.bmp_num);
        #endif
				
        FLASH_BMP_play(BMP_Code.bmp_num);   //��ʾָ������ͼƬ
				return 0;
			}
			else if(BMP_Code.MOS_flaot==0x3412)   //��Ҫ��С��ת��
			{
        BMP_Code.bmp_num=((BMP_Code.bmp_num&0xff00)>>8)|((BMP_Code.bmp_num&0x00ff)<<8);  //ͼƬ������С��ת��
				
	      #if UART_TEST__EN
	          Uart_printf(" ��ӦͼƬ������Ϊ %d \r\n",BMP_Code.bmp_num);
        #endif
				
        FLASH_BMP_play(BMP_Code.bmp_num);       //��ʾָ������ͼƬ
        return 0;
      }	
    }
  }
#if UART_TEST__EN
	Uart_printf(" û���ҵ���ӦͼƬ�� \r\n");
#endif	
	return 1;     //û���ҵ�
}

//�Ƚ����ַ��� str0����������������������ͬ�Ƚ����
//���� 0 ��ͬ  ������ͬ
u8 MY_STRCMP(const char * str0,const char *str1)
{
  while(*str0)
	{
	  if(*str0 != *str1)
	  {
		 return 1;
	  }
	  str0++;
		str1++;
	}
  return 0;
}

