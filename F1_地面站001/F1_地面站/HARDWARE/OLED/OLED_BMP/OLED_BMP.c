#include "OLED_BMP.h"
#include "OLED_GUI.h"



/***********************************************************************
typedef struct
{
   u8          start;      //图片指针结构体起始标记  0X5A     1个字节
	 u16         MOS_flaot;  //大小端校验标记   正确值为0X123sizeof(const char *)*2或者0x3sizeof(const char *)*212     0x3sizeof(const char *)*212需要转换  其它值错误   2个字节
   u16         bmp_num;    //图片索引                        2个字节
   BMP_u32         bmp_aderr;  //图片数组起始地址                 sizeof(const char *)*2个字节
   u8          bmp_ID[8];  //图片名字或简写  GBK              8个字节
   u8          end;        //字库指针结构体  结束标记 0XA5     1个字节
	 BMP_code_u8    *bmp_ID_code;//图片名字数据指针                  2个字节
	 BMP_code_u8    *bmp_code;   //图片数组数据指针                  2个字节
}bmp_code_typ;    //图片指针结构体    共22个字节   保存的字节数为 22-2-2 = 18
*************************************************************************/

bmp_code_typ  BMP_Code={                  //FLASH中图片数组结构指针
                        0X5A,             //数据头
                        0X1234,           //大小端校验标记
                        0,                //图片索引
                        OLED_CODE_Adress, //图片数据起始地址
	                      "0",              //图片ID
	                      0xA5,             //结束标记
	                      0,  //图片ID数据指针
	                      0   //图片数据指针
                        }; 

//写入图片数据到FLASH   返回0 写入成功   其它失败
//如果待写地址没有擦除  则直接退出
u8 FLASH_Write_bmp(bmp_code_typ *BMP_code)
{
  SPI_Flash_Write((u8*)&BMP_Code,OLED_BMP_START+BMP_code->bmp_num*(FLASH_FLAT_SIZE),FLASH_FLAT_SIZE); //写入指针
 
  SPI_Flash_Write((u8*)BMP_code->bmp_code,BMP_code->bmp_aderr,1024);
	
	return 0;    //返回成功
}


u8  FLASH_BMP_play(u8 num)  //指定索引  图片显示    返回 0成功  其它失败
{
  u8  code_buf[1024]={0};   //数组数据缓存

  SPI_Flash_Read((u8*)&BMP_Code,OLED_BMP_START+num*(FLASH_FLAT_SIZE),FLASH_FLAT_SIZE);  //读取指定图片数据指针结构体
  
  if((BMP_Code.start==0x5A)&&(BMP_Code.end==0XA5))   //是否存在
	{ 
	 if(BMP_Code.MOS_flaot==0x1234)   //无需进行大小端转换    
	 {
		SPI_Flash_Read(code_buf,BMP_Code.bmp_aderr,1024);  //读取FLASH  内数据
		 
		 Movie_Show_BMP(code_buf);   //显示图片                               	
    return 0;                                          //返回成功
   } 
	 else if(BMP_Code.MOS_flaot==0x3412)     //需进行大小端转换    图片索引 及 图片数组数据地址  需转换 
	 {
		 BMP_Code.bmp_aderr = ((BMP_Code.bmp_aderr & 0xff000000) >> 24)\
                        | ((BMP_Code.bmp_aderr & 0x00ff0000) >>  8)\
                        | ((BMP_Code.bmp_aderr & 0x0000ff00) <<  8)\
                        | ((BMP_Code.bmp_aderr & 0x000000ff) << 24);     //数据起始地址进行大小端转换
		 
		SPI_Flash_Read(code_buf,BMP_Code.bmp_aderr,1024);  //读取FLASH  内数据
    Movie_Show_BMP(code_buf);                          //显示图片   
    return 0;                                          //返回成功
   }                                           
  }
	return 1;                                            //返回失败
}

u16 FLASH_BMP_Scan(void)       //扫描FLASH中图片张数 返回  0表示无图片  其它为图片个数
{
	u16  Scan_num=0;   //扫描计数
	Scan_num = FLASH_FLAT_SIZE;
	Scan_num = FLASH_Pointer_MAX;
	Scan_num = FLASH_BMP_NUBL;
	Scan_num = 0;
#if UART_TEST__EN
  UartInit();                //初始化串口
#endif

	while((Scan_num<=FLASH_Pointer_MAX)&&(Scan_num<=FLASH_BMP_NUBL))
	{
	  SPI_Flash_Read((u8*)&BMP_Code,OLED_BMP_START+Scan_num*(FLASH_FLAT_SIZE),FLASH_FLAT_SIZE);  //读出待写地址数据
    if((BMP_Code.start==0x5A)&&(BMP_Code.end==0XA5))   //是否存在
		{
			Scan_num++;                     //下一个地址
			
#if UART_TEST__EN
			Uart_printf("\r\n 图片索引为： %d ",BMP_Code.bmp_num);	
			Uart_printf("\r\n 图片ID 为：  %s ",BMP_Code.bmp_ID);	
#endif
    }
		else
		{
#if UART_TEST__EN
			Uart_printf("\r\n 搜索完成！");
#endif
      break;
    }
  }
#if UART_TEST__EN
 Uart_printf("\r\n 共搜索到%d张图片 \r\n",Scan_num);
#endif
	
 return Scan_num;
}

u8  FLASH_BMP_ID_play(u8 *BMP_ID) //显示FLASH中指定ID图片  返回0成功   1失败
{
  u16 BMP_MAX = FLASH_BMP_Scan();   //当前FLASH 图片总数

#if UART_TEST__EN
	Uart_printf(" 输入数组长度为 %d \r\n",strlen((const char *)BMP_ID));
#endif

	while(BMP_MAX--)     //从后向前轮询
  {
#if UART_TEST__EN
	Uart_printf(" 当前扫描索引为 %d \r\n",BMP_MAX);
#endif
		
    SPI_Flash_Read((u8*)&BMP_Code,OLED_BMP_START+BMP_MAX*(FLASH_FLAT_SIZE),FLASH_FLAT_SIZE);  //读取指定索引图片指针数据
    if(MY_STRCMP((const char*)BMP_ID,(const char*)BMP_Code.bmp_ID)==0)
	  {
		  if(BMP_Code.MOS_flaot==0x1234)   //无需进行大小端转换    
	    {
        #if UART_TEST__EN
	          Uart_printf(" 对应图片的索引为 %d \r\n",BMP_Code.bmp_num);
        #endif
				
        FLASH_BMP_play(BMP_Code.bmp_num);   //显示指定索引图片
				return 0;
			}
			else if(BMP_Code.MOS_flaot==0x3412)   //需要大小端转换
			{
        BMP_Code.bmp_num=((BMP_Code.bmp_num&0xff00)>>8)|((BMP_Code.bmp_num&0x00ff)<<8);  //图片索引大小端转换
				
	      #if UART_TEST__EN
	          Uart_printf(" 对应图片的索引为 %d \r\n",BMP_Code.bmp_num);
        #endif
				
        FLASH_BMP_play(BMP_Code.bmp_num);       //显示指定索引图片
        return 0;
      }	
    }
  }
#if UART_TEST__EN
	Uart_printf(" 没有找到对应图片！ \r\n");
#endif	
	return 1;     //没有找到
}

//比较两字符串 str0遇到结束符或者遇到不相同比较完成
//返回 0 相同  其它不同
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

