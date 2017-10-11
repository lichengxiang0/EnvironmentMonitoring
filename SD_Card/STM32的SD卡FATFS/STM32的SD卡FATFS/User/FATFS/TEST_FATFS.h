	

#ifndef __TEST_FATFS_h
#define __TEST_FATFS_h

#define TEST_FATFS_EN
#ifdef TEST_FATFS_EN

#include "config.h" 
#include "integer.h"
//#include "diskio.h" 
#define TRUE 1
#define FALSE 0
//#define NULL (void*)0

typedef	unsigned char  BOOL;

FRESULT Test_f_mkfs(BYTE drv);//�Կ����и�ʽ������������һ��Ϊ0
FRESULT Test_f_mkdir(const TCHAR* );//����һ����Ŀ¼,����Ŀ¼��·��
FRESULT Test_f_deldir(const TCHAR* path);//ɾ��һ��Ŀ¼,����Ŀ¼��·��	
FRESULT Test_f_readdir(const TCHAR* path,char (*filename)[13]);//��ȡĿ¼�е��ļ�,����Ŀ¼��·��
FRESULT Test_f_creat(const TCHAR* path);//�����ļ������ļ�·�����ļ���
FRESULT Test_f_delfile(const TCHAR* path);//ɾ��һ���ļ�,����Ŀ¼��·��
FRESULT Test_f_readfile(const TCHAR* path,char *buff,DWORD ofs,UINT strl);//���ļ������ݣ����û�д��ļ��򷵻ش���;�����ļ���·���������ݻ��棬��ȡ��ʼƫ��ָ�룬��ȡ���ֽ���										
FRESULT Test_f_writefile(const TCHAR* path,char *buff,DWORD ofs,UINT strl);//д���ݵ��ļ������û�д��ļ��򴴽��ļ�;�����ļ���·���������ݻ��棬д��ʼƫ��ָ�룬д���ֽ���
FRESULT Test_f_getfreem(DWORD *freem);//��ȡ����ʣ��������λΪM
//The f_getfree function gets number of the free clusters
void Test_f_getfree(void);
//The f_read function reads data from a file.
void Test_f_read(void);
//The f_write writes data to a file.
void Test_f_write(void);
//The f_mkdir function creates a new directory
//void Test_f_mkdir(void);
//The f_readdir function reads directory entries.
//void Test_f_readdir(void);
//Rename file or directory
//void Test_f_rename(void);
//The f_unlink removes file or directory
//void Test_f_unlink(void);
//The f_mkfs fucntion creates a file system on the drive.


extern BYTE disk_detect_OK(void);
u8 StrToData(u8 * data, u8 len);

#endif

#endif


