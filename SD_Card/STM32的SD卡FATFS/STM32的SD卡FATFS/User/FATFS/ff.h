/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module include file  R0.09     (C)ChaN, 2011
/----------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following trems.
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/----------------------------------------------------------------------------*/

#ifndef _FATFS
#define _FATFS	6502	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

#include "integer.h"	/* Basic integer types */
#include "ffconf.h"		/* FatFs configuration options */

#if _FATFS != _FFCONF
#error Wrong configuration file (ffconf.h).
#endif



/* Definitions of volume management */

#if _MULTI_PARTITION		/* Multiple partition configuration �ñ�������Ϊ1ʱ��֧��һ�����̵Ķ������*/
typedef struct {
	BYTE pd;	/* Physical drive number */
	BYTE pt;	/* Partition: 0:Auto detect, 1-4:Forced partition) */
} PARTITION;
extern PARTITION VolToPart[];	/* Volume - Partition resolution table */
#define LD2PD(vol) (VolToPart[vol].pd)	/* Get physical drive number ��ô��̶�Ӧ��������� */
#define LD2PT(vol) (VolToPart[vol].pt)	/* Get partition index ��ô��̶�Ӧ�ķ���*/

#else						/* Single partition configuration */
#define LD2PD(vol) (vol)	/* Each logical drive is bound to the same physical drive number */
#define LD2PT(vol) 0		/* Always mounts the 1st partition or in SFD */

#endif



/* Type of path name strings on FatFs API */

#if _LFN_UNICODE			/* Unicode string */
#if !_USE_LFN
#error _LFN_UNICODE must be 0 in non-LFN cfg.
#endif
#ifndef _INC_TCHAR
typedef WCHAR TCHAR;
#define _T(x) L ## x
#define _TEXT(x) L ## x
#endif

#else						/* ANSI/OEM string */
#ifndef _INC_TCHAR
typedef char TCHAR;
#define _T(x) x
#define _TEXT(x) x
#endif

#endif



/* File system object structure (FATFS)�ļ�ϵͳ�Ľṹ����� */

typedef struct {
	BYTE	fs_type;		/* FAT sub-type (0:Not mounted) �ļ�ϵͳ���ͣ�0Ϊ���ļ�ϵͳ��*/
	BYTE	drv;			/* Physical drive number ���������ţ�һ��Ϊ0�� f_chdrive();Ӱ���ֵ*/
	BYTE	csize;			/* Sectors per cluster (1,2,4...128) ÿ�ص�������*/
	BYTE	n_fats;			/* Number of FAT copies (1,2) FATFS�ĸ����� */
	BYTE	wflag;			/* win[] dirty flag (1:must be written back) �ļ��Ƿ�Ķ��ı�־��Ϊ1ʱҪ��д*/
	BYTE	fsi_flag;		/* fsinfo dirty flag (1:must be written back) �ļ�ϵͳ��Ϣ�Ƿ�Ķ�����ʶ*/
	WORD	id;				/* File system mount ID �ļ�ϵͳ����ID*/
	WORD	n_rootdir;		/* Number of root directory entries (FAT12/16) ��Ŀ¼��Ŀ¼�����Ŀ*/
#if _MAX_SS != 512
	WORD	ssize;			/* Bytes per sector (512, 1024, 2048 or 4096) ÿ�������ֽ���*/
#endif
#if _FS_REENTRANT
	_SYNC_t	sobj;			/* Identifier of sync object �������룬����ͬ������*/
#endif
#if !_FS_READONLY//�ļ�Ϊ��д
	DWORD	last_clust;		/* Last allocated cluster ������ĵ�һ����*/
	DWORD	free_clust;		/* Number of free clusters �մص���Ŀ*/
	DWORD	fsi_sector;		/* fsinfo sector (FAT32) �ļ�ϵͳ��Ϣ����*/
#endif
#if _FS_RPATH
	DWORD	cdir;			/* Current directory start cluster (0:root) ��ǰĿ¼�Ŀ�ʼ����f_chdir("/dir1");������� */
#endif
	DWORD	n_fatent;		/* Number of FAT entries (= number of clusters + 2)���ص�����;��ֵ-2���Ǵ��̵Ŀ��ô���Ŀ */
	DWORD	fsize;			/* Sectors per FAT ÿ��FAT��������*/
	DWORD	fatbase;		/* FAT start sector FAT��ʼ������ */
	DWORD	dirbase;		/* Root directory start sector (FAT32:Cluster#)FAT32�ĸ�Ŀ¼��ʼ���� */
	DWORD	database;		/* Data start sector���ݿ�ʼ���� */
	DWORD	winsect;		/* Current sector appearing in the win[] Ŀǰ��������win[]����*/
	BYTE	win[_MAX_SS];	/* Disk access window for Directory, FAT (and Data on tiny cfg)  ���̻���*/
} FATFS;



/* File object structure (FIL) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object ָ����Ӧ�ļ�ϵͳ����*/
	WORD	id;				/* Owner file system mount ID �ļ�ϵͳ����ID*/
	BYTE	flag;			/* File status flags �ļ�״̬��־*/
	BYTE	pad1;
	DWORD	fptr;			/* File read/write pointer (0 on file open) �ļ��Ķ�дָ��*/
	DWORD	fsize;			/* File size �ļ��Ĵ�С*/
	DWORD	sclust;			/* File start cluster (0 when fsize==0) �ļ���ʼ�Ĵغ�*/
	DWORD	clust;			/* Current cluster ��ǰ�Ĵغ�*/
	DWORD	dsect;			/* Current data sector ��ǰ���ݵ���������*/
#if !_FS_READONLY
	DWORD	dir_sect;		/* Sector containing the directory entry ���ļ�Ŀ¼���Ӧ���ڵ�����*/
	BYTE*	dir_ptr;		/* Ponter to the directory entry in the window Ŀ¼��ָ��*/
#endif
#if _USE_FASTSEEK
	DWORD*	cltbl;			/* Pointer to the cluster link map table (null on file open)ָ��ص���·ӳ��� */
#endif
#if _FS_SHARE
	UINT	lockid;			/* File lock ID (index of file semaphore table) �ļ������ţ������ļ��ź�����*/
#endif
#if !_FS_TINY
	BYTE	buf[_MAX_SS];	/* File data read/write buffer �ļ���д�Ļ���*/
#endif
} FIL;



/* Directory object structure (DIR) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object  ָ����Ӧ�ļ�ϵͳ����*/
	WORD	id;				/* Owner file system mount ID �ļ�ϵͳ����ID*/
	WORD	index;			/* Current read/write index number ��ǰ��д��������*/
	DWORD	sclust;			/* Table start cluster (0:Root dir) ��ʼ�Ĵ�*/
	DWORD	clust;			/* Current cluster ��ǰ��*/
	DWORD	sect;			/* Current sector ��ǰ����*/
	BYTE*	dir;			/* Pointer to the current SFN entry in the win[] */
	BYTE*	fn;				/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
#if _USE_LFN
	WCHAR*	lfn;			/* Pointer to the LFN working buffer ָ���ļ���������*/
	WORD	lfn_idx;		/* Last matched LFN index number (0xFFFF:No LFN) */
#endif
} DIR;



/* File status structure (FILINFO) */

typedef struct {
	DWORD	fsize;			/* File size �ļ���С*/
	WORD	fdate;			/* Last modified date ����޸ĵ�����*/
	WORD	ftime;			/* Last modified time ����޸ĵ�ʱ��*/
	BYTE	fattrib;		/* Attribute ����*/
	TCHAR	fname[13];		/* Short file name (8.3 format) ���ļ���*/
#if _USE_LFN
	TCHAR*	lfname;			/* Pointer to the LFN buffer ָ���ļ�������*/
	UINT 	lfsize;			/* Size of LFN buffer in TCHAR ���ļ����Ļ����С*/
#endif
} FILINFO;



/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occured in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Acces denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Acces denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file shareing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

FRESULT f_mount (BYTE, FATFS*);						/* Mount/Unmount a logical drive ��װ/ж��һ���߼������� ������װ��ж�ص��߼��������ţ�ָ��һ���µ��ļ�ϵͳ����*/
FRESULT f_open (FIL*, const TCHAR*, BYTE);			/* Open or create a file�򿪻򴴽�һ���ļ���ָ��һ���հ׵��ļ�ָ�룬ָ���ļ���������ģʽ���ļ���ģʽ��־�� */
FRESULT f_read (FIL*, void*, UINT, UINT*);			/* Read data from a file���ļ��ж����ݣ��ļ��ṹ�壨�ļ��򿪺����еõ������ļ���ȡ����������ȡ���ֽ�����ָ���ȡ���ֽ����� */
FRESULT f_lseek (FIL*, DWORD);						/* Move file pointer of a file object �ƶ��ļ�ָ�루�ļ����󣬴��ļ�������ʼ���ļ�ָ�룩*/
FRESULT f_close (FIL*);								/* Close an open file object �رմ򿪵��ļ��ṹ��*/
FRESULT f_opendir (DIR*, const TCHAR*);				/* Open an existing directory ��һ�����ڵ�Ŀ¼��ָ��һ��������Ŀ¼�ṹ�壬ָ��Ŀ¼·����*/
FRESULT f_readdir (DIR*, FILINFO*);					/* Read a directory item ��ȡһ��Ŀ¼�ָ��򿪵�Ŀ¼�ṹ�壬ָ�򷵻ص��ļ���Ϣ�Ľṹ�壩*/
FRESULT f_stat (const TCHAR*, FILINFO*);			/* Get file status ��ȡ�ļ�״̬��ָ���ļ�·����ָ�򷵻ص��ļ���Ϣ�ṹ�壩*/
FRESULT f_write (FIL*,const void*, UINT, UINT*);	/* Write data to a file д���ݵ��ļ���ָ���ļ��ṹ�壬ָ��д���ݣ���д���ݵ���Ŀ��ָ��д������Ŀ��ָ�룩*/
FRESULT f_getfree (const TCHAR*, DWORD*, FATFS**);	/* Get number of free clusters on the drive ��ȡ�������Ŀ��дغţ�ָ���������ţ���Ŀ¼����ָ�򷵻ؿմغŵı�����ָ��ָ�򷵻���Ӧ���ļ�ϵͳ�Ľṹ�壩*/
FRESULT f_truncate (FIL*);							/* Truncate file �ض��ļ���ָ���ļ��Ľṹ�壩*/
FRESULT f_sync (FIL*);								/* Flush cached data of a writing file ˢ��д���ļ��Ļ������ݣ�ָ���ļ��Ľṹ�壩*/
FRESULT f_unlink (const TCHAR*);					/* Delete an existing file or directory ɾ�����ڵ��ļ���Ŀ¼��ָ���ļ���Ŀ¼��·����*/
FRESULT	f_mkdir (const TCHAR*);						/* Create a new directory ����һ����Ŀ¼��ָ��Ŀ¼��·����*/
FRESULT f_chmod (const TCHAR*, BYTE, BYTE);			/* Change attriburte of the file/dir �ı��ļ���Ŀ¼�����ԣ�ָ��Ŀ¼��·��������λ��ȥ�ı���������룩*/
FRESULT f_utime (const TCHAR*, const FILINFO*);		/* Change timestamp of the file/dir �ı��ļ���Ŀ¼��ʱ�����ָ���ļ���Ŀ¼����ָ�����õ�ʱ�����*/
FRESULT f_rename (const TCHAR*, const TCHAR*);		/* Rename/Move a file or directory ���������ƶ�һ���ļ���Ŀ¼��ָ��ɵ��ļ�����ָ���µ��ļ�����*/
FRESULT f_chdrive (BYTE);							/* Change current drive �ı䵱ǰ�������߼��������ţ�*/
FRESULT f_chdir (const TCHAR*);						/* Change current directory �ı䵱ǰĿ¼��ָ��Ŀ¼·����*/
FRESULT f_getcwd (TCHAR*, UINT);					/* Get current directory ��ȡ��ǰĿ¼��ָ��Ŀ¼·����·���Ĵ�С��*/
FRESULT f_forward (FIL*, UINT(*)(const BYTE*,UINT), UINT, UINT*);	/* Forward data to the stream ��������ָ���ļ��Ľṹ�壬ָ������������ǰ���ֽ�����ָ����ǰ���ֽ�����*/
FRESULT f_mkfs (BYTE, BYTE, UINT);					/* Create a file system on the drive ������������һ���ļ�ϵͳ���������ţ���������0:FDISK, 1:SFD��������ĵ�Ԫ��С��*/
FRESULT	f_fdisk (BYTE, const DWORD[], void*);		/* Divide a physical drive into some partitions �����������������������������ţ�ָ��ÿһ��������С�ĳߴ��ָ�򻺴棩*/
int f_putc (TCHAR, FIL*);							/* Put a character to the file ��һ���ַ����ļ�����������ַ���ָ���ļ��ṹ�壩*/
int f_puts (const TCHAR*, FIL*);					/* Put a string to the file ��һ���ַ������ļ���ָ��������ַ�����ָ���ļ��ṹ�壩*/
int f_printf (FIL*, const TCHAR*, ...);				/* Put a formatted string to the file ���и�ʽ���ַ������ļ���ָ���ļ��ṹ�壬ָ���ַ�������ѡ������*/
TCHAR* f_gets (TCHAR*, int, FIL*);					/* Get a string from the file ���ļ��л�ȡһ���ַ�����ָ���ַ����Ļ��棬�ַ�������Ĵ�С��ָ���ļ��ṹ�壩*/

#define f_eof(fp) (((fp)->fptr == (fp)->fsize) ? 1 : 0)
#define f_error(fp) (((fp)->flag & FA__ERROR) ? 1 : 0)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->fsize)

#ifndef EOF
#define EOF (-1)
#endif




/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
#if !_FS_READONLY//ֻ��ģʽ
DWORD get_fattime (void);
#endif

/* Unicode support functions */ //ʹ�ó��ļ���
#if _USE_LFN						/* Unicode - OEM code conversion */
WCHAR ff_convert (WCHAR, UINT);		/* OEM-Unicode bidirectional conversion */
WCHAR ff_wtoupper (WCHAR);			/* Unicode upper-case conversion */
#if _USE_LFN == 3					/* Memory functions */
void* ff_memalloc (UINT);			/* Allocate memory block */
void ff_memfree (void*);			/* Free memory block */
#endif
#endif

/* Sync functions */
#if _FS_REENTRANT   //��������
int ff_cre_syncobj (BYTE, _SYNC_t*);/* Create a sync object ����ͬ������*/
int ff_req_grant (_SYNC_t);			/* Lock sync object ����ͬ������*/
void ff_rel_grant (_SYNC_t);		/* Unlock sync object ����ͬ������*/
int ff_del_syncobj (_SYNC_t);		/* Delete a sync object ɾ��ͬ������*/
#endif




/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access control and file status flags (FIL.flag) �ļ����ʿ��Ƽ��ļ�״̬��ʶ*/

#define	FA_READ				0x01 //���ļ�
#define	FA_OPEN_EXISTING	0x00 //���ļ�������ļ������ڣ����ʧ�ܡ�(Ĭ��)
#define FA__ERROR			0x80

#if !_FS_READONLY
#define	FA_WRITE			0x02 //д�ļ�
#define	FA_CREATE_NEW		0x04 //����һ�����ļ�������ļ��Ѵ��ڣ��򴴽�ʧ�ܡ�
#define	FA_CREATE_ALWAYS	0x08 //����һ�����ļ�������ļ��Ѵ��ڣ����������ضϲ����ǡ�
#define	FA_OPEN_ALWAYS		0x10 //����ļ����ڣ���򿪣����򣬴���һ�����ļ���
#define FA__WRITTEN			0x20
#define FA__DIRTY			0x40
#endif


/* FAT sub type (FATFS.fs_type) */

#define FS_FAT12	1//�ļ�ϵͳ������
#define FS_FAT16	2
#define FS_FAT32	3


/* File attribute bits for directory entry �ļ�����*/

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_MASK	0x3F	/* Mask of defined bits */


/* Fast seek feature */
#define CREATE_LINKMAP	0xFFFFFFFF



/*--------------------------------*/
/* Multi-byte word access macros  */

#if _WORD_ACCESS == 1	/* Enable word access to the FAT structure */
#define	LD_WORD(ptr)		(WORD)(*(WORD*)(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(*(DWORD*)(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(WORD*)(BYTE*)(ptr)=(WORD)(val)
#define	ST_DWORD(ptr,val)	*(DWORD*)(BYTE*)(ptr)=(DWORD)(val)
#else					/* Use byte-by-byte access to the FAT structure */
#define	LD_WORD(ptr)		(WORD)(((WORD)*((BYTE*)(ptr)+1)<<8)|(WORD)*(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(((DWORD)*((BYTE*)(ptr)+3)<<24)|((DWORD)*((BYTE*)(ptr)+2)<<16)|((WORD)*((BYTE*)(ptr)+1)<<8)|*(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(BYTE*)(ptr)=(BYTE)(val); *((BYTE*)(ptr)+1)=(BYTE)((WORD)(val)>>8)
#define	ST_DWORD(ptr,val)	*(BYTE*)(ptr)=(BYTE)(val); *((BYTE*)(ptr)+1)=(BYTE)((WORD)(val)>>8); *((BYTE*)(ptr)+2)=(BYTE)((DWORD)(val)>>16); *((BYTE*)(ptr)+3)=(BYTE)((DWORD)(val)>>24)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _FATFS */
