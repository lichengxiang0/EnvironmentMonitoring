/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			24C02.c
** Descriptions:		24C02 ���������� 
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2010-10-29
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "24C02.h"


/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : EEPROM�ܽ�����
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void I2C_Configuration(void)
{
   I2C_InitTypeDef  I2C_InitStructure;//����I2C���õ��Ľṹ�����
   GPIO_InitTypeDef  GPIO_InitStructure;//�����������ýṹ����� 

   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);//��I2C1�ĵ�ʱ��
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);//��GPIOB��AFIO��ʱ��

   /* Configure I2C1 pins: PB6->SCL and PB7->SDA */
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;//����PB6��PB7Ϊ���ÿ�©����Ϊ���ݺ�ʱ���Ǽȿ��������ֿ��������
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
		
   I2C_DeInit(I2C1);//��λI2C1��صļĴ���
   I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//I2Cģʽ
   I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//����ģʽTl/Th=2
   I2C_InitStructure.I2C_OwnAddress1 = 0x30;//�Լ��ĵ�ַΪ0x30
   I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//Ӧ��ʹ��
   I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//Ӧ��7λ��ַ
   I2C_InitStructure.I2C_ClockSpeed = 100000;  /* 100K�ٶ� */
    
   I2C_Cmd(I2C1, ENABLE);//I2Cʹ��
   I2C_Init(I2C1, &I2C_InitStructure);//���������ö�I2C��ʼ��
   /*����1�ֽ�1Ӧ��ģʽ*/
   I2C_AcknowledgeConfig(I2C1, ENABLE);//ʹ��I2C1��Ӧ��
}
 
/*******************************************************************************
* Function Name  : I2C_delay
* Description    : �ӳ�ʱ��
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_delay(uint16_t cnt)
{
	while(cnt--);
}

/*******************************************************************************
* Function Name  : I2C_AcknowledgePolling
* Description    : �ȴ���ȡI2C���߿���Ȩ �ж�æ״̬
* Input          : - I2Cx:I2C�Ĵ�����ַ
*                  - I2C_Addr:��������ַ
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_AcknowledgePolling(I2C_TypeDef *I2Cx,uint8_t I2C_Addr)
{
  vu16 SR1_Tmp;
  do
  {   
    I2C_GenerateSTART(I2Cx, ENABLE); /*��ʼλ*/
    /*��SR1*/
    SR1_Tmp = I2C_ReadRegister(I2Cx, I2C_Register_SR1);
    /*������ַ(д)*/
#ifdef AT24C01A

	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
#else

	I2C_Send7bitAddress(I2Cx, 0, I2C_Direction_Transmitter);
#endif

  }while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));
  
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
    
  I2C_GenerateSTOP(I2Cx, ENABLE);  /*ֹͣλ*/  
}


/*******************************************************************************
* Function Name  : I2C_Read
* Description    : ͨ��ָ��I2C�ӿڶ�ȡ����ֽ�����
* Input          : - I2Cx:I2C�Ĵ�����ַ
*                  - I2C_Addr:��������ַ
*                  - addr:Ԥ��ȡ�ֽڴ洢λ��
*                  - *buf:��ȡ���ݵĴ洢λ��
*                  - num:��ȡ�ֽ���
* Output         : None
* Return         : �ɹ�����0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
    if(num==0)
	return 1;
	
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
		
	/*����1�ֽ�1Ӧ��ģʽ*/
	I2C_AcknowledgeConfig(I2Cx, ENABLE);


	/* ������ʼλ */
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,��ģʽ*/

#ifdef AT24C01A	
    /*����������ַ(д)*/
    I2C_Send7bitAddress(I2Cx,  I2C_Addr, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/*���͵�ַ*/
	I2C_SendData(I2Cx, addr);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));/*�����ѷ���*/
		
	/*��ʼλ*/
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	
	/*������*/
	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		
#else	
	/*����������ַ(��)24C01*/
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
#endif
	
    while (num)
    {
		if(num==1)
		{
     		I2C_AcknowledgeConfig(I2Cx, DISABLE);	/* ���һλ��Ҫ�ر�Ӧ��� */
    		I2C_GenerateSTOP(I2Cx, ENABLE);			/* ����ֹͣλ */
		}
	    
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */
	    *buf = I2C_ReceiveData(I2Cx);
	    buf++;
	    /* Decrement the read bytes counter */
	    num--;
    }
	/* �ٴ�����Ӧ��ģʽ */
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	return 0;
}	

/*******************************************************************************
* Function Name  : I2C_WriteOneByte
* Description    : ͨ��ָ��I2C�ӿ�д��һ���ֽ�����
* Input          : - I2Cx:I2C�Ĵ�����ַ
*                  - I2C_Addr:��������ַ
*                  - addr:Ԥд���ֽڵ�ַ
*                  - value:д������
* Output         : None
* Return         : �ɹ�����0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t value)
{
    /* ��ʼλ */
  	I2C_GenerateSTART(I2Cx, ENABLE);
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));  

#ifdef AT24C01A
  	/* ����������ַ(д)*/
  	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);//����ģʽ
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  	/*���͵�ַ*/
  	I2C_SendData(I2Cx, addr);//�洢�������ݵĵ�ַ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#else	
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Transmitter);
 	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

	/* дһ���ֽ�*/
  	I2C_SendData(I2Cx, value);//�洢�������� 
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
  	/* ֹͣλ*/
  	I2C_GenerateSTOP(I2Cx, ENABLE);
  
  	I2C_AcknowledgePolling(I2Cx,I2C_Addr);//�ȴ���ȡI2C���ߵĿ���Ȩ

	I2C_delay(1000);

	return 0;
}


/*******************************************************************************
* Function Name  : I2C_Write
* Description    : ͨ��ָ��I2C�ӿ�д�����ֽ�����
* Input          : - I2Cx:I2C�Ĵ�����ַ
*                  - I2C_Addr:��������ַ
*                  - addr:Ԥд���ֽڵ�ַ
*                  - *buf:Ԥд�����ݴ洢λ��
*                  - num:д���ֽ���
* Output         : None
* Return         : �ɹ�����0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
	uint8_t err=0;
	
	while(num--)
	{
		if(I2C_WriteOneByte(I2Cx, I2C_Addr,addr++,*buf++))
		{
			err++;
		}
	}
	if(err)
		return 1;
	else 
		return 0;	
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
