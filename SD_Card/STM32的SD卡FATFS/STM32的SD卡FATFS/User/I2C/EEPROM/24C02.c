/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			24C02.c
** Descriptions:		24C02 操作函数库 
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
* Description    : EEPROM管脚配置
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void I2C_Configuration(void)
{
   I2C_InitTypeDef  I2C_InitStructure;//定义I2C所用到的结构体变量
   GPIO_InitTypeDef  GPIO_InitStructure;//定义引脚所用结构体变量 

   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);//打开I2C1的的时钟
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);//打开GPIOB和AFIO的时钟

   /* Configure I2C1 pins: PB6->SCL and PB7->SDA */
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;//设置PB6和PB7为复用开漏，因为数据和时钟是既可以输入又可以输出的
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
		
   I2C_DeInit(I2C1);//复位I2C1相关的寄存器
   I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//I2C模式
   I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//快速模式Tl/Th=2
   I2C_InitStructure.I2C_OwnAddress1 = 0x30;//自己的地址为0x30
   I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//应答使能
   I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//应答7位地址
   I2C_InitStructure.I2C_ClockSpeed = 100000;  /* 100K速度 */
    
   I2C_Cmd(I2C1, ENABLE);//I2C使能
   I2C_Init(I2C1, &I2C_InitStructure);//按以上配置对I2C初始化
   /*允许1字节1应答模式*/
   I2C_AcknowledgeConfig(I2C1, ENABLE);//使能I2C1的应答
}
 
/*******************************************************************************
* Function Name  : I2C_delay
* Description    : 延迟时间
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
* Description    : 等待获取I2C总线控制权 判断忙状态
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_AcknowledgePolling(I2C_TypeDef *I2Cx,uint8_t I2C_Addr)
{
  vu16 SR1_Tmp;
  do
  {   
    I2C_GenerateSTART(I2Cx, ENABLE); /*起始位*/
    /*读SR1*/
    SR1_Tmp = I2C_ReadRegister(I2Cx, I2C_Register_SR1);
    /*器件地址(写)*/
#ifdef AT24C01A

	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
#else

	I2C_Send7bitAddress(I2Cx, 0, I2C_Direction_Transmitter);
#endif

  }while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));
  
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
    
  I2C_GenerateSTOP(I2Cx, ENABLE);  /*停止位*/  
}


/*******************************************************************************
* Function Name  : I2C_Read
* Description    : 通过指定I2C接口读取多个字节数据
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
*                  - addr:预读取字节存储位置
*                  - *buf:读取数据的存储位置
*                  - num:读取字节数
* Output         : None
* Return         : 成功返回0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
    if(num==0)
	return 1;
	
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
		
	/*允许1字节1应答模式*/
	I2C_AcknowledgeConfig(I2Cx, ENABLE);


	/* 发送起始位 */
    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,主模式*/

#ifdef AT24C01A	
    /*发送器件地址(写)*/
    I2C_Send7bitAddress(I2Cx,  I2C_Addr, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/*发送地址*/
	I2C_SendData(I2Cx, addr);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));/*数据已发送*/
		
	/*起始位*/
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	
	/*器件读*/
	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		
#else	
	/*发送器件地址(读)24C01*/
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
#endif
	
    while (num)
    {
		if(num==1)
		{
     		I2C_AcknowledgeConfig(I2Cx, DISABLE);	/* 最后一位后要关闭应答的 */
    		I2C_GenerateSTOP(I2Cx, ENABLE);			/* 发送停止位 */
		}
	    
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */
	    *buf = I2C_ReceiveData(I2Cx);
	    buf++;
	    /* Decrement the read bytes counter */
	    num--;
    }
	/* 再次允许应答模式 */
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	return 0;
}	

/*******************************************************************************
* Function Name  : I2C_WriteOneByte
* Description    : 通过指定I2C接口写入一个字节数据
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
*                  - addr:预写入字节地址
*                  - value:写入数据
* Output         : None
* Return         : 成功返回0
* Attention		 : None
*******************************************************************************/
uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t value)
{
    /* 起始位 */
  	I2C_GenerateSTART(I2Cx, ENABLE);
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));  

#ifdef AT24C01A
  	/* 发送器件地址(写)*/
  	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);//发送模式
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  	/*发送地址*/
  	I2C_SendData(I2Cx, addr);//存储数据内容的地址
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#else	
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Transmitter);
 	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

	/* 写一个字节*/
  	I2C_SendData(I2Cx, value);//存储数据内容 
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
  	/* 停止位*/
  	I2C_GenerateSTOP(I2Cx, ENABLE);
  
  	I2C_AcknowledgePolling(I2Cx,I2C_Addr);//等待获取I2C总线的控制权

	I2C_delay(1000);

	return 0;
}


/*******************************************************************************
* Function Name  : I2C_Write
* Description    : 通过指定I2C接口写入多个字节数据
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
*                  - addr:预写入字节地址
*                  - *buf:预写入数据存储位置
*                  - num:写入字节数
* Output         : None
* Return         : 成功返回0
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
