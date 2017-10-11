/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : Spi_SD_Driver.h
* Author             : ~���е�Ҷ~
* Version            : V3.0.1
* Date               : 03/12/2012
* Description        : Header for Spi_SD_Driver.c file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SD_SPI_FLASH_H
#define __SD_SPI_FLASH_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_lib.h"
#include "config.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */
///typedef struct SD_STRUCT
///{	
///	u8 card_insert;				/*���Ƿ���룻����Ϊ1������Ϊ0*/
///	u8 card_type;               /* ���� */	
///	u32 block_num;				/* ���п������ */
///	u32 block_len;				/* ���Ŀ鳤��(��λ:�ֽ�)��δ�� */
///	u32 erase_unit;				/* һ�οɲ����Ŀ������δ�� */
///	u8 errer;				    /* SD���Ĵ�����Ϣ */
///}sd_struct;
///extern sd_struct sd;
//#define NO_CARD      99//FatFS�����־��û�в������
/* Private define ------------------------------------------------------------*/
/* SD�����Ͷ��� */
#define SD_TYPE_MMC     0
#define SD_TYPE_V1      1
#define SD_TYPE_V2      2
#define SD_TYPE_V2HC    4

/* SPI�����ٶ�����*/
#define SPI_SPEED_LOW   0
#define SPI_SPEED_HIGH  1

/* SD�������ݽ������Ƿ��ͷ����ߺ궨�� */
#define NO_RELEASE      0
#define RELEASE         1

/* SD��ָ��� */
#define CMD0    0       //����λ
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define ACMD23  23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define ACMD41  41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00

#define SD_Insert_Status()    GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)//��������  

/* Exported macro ------------------------------------------------------------*/
/* Select SPI FLASH: Chip Select pin low  */
#define SD_CS_ENABLE()       GPIO_ResetBits(GPIOA, GPIO_Pin_3)
/* Deselect SPI FLASH: Chip Select pin high */
#define SD_CS_DISABLE()      GPIO_SetBits(GPIOA, GPIO_Pin_3)

/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
/* Private function prototypes -----------------------------------------------*/
void SDCard_SpiInit(void);
void SPI_SetSpeed(u8 SpeedSet);

u8 SPI_ReadWriteByte(u8 TxData);                //SPI���߶�дһ���ֽ�
u8 SD_WaitReady(void);                          //�ȴ�SD������
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc);     //SD������һ������
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg, u8 crc);
u8 SD_Init(void);                               //SD����ʼ��
                                                //
u8 SD_ReceiveData(u8 *data, u16 len, u8 release);//SD��������
u8 SD_GetCID(u8 *cid_data);                     //��SD��CID
u8 SD_GetCSD(u8 *csd_data);                     //��SD��CSD
u32 SD_GetCapacity(void);                       //ȡSD������

u8 SD_ReadSingleBlock(u32 sector, u8 *buffer);  //��һ��sector
u8 SD_WriteSingleBlock(u32 sector,const u8 *buffer); //дһ��sector
u8 SD_ReadMultiBlock(u32 sector,u8 *buffer,u8 count); //�����sector
u8 SD_WriteMultiBlock(u32 sector,const u8 *data,u8 count);  //д���sector

#endif /* __SD_SPI_FLASH_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
