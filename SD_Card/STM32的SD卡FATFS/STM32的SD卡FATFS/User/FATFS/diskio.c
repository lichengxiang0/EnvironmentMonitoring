/*-----------------------------------------------------------------------*/
/* Glue functions for FatFs - MCI/NAND drivers            (C)ChaN, 2010  */
/*-----------------------------------------------------------------------*/

#include "diskio.h"


DSTATUS disk_initialize (BYTE drv)//��������ĸ�����0Ϊ��һ���������ص��ǿ���״̬ ��ʼ���ɹ����޿�����ʼ��ʧ��
{
u8 state;

    if(drv)
    {
        return STA_NOINIT;  //��֧�ִ���0�Ĳ���  0x01
    }

    state = SD_Init();////////////////////////////////////////////////////
    if(state == STA_NODISK)
    {
        return STA_NODISK; //0x02
    }
    else if(state != 0)
    {
        return STA_NOINIT;  //�������󣺳�ʼ��ʧ��
    }
    else
    {
        return 0;           //��ʼ���ɹ�
    }
}



DSTATUS disk_status (BYTE drv)//��������ĸ�����0Ϊ��һ�����������ݲ�֧�֣����ص��ǿ���״̬ ��ʼ���ɹ����޿�����ʼ��ʧ��
{
    if(drv)
    {
        return STA_NOINIT;  //��֧�ִ���0����
    }

    //���SD���Ƿ����
    if(SD_Insert_Status())
    {
        return STA_NODISK;
    }
    return 0;
}



DRESULT disk_read (BYTE drv,BYTE* buff,DWORD sector,BYTE count)//�ĸ�����һ��Ϊ0�������������ݴ洢�ĵط������ַ�������
{
	u8 res=0;
    if (drv || !count)
    {    
        return RES_PARERR;  //��֧�ֵ����̲�����count���ܵ���0�����򷵻ز�������
    }
    if(SD_Insert_Status())
    {
        return RES_NOTRDY;  //û�м�⵽SD������NOT READY����
    }

    
	
    if(count==1)            //1��sector�Ķ�����      
    {                                                
        res = SD_ReadSingleBlock(sector, buff);      
    }                                                
    else                    //���sector�Ķ�����     
    {                                                
        res = SD_ReadMultiBlock(sector, buff, count);
    }                                                
	/*
    do                           
    {                                          
        if(SD_ReadSingleBlock(sector, buff)!=0)
        {                                      
            res = 1;                           
            break;                             
        }                                      
        buff+=512;                             
    }while(--count);                                         
    */
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}


#if _READONLY == 0 //�Ƿ�Ϊֻ��ģʽ

DRESULT disk_write (BYTE drv,const BYTE* buff,DWORD sector,BYTE count)
{
u8 res;

    if (drv || !count)
    {    
        return RES_PARERR;  //��֧�ֵ����̲�����count���ܵ���0�����򷵻ز�������
    }
  	if(SD_Insert_Status())
    {
        return RES_NOTRDY;  //û�м�⵽SD������NOT READY����
    }

    // ��д����
    if(count == 1)
    {
        res = SD_WriteSingleBlock(sector, buff);
    }
    else
    {
        res = SD_WriteMultiBlock(sector, buff, count);
    }
    // ����ֵת��
    if(res == 0)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}

#endif /* _READONLY */

DRESULT disk_ioctl (BYTE drv,BYTE ctrl,void* buff)//�ĸ��̣��������0ˢ�»��棬3��ȡ���С��1��ȡ���������������С�ʹ���������ŵ�ָ��
{
    DRESULT res;
    if (drv)
    {    
        return RES_PARERR;  //��֧�ֵ����̲��������򷵻ز�������
    }
    
    //FATFSĿǰ�汾���账��CTRL_SYNC��GET_SECTOR_COUNT��GET_BLOCK_SIZ��������
    switch(ctrl)
    {
    case CTRL_SYNC:	//ˢ�´��̻���
   
		SD_CS_ENABLE();
        if(SD_WaitReady()==0)
        {
            res = RES_OK;
        }
        else
        {
            res = RES_ERROR;
        }
       
		SD_CS_DISABLE();
        break;
        
    case GET_BLOCK_SIZE://��ȡ��Ĵ�С
        *(WORD*)buff = 512;
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT: //��ȡ���̵�����
        *(DWORD*)buff = SD_GetCapacity();
        res = RES_OK;
        break;
    default:
        res = RES_PARERR;
        break;
    }

    return res;
}

DWORD get_fattime (void)
{ /*
    struct tm t;
    DWORD date;
    t = Time_GetCalendarTime();
    t.tm_year -= 1980;		//��ݸ�Ϊ1980����
    t.tm_mon++;         	//0-11�¸�Ϊ1-12��
    t.tm_sec /= 2;      	//��������Ϊ0-29
    
    date = 0;
    date = (t.tm_year << 25)|(t.tm_mon<<21)|(t.tm_mday<<16)|\
            (t.tm_hour<<11)|(t.tm_min<<5)|(t.tm_sec);

    return date;*/
	return 0;
}

//�õ��ļ�Calendar��ʽ�Ľ�������,��DWORD get_fattime (void) ��任							
//struct tm
void get_Calendarfattime (DWORD date,DWORD time)
{  	/*
    struct tm t;
	DWORD time_t;

	t.tm_year=(date>>9)&0x007F;  //bit 9-15
	t.tm_mon=(date>>5)&0x000F;   //bit 5-8
	t.tm_mday=date&0x001F;       //bit 0-4

	t.tm_hour=(time>>11)&0x001F; //bit 11-15
	t.tm_min=(time>>5)&0x003F;   //bit  5-10
	t.tm_sec=(time&0x001F)<<1;   //bit  0-4

	t.tm_year+=1980;		     //��ݸ�Ϊ1980�����ļ�FAT��¼��ʽ
	t.tm_mon--;					 //FAT��¼��ʽ1-12,Calendar��Ӧ0-11

	//��������������ҪĿ����Ҫ�õ����ڶ�Ӧ�����������Ƚ����
    time_t=Time_ConvCalendarToUnix(t);
	t=Time_ConvUnixToCalendar(time_t);

    return t; */

}


