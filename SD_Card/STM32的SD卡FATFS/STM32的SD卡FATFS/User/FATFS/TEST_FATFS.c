/*--------------File Info-------------------------------------------------------
** ��   ��   ��:  FATFS_Function.c
** ��        ��:~���е�Ҷ~    ����
** ����޸�����:  2010.01.18
** ��        ��:  V1.0
** ��        ��:  FATFS���ù��ܲ��Ժ��� .����PC�����ڣ��۲쳬���ն����
**------------------------------------------------------------------------------
** Created   by:  Dt.
** Created date:  2009.03.04
*******************************************************************************/ 
#include "TEST_FATFS.h"
#include "string.h"

#ifdef TEST_FATFS_EN



//�������Ƿ���
BOOL disk_detect_OK(void)
{
    if( disk_status(0)==STA_NODISK )/* Physical drive nmuber (0..) */
	{
	    printf("\r\n\r\n%48s\r\n","<δ��⵽����,����SD���Ƿ���...>");
		return FALSE;
	}
	return TRUE;
}



//���Ժ���ִ�н������
void die(FRESULT res)
{
    switch(res)
	{
	    case FR_OK:	//The function succeeded. 
		{
		    printf("\r\nThe function succeeded!\r\n");
			break;
		}
		case FR_NOT_READY://The disk drive cannot work due to no medium in the drive or any other reason
		{
		    printf("\r\nThe disk drive cannot work due to no medium in the drive or any other reason!\r\n");
			break;
		}
		case FR_NO_FILE://Could not find the file.
		{
		    printf("\r\nCould not find the file!\r\n");
			break;
		}
		case FR_NO_PATH://Could not find the path
		{
		    printf("\r\nCould not find the path!\r\n");
			break;
		}
		case FR_INVALID_NAME://The path name is invalid
		{
		    printf("\r\nThe path name is invalid!\r\n");
			break;
		}
		case FR_INVALID_DRIVE://The drive number is invalid
		{
		    printf("\r\nThe drive number is invalid!\r\n");
			break;
		}
		case FR_DENIED://The directory cannot be created due to directory table or disk is full. 
		{
		    printf("\r\nThe directory cannot be created due to directory table or disk is full!\r\n");
			break;
		}
		case FR_EXIST://A file or directory that has same name is already existing
		{
		    printf("\r\nA file or directory that has same name is already existing!\r\n");
			break;
		}
//		case FR_RW_ERROR://The function failed due to a disk error or an internal error
/*
		case FR_RW_ERROR://The function failed due to a disk error or an internal error
		{
		    printp("\r\nThe function failed due to a disk error or an internal error!\r\n");
			break;
		}
*/
		case FR_WRITE_PROTECTED://The medium is write protected
		{
		    printf("\r\nThe medium is write protected!\r\n");
			break;
		}
		case FR_NOT_ENABLED://The logical drive has no work area
		{
		    printf("\r\nThe logical drive has no work area!\r\n");
			break;
		}
		case FR_NO_FILESYSTEM://There is no valid FAT partition on the disk
		{
		    printf("\r\nThere is no valid FAT partition on the disk!\r\n");
			break;
		}
		case FR_INVALID_OBJECT://The file object is invalid
		{
		    printf("\r\nThe file object is invalid!\r\n");
			break;
		}
       
	    //The function aborted before start in format due to a reason as follows. 
        //The disk size is too small. 
        //Invalid parameter was given to any parameter. 
        //Not allowable cluster size for this drive. This can occure when number of clusters becomes around 0xFF7 and 0xFFF7. 
		case FR_MKFS_ABORTED://
		{
		    printf("\r\nThe function aborted before start in format!\r\n");
			break;
		}
		
		default:
		{
		    printf("\r\nerror!\r\n");
			break;
		}	
	}
	return;
}
void Test_f_getfree(void)//��ȡ������������ʣ������
{
    FATFS fs;
	FATFS *pfs;
    DWORD clust;
	FRESULT res;         // FatFs function common result code

	//�������Ƿ���
	if( disk_detect_OK()==FALSE ) return;

	pfs=&fs;//ָ��
	// Register a work area for logical drive 0
    f_mount(0, &fs);//��װFATFS�����Ǹ�FATFS����ռ�

    // Get free clusters
    res = f_getfree("/", &clust, &pfs);//�����Ǹ�Ŀ¼��Ĭ�ϴ���0;"/"����"0:/"
    if ( res==FR_OK ) 
    {
	    // Get free space
        printf("\r\n%d MB total disk space.\r\n%d MB available on the disk.\r\n",
                (DWORD)(pfs->n_fatent - 2) * pfs->csize /2/1024,//�ܵĴ��̿ռ�M	=���ܴ���-2��*ÿ�ص�������/2/1024=���ô���*ÿ�ص�������/2/1024
                 clust * pfs->csize /2/1024);//���еĴ��̿ռ�M=ʣ�����*ÿ�ص�������/2/1024
	}
	else die(res);//���Ժ���ִ�н������
	
	// Unregister a work area before discard it
    f_mount(0, NULL);//ж��FATFS�������ͷ�FATFS�ṹ����ռ�ռ�
}

void Test_f_read(void)//���ļ������ݣ����û�д��ļ��򷵻ش���
{
    FATFS fs;            // Work area (file system object) for logical drive
    FIL fsrc;            // file objects
    BYTE buffer[512];     // file copy buffer
    FRESULT res;         // FatFs function common result code
    UINT br;             // File R count
	u16 i;
	

	char path[20];

	//�������Ƿ���
	if( disk_detect_OK()==FALSE ) return;

    // Register a work area for logical drive 0
    f_mount(0, &fs);

	printf("\r\nread file:>");
	USART_Scanf_Name(path);//ͨ�����������ļ�·����/dir/file.txt����0:dir/file.txt����0:/dir/file.txt

	//Open source file
    res = f_open(&fsrc, path, FA_OPEN_EXISTING | FA_READ);//�򿪴��ڵ��ļ������û���򷵻ش���
	die(res);
	
	//buffer�ռ����һ�㣬����߶����ٶȡ�
	//����ļ�ʵ�ʴ�С512byte��
	//��Ϊbuffer[512]ʱ��ֻ��Ҫѭ��һ�Σ������Ϊbuffer[1]����Ҫѭ��512�Ρ�
	//����������Ҫ��ȥ��1s��

	for (;;) 
	{
		for(i=0;i<sizeof(buffer);i++) buffer[i]='\0';//�������

		res = f_read(&fsrc, buffer, sizeof(buffer), &br);
        if (res ||(br == 0)) break;   // error or eof	
		
		printf("%s",buffer);	
    }

			    
    // Close all files
    f_close(&fsrc);
    // Unregister a work area before discard it
    f_mount(0, NULL);
}

void Test_f_write(void)//д���ݵ��ļ������û�д��ļ��򴴽��ļ�
{
    FATFS fs;            // Work area (file system object) for logical drive
    FRESULT res;         // FatFs function common result code
	FIL Make_file;
    char file_name[20];
    char Storage_buffer[] ="0";
	
	UINT bw;
	//�������Ƿ���
	if( disk_detect_OK()==FALSE ) return;
	 printf("\r\n inaert_ok:>");
    // Register a work area for logical drive 0
    f_mount(0, &fs);

	printf("\r\n Make file Name:>");
	USART_Scanf_Name(file_name);//ͨ����������Դ�ļ�·����/dir/file.txt����0:dir/file.txt����0:/dir/file.txt

    res = f_open(&Make_file, file_name,  FA_OPEN_ALWAYS | FA_WRITE); //��д��ʽ�� û���ļ��򴴽� 
	 printf("\r\n open_ok:>");
	 die(res);
    res = f_lseek(&Make_file, Make_file.fsize); //ָ���Ƶ��ļ����  
      printf("\r\n seek_ok:>");
	 die(res);
	res = f_write(&Make_file, Storage_buffer, (sizeof (Storage_buffer))-1 , &bw); //ÿ����Ҫд��������ֽ�����ȥ������\0����-1  
     printf("\r\n write_ok:>");
	 die(res);
	res = f_lseek(&Make_file, Make_file.fsize); //ָ���Ƶ��ļ����  
	f_close(&Make_file);//�ر��ļ�
	printf("\r\n close_ok:>");

	printf("\r\nд�ļ�����OK!\r\n");

	// Unregister a work area before discard it
    f_mount(0, NULL);
}

//The f_read function reads data from a file.
//��RAM���������������ʱ���ܳ���Ī������Ĵ���������Ƕ�կ����ˣ�֤����Ȼ������
//��Stack_Size   EQU   0x00000400  ��Ϊ Stack_Size   EQU  0x00000800��������
//�����Ժ�Ҫ�ر�ע��������⡣

u8 StrToData(u8 * data, u8 len)
{
	u8 ltemp;
	if(len == 1)
	{
		ltemp = data[0]-0x30;

	}
	else if(len == 2)
	{
		ltemp = (data[0]-0x30)*10 + (data[1]-0x30);

	 }
	//else if(len == 3)
		//ltemp = (data[0]-0x30)*100 + (data[1]-0x30)*10 + [data[2] - 0x30];

	return ltemp;
	
}


#endif


/*
int main(void)
{
///////////////////////////////////////////////////////////
UART1GPIO_config();//����IO������
USART1_config();//���ڳ�ʼ��������Ϊ19200
//UART1NVIC_config();//�����ж�
///////////////////////////////////////////////////////////
SDCard_SpiInit();//SD����IO�ڼ�SPIģ���ʼ����PA2�����⣨H��>L��;PA3 CD_CSƬѡ��PA5 SCK��PA6 MISO��PA7 MOSI������ģʽ
//SD_Init();//SD����ʼ�������ʹ�õ�FATFS����Բ�����䣬��ΪFATFS�ڴ��ļ��Ȳ���ʱ�Ѿ���������
Test_f_getfree();//��ȡSD����������ʣ������
Test_f_read();//SD�����ļ�����
Test_f_write();//SD��д�ļ�����
/////////////////////////////////////////////////////////// 
    while (1)
    {
	//printf("�õ�");
    }
}*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FRESULT Test_f_mkfs(BYTE drv)//�Կ����и�ʽ������������һ��Ϊ0
{
    FATFS fs;            // Work area (file system object) for logical drive
    FRESULT res;         // FatFs function common result code

	//�������Ƿ���
	//if(disk_detect_OK()==FALSE ) return ;
	//printf("\r\n inaert_ok:>");
	// Register a work area for logical drive 0
	f_mount(drv, &fs);
	res=f_mkfs(drv, 0, 0);/* Create a file system on the drive ������������һ���ļ�ϵͳ���������ţ���������0:FDISK, 1:SFD��������ĵ�Ԫ��С��*/
	//die(res);
	f_mount(0, NULL);
	return res;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FRESULT Test_f_mkdir(const TCHAR* path)//����һ����Ŀ¼,����Ŀ¼��·��Test_f_mkdir("/dir");ֻ��һ��һ���Ľ���Ŀ¼	
{
    FATFS fs;            // Work area (file system object) for logical drive
    FRESULT res;         // FatFs function common result code

	//�������Ƿ���
	//if(disk_detect_OK()==FALSE ) return ;
	//printf("\r\n inaert_ok:>");
	// Register a work area for logical drive 0
	f_mount(0, &fs);
	res=f_mkdir(path);//����һ����Ŀ¼
	//die(res);
	f_mount(0, NULL);
	return res;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FRESULT Test_f_deldir(const TCHAR* path)//ɾ��һ��Ŀ¼,����Ŀ¼��·��;Ŀ¼��������ǿյĲ���ɾ��Test_f_deldir("/dir");	
{
    FATFS fs;            // Work area (file system object) for logical drive
	//DIR	dir;
    FRESULT res;         // FatFs function common result code

	//�������Ƿ���
	//if(disk_detect_OK()==FALSE ) return ;
	//printf("\r\n inaert_ok:>");
	// Register a work area for logical drive 0
	f_mount(0, &fs);
	//f_opendir (&dir,path);
	res=f_unlink(path);//ɾ��һ��Ŀ¼
	//die(res);
	f_mount(0, NULL);
	return res;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FRESULT Test_f_readdir(const TCHAR* path,char (*filename)[13])//��ȡĿ¼�е��ļ�,����Ŀ¼��·��;����ļ������ļ�����Test_f_readdir("/DIR",name);char name[5][13]	
{
    FATFS fs;            // Work area (file system object) for logical drive
	DIR	dir;
	FILINFO finf;
    FRESULT res;         // FatFs function common result code

	//�������Ƿ���
	//if(disk_detect_OK()==FALSE ) return ;
	//printf("\r\n inaert_ok:>");
	// Register a work area for logical drive 0
	f_mount(0, &fs);
	f_opendir (&dir,path);

	while(((res=f_readdir(&dir,&finf))==FR_OK)&&(finf.fname[0]))//��ȡĿ¼�е��ļ�	
	{strcpy(*(filename++),finf.fname);
	//printf("%s",finf.fname);
	}
	//die(res);
	f_mount(0, NULL);
	return res;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FRESULT Test_f_creat(const TCHAR* path)//�����ļ��������ļ�·�����ļ���
{
    FATFS fs;            // Work area (file system object) for logical drive
	FIL file;
    FRESULT res;         // FatFs function common result code

	//�������Ƿ���
	//if(disk_detect_OK()==FALSE ) return ;
	//printf("\r\n inaert_ok:>");
	// Register a work area for logical drive 0
	f_mount(0, &fs);
	res=f_open(&file,path,FA_CREATE_NEW); //����һ�����ļ�������ļ��Ѵ��ڣ��򴴽�ʧ��//FA_CREATE_NEW����һ�����ļ�������ļ��Ѵ��ڣ��򴴽�ʧ�ܡ�
                                                                                      //FA_CREATE_ALWAYS	//����һ�����ļ�������ļ��Ѵ��ڣ����������ضϲ����ǡ�
                                                                                      //FA_OPEN_ALWAYS    //����ļ����ڣ���򿪣����򣬴���һ�����ļ���
	//die(res);
	f_close(&file);//�ر��ļ�
	f_mount(0, NULL);
	return res;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FRESULT Test_f_delfile(const TCHAR* path)//ɾ��һ���ļ�,�����ļ���·��;Test_f_delfile("/dir");	
{
    FATFS fs;            // Work area (file system object) for logical drive
	//DIR	dir;
    FRESULT res;         // FatFs function common result code

	//�������Ƿ���
	//if(disk_detect_OK()==FALSE ) return ;
	//printf("\r\n inaert_ok:>");
	// Register a work area for logical drive 0
	f_mount(0, &fs);
	//f_opendir (&dir,path);
	res=f_unlink(path);//ɾ��һ���ļ�
	//die(res);
	f_mount(0, NULL);
	return res;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FRESULT Test_f_readfile(const TCHAR* path,char *buff,DWORD ofs,UINT strl)//���ļ������ݣ����û�д��ļ��򷵻ش���;�����ļ���·���������ݻ��棬��ȡ��ʼƫ��ָ�룬��ȡ���ֽ���
{																		  //Test_f_readfile("/dir/r.txt",str,0,sizeof(strr));
    FATFS fs;            // Work area (file system object) for logical drive
    FIL file;            // file objects
    FRESULT res;         // FatFs function common result code
    UINT br;             // File R count �ļ����ص��ֽڼ���
	//u16 i;
	//�������Ƿ���
	//if( disk_detect_OK()==FALSE ) return;
    // Register a work area for logical drive 0
	//for(i=0;i<sizeof(buff);i++) buff[i]='\0';
    f_mount(0, &fs);
	//Open source file
    res = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);//�򿪴��ڵ��ļ������û���򷵻ش���
	//die(res);
	res = f_lseek(&file,ofs); //ָ���Ƶ��ļ�ofs���ֽڴ�
	//buffer�ռ����һ�㣬����߶����ٶȡ�
	//����ļ�ʵ�ʴ�С512byte��
	//��Ϊbuffer[512]ʱ��ֻ��Ҫѭ��һ�Σ������Ϊbuffer[1]����Ҫѭ��512�Ρ�
	//for (;;) 
	{
		//for(i=0;i<sizeof(buff);i++) buff[i]='\0';//�������

		res = f_read(&file, buff, strl, &br);
        //if (res ||(br == 0)) break;   // error or eof���������ߵ��ļ�ĩβ���˳�
		
		//printf("%s",buff);	
    }		    
    // Close all files
    f_close(&file);
    // Unregister a work area before discard it
    f_mount(0, NULL);
	return res;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FRESULT Test_f_writefile(const TCHAR* path,char *buff,DWORD ofs,UINT strl)//д���ݵ��ļ������û�д��ļ��򴴽��ļ�;�����ļ���·���������ݻ��棬д��ʼƫ��ָ�룬д���ֽ���
{																		  //Test_f_writefile("/dir/r.txt",strw,0,strlen(strw));
    FATFS fs;            // Work area (file system object) for logical drive
    FRESULT res;         // FatFs function common result code
	FIL file;
	UINT bw;             //�ļ�д����ֽڼ���
	//�������Ƿ���
	//if( disk_detect_OK()==FALSE ) return;
    // Register a work area for logical drive 0
    f_mount(0, &fs);
    res = f_open(&file, path,  FA_OPEN_ALWAYS | FA_WRITE); //��д��ʽ�� û���ļ��򴴽� 
	 //die(res);
    res = f_lseek(&file, ofs); //ָ���Ƶ��ļ�ofs��������file.fsizeָ���Ƶ��ļ���� 
	 //die(res);
	res = f_write(&file, buff, strl, &bw); //ÿ����Ҫд��������ֽ���;���btrΪstrlen(buff)��д�����е��ַ�
	 //die(res);
	//res = f_lseek(&file, Make_file.fsize); //ָ���Ƶ��ļ����,һ����һ�ε�д��  
	f_close(&file);//�ر��ļ�
	// Unregister a work area before discard it
    f_mount(0, NULL);
	return res;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FRESULT Test_f_getfreem(DWORD *freem)//��ȡ����ʣ��������λΪM
{
    FATFS fs;
	FATFS *pfs;
	FRESULT res;         // FatFs function common result code
	//�������Ƿ���
	//if( disk_detect_OK()==FALSE ) return;
	// Register a work area for logical drive 0
	pfs=&fs;//ָ��
    f_mount(0, &fs);//��װFATFS�����Ǹ�FATFS����ռ�
    // Get free clusters
    res = f_getfree("/",freem,&pfs);//�����Ǹ�Ŀ¼��Ĭ�ϴ���0;"/"����"0:/"
	//die(res);
	if ( res==FR_OK ) 
    {
	    // Get free space
        //printf("\r\n%d MB total disk space.\r\n%d MB available on the disk.\r\n",
               //(DWORD)(pfs->n_fatent - 2) * (pfs->csize) /2/1024,//�ܵĴ��̿ռ�M	=���ܴ���-2��*ÿ�ص�������/2/1024=���ô���*ÿ�ص�������/2/1024
                 //(*freem)*(pfs->csize)/2/1024);//���еĴ��̿ռ�M=ʣ�����*ÿ�ص�������/2/1024
	 *freem=((*freem)*pfs->csize)/2/1024;
	}
	//else die(res);//���Ժ���ִ�н������
	// Unregister a work area before discard it
    f_mount(0, NULL);//ж��FATFS�������ͷ�FATFS�ṹ����ռ�ռ�
	return res;
}



