//#include "stm32f1xx_hal.h"
#include "soft_iic.h"
#include <math.h>
//#include "stm32_bsp_time.h"
//#include "stm32f10x_gpio.h"
//
#include "delay.h"
#define I2C_SDA 				GPIO_Pin_11
#define I2C_SCL 				GPIO_Pin_10
#define I2C_PORT   		     	GPIOB

//

//
/*
#define I2C_SCL_0 				HAL_GPIO_WritePin(I2C_PORT, I2C_SCL,GPIO_PIN_RESET)
#define I2C_SCL_1 				HAL_GPIO_WritePin(I2C_PORT, I2C_SCL,GPIO_PIN_SET)
#define I2C_SDA_0 				HAL_GPIO_WritePin(I2C_PORT, I2C_SDA,GPIO_PIN_RESET)
#define I2C_SDA_1   			HAL_GPIO_WritePin(I2C_PORT, I2C_SDA,GPIO_PIN_SET)
*/
#define I2C_SCL_0 				GPIO_ResetBits(I2C_PORT , I2C_SCL)
#define I2C_SCL_1 				GPIO_SetBits(I2C_PORT , I2C_SCL)
#define I2C_SDA_0 				GPIO_ResetBits(I2C_PORT , I2C_SDA)
#define I2C_SDA_1   			GPIO_SetBits(I2C_PORT , I2C_SDA)





//
#define I2C_SDA_STATE   	GPIO_ReadInputDataBit(I2C_PORT, I2C_SDA)
#define I2C_DELAY 				I2C_Delay(100)
#define I2C_NOP						I2C_Delay(10) 		     //���Ĵ�NOP����ʱʱ�䣬���Ը���I2C���ݶ�ȡ�Ŀ���������Ҫ���Կɿ���

//
#define I2C_READY					0x00
#define I2C_BUS_BUSY			0x01	
#define I2C_BUS_ERROR			0x02
//
#define I2C_NACK	  			0x00 
#define I2C_ACK						0x01

float	f_rot_matrix_detection_array[9];
#define	USER_TIMER_INIT_VALUE	{USER_TIMER_STOP,0,0,0,0,0}


float	f_sensor_gyro_raw_data_array[3],	f_sensor_gyro_offset_data_array[3] = {1.00,0.70,0.30};
float	f_sensor_acc_raw_data_array[3],		f_sensor_acc_offset_data_array[3] = {-0.02,0,0.01};	
float	f_sensor_gyro_cal_data_array[3],	f_sensor_gyro_flt_data_array[3];
float	f_sensor_acc_cal_data_array[3],		f_sensor_acc_flt_data_array[3];
float	f_sensor_acc_scale_data_array[3]= {1,1,1};


float	f_quad_detection_array[4] = {1,0,0,0};


float	f_quat_x_right_array[4] = {1,0,0,0},	f_quat_x_front_array[4] = {1,0,0,0},	f_quat_array[4];
float	f_euler_x_right_array[3],							f_euler_x_front_array[3],							f_euler_array[3];

float	f_gimbal_gyro_data_array[3];		
float	f_gimbal_acc_data_array[3];

float	quad_kp = 0.2,		quad_ki = 0.004;

float invSqrt(float x) 
{
	float halfx = 0.5f * x;
	float y = x;
	long  i = *(long*)&y;
	
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

__weak void HAL_IncTick(void)
{
  uwTick++;
}
__weak uint32_t HAL_GetTick(void)
{
  return uwTick;
}


#define	DEG_TO_RADIAN		((float)0.01744)
#define	RADIAN_TO_DEG		((float)57.3)
#define	PI							((float)3.14)
	
FlagStatus	en_ahrs_init_flag = RESET;

//static u8  fac_us=0;							//us��ʱ������		

float	acc_correction_gain = 1,	acc_correction_gain_thr = 0.4;  //0.8-0.05g  0.2-0.2g 

void Tim4_Init(u16 period_num)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//ʱ������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	//��λ��ʱ��4����
	TIM_DeInit(TIM4);
	TIM_TimeBaseStructure.TIM_Period=period_num;//װ��ֵ
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;//��Ƶϵ��
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//count up
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	//�ж�����
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//clear the TIM4 overflow interrupt flag
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);
	//TIM4 overflow interrupt enable
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	//enable TIM4
	TIM_Cmd(TIM4,ENABLE);
}

void I2C_Delay(unsigned int dly) ;

void TIM3_Configuration(void) //ֻ��һ���ⲿ����˿�
	{
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
 	  //����TIMER2��Ϊ������
	TIM_DeInit(TIM3);
  	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // Time base configuration

//	TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);
	//TIM_SetCounter(TIM2, 0);
	TIM_Cmd(TIM3, ENABLE);
}
/*************************************************************************************
����ԭ�ͣ�void I2C_Configuration(void)
�������ܣ�����I2C��IO�ںͳ�ʼ��I2C
�����������
�����������
�汾��A1.0
**************************************************************************************/
void I2C_Configuration(void)			
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	
	GPIO_InitStructure.GPIO_Pin = I2C_SCL|I2C_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 // HAL_GPIO_Init(I2C_PORT, &GPIO_InitStructure); 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_InitStructure.Pin = I2C_SDA;
//	HAL_GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure); 
	
		
	I2C_SCL_1; 												    //��ʼ��I2C��ƽ
	I2C_SDA_1;
	I2C_DELAY;
}



/*************************************************************************************
����ԭ�ͣ�void I2C_Delay(unsigned int dly)
�������ܣ���ʱ����
���������unsigned int dly����ʾ��ʱ���ٸ�ʱ������
�����������
�汾��A1.0
**************************************************************************************/
void I2C_Delay(unsigned int dly)               
{


	while(--dly);	//dly=100: 8.75us; dly=100: 85.58 us (SYSCLK=72MHz)
}

///*************************************************************************************
//����ԭ�ͣ�unsigned char I2C_START(void)
//�������ܣ�ģ��I2C����ʱ��
//�����������
//�����������
//�汾��A1.0
//**************************************************************************************/
unsigned char I2C_START(void)
{ 
  I2C_SDA_1; 
 	I2C_NOP;
  // 
 	I2C_SCL_1; 
 	I2C_NOP;    
	//
 	if(!I2C_SDA_STATE) return I2C_BUS_BUSY;
	//
 	I2C_SDA_0;
 	I2C_NOP;
  //
 	I2C_SCL_0;  
 	I2C_NOP; 
	//
 	if(I2C_SDA_STATE) return I2C_BUS_ERROR;
	//
 	return I2C_READY;
}

///*************************************************************************************
//����ԭ�ͣ�void I2C_STOP(void)
//�������ܣ�ģ��I2C����ʱ��
//�����������
//�����������
//�汾��A1.0
//**************************************************************************************/
void I2C_STOP(void)
{
 	I2C_SDA_0; 
 	I2C_NOP;
  // 
 	I2C_SCL_1; 
 	I2C_NOP;    
	//
 	I2C_SDA_1;
 	I2C_NOP;
}

///*************************************************************************************
//����ԭ�ͣ�void I2C_SendACK(void)
//�������ܣ�ģ��I2C����Ӧ��(��������)
//�����������
//�����������
//�汾��A1.0
//**************************************************************************************/
void I2C_SendACK(void)
{
 	I2C_SDA_0;
 	I2C_NOP;
 	I2C_SCL_1;
 	I2C_NOP;
 	I2C_SCL_0; 
 	I2C_NOP;  
}

///*************************************************************************************
//����ԭ�ͣ�void I2C_SendNACK(void)
//�������ܣ�ģ��I2C������Ӧ��(��������)
//�����������
//�����������
//�汾��A1.0
//**************************************************************************************/
void I2C_SendNACK(void)
{
	I2C_SDA_1;
	I2C_NOP;
	I2C_SCL_1;
	I2C_NOP;
	I2C_SCL_0; 
	I2C_NOP;  
}

///*************************************************************************************
//����ԭ�ͣ�unsigned char I2C_SendByte(uint8_t i2c_data)
//�������ܣ�ģ��I2C����1byte����
//���������i2c_data��Ҫ���͵�����
//���������unsigned char���ӻ�����Ӧ��
//�汾��A1.0
//**************************************************************************************/
unsigned char I2C_SendByte(uint8_t i2c_data)
{
 	unsigned char i;
 	//
	I2C_SCL_0;
 	for(i=0;i<8;i++)
 	{  
  		if(i2c_data&0x80) I2C_SDA_1;
   		else I2C_SDA_0;
			//
  		i2c_data<<=1;
  		I2C_NOP;
			//
  		I2C_SCL_1;
  		I2C_NOP;
  		I2C_SCL_0;
  		I2C_NOP; 
 	}
	//
 	I2C_SDA_1; 
 	I2C_NOP;
 	I2C_SCL_1;
 	I2C_NOP;   
 	if(I2C_SDA_STATE)
 	{
  		I2C_SCL_0;
  		return I2C_NACK;
 	}
 	else
 	{
  		I2C_SCL_0;
  		return I2C_ACK;  
 	}    
}


///*************************************************************************************
//����ԭ�ͣ�unsigned char I2C_ReceiveByte_NoACK(void)
//�������ܣ�ģ��I2C����1byte���ݣ����Ҳ�����Ӧ���ź�
//�����������
//���������unsigned char�����յ�������
//�汾��A1.0
//**************************************************************************************/
unsigned char I2C_ReceiveByte_NoACK(void)
{
	unsigned char i,i2c_data;
	//
 	I2C_SDA_1;
 	I2C_SCL_0; 
 	i2c_data=0;
	//
 	for(i=0;i<8;i++)
 	{
  		I2C_SCL_1;
  		I2C_NOP; 
  		i2c_data<<=1;
			//
  		if(I2C_SDA_STATE)	i2c_data|=0x01; 
  
  		I2C_SCL_0;  
  		I2C_NOP;         
 	}
	I2C_SendNACK();
 	return i2c_data;
}


///*************************************************************************************
//����ԭ�ͣ�unsigned char I2C_ReceiveByte_WithACK(void)
//�������ܣ�ģ��I2C����1byte���ݣ����ҷ���Ӧ���ź�
//�����������
//���������unsigned char�����յ�������
//�汾��A1.0
//**************************************************************************************/
unsigned char I2C_ReceiveByte_WithACK(void)
{
	unsigned char i,i2c_data;
	//
 	I2C_SDA_1;
 	I2C_SCL_0; 
 	i2c_data=0;
	//
 	for(i=0;i<8;i++)
 	{
  		I2C_SCL_1;
  		I2C_NOP; 
  		i2c_data<<=1;
			
  		if(I2C_SDA_STATE)	i2c_data|=0x01; 
  
  		I2C_SCL_0;  
  		I2C_NOP;         
 	}
	I2C_SendACK();
 	return i2c_data;
}


////-------------------------------------------------Ӧ�ò����------------------------------------------------//
///*************************************************************************************
//����ԭ�ͣ�void I2C_Write_Single_Reg(uint8_t device_addr,uint8_t reg_addr,uint8_t reg_value)
//�������ܣ������豸�ĵ����Ĵ�����д������
//���������uint8_t device_addr�����豸��ַ
//		  uint8_t reg_addr��   ���豸�мĴ�����ַ
//		  uint8_t reg_value    Ҫд��Ĵ���������		   
//�����������
//�汾��A1.0
//**************************************************************************************/
void I2C_Write_Single_Reg(uint8_t device_addr,uint8_t reg_addr,uint8_t reg_value)
{
	I2C_START();
	I2C_SendByte(device_addr);				//Բ�㲩ʿ:����������д��ַ
	I2C_SendByte(reg_addr);  					//Բ�㲩ʿ:����������PWM��ַ
	I2C_SendByte(reg_value); 					//Բ�㲩ʿ:����������PWMֵ
	I2C_STOP();	
}


/*************************************************************************************
����ԭ�ͣ�void I2C_Write_Multi_Reg(uint8_t device_addr,uint8_t reg_addr,uint8_t *value_buf_addr, uint8_t reg_number)
�������ܣ������豸����������Ĵ�����д�����ݣ����豸���Զ������Ĵ�����ַ
���������uint8_t device_addr��		���豸��ַ
		  uint8_t reg_addr��   		���豸�мĴ�����ַ
		  uint8_t *value_buf_addr	Ҫд��Ĵ�����������ɵ�����
		  uint8_t reg_number    		Ҫд��ļĴ����ĸ���		   
�����������
�汾��A1.0
**************************************************************************************/
void I2C_Write_Multi_Reg(uint8_t device_addr,uint8_t reg_addr,uint8_t *value_buf_addr, uint8_t reg_number)
{
	uint8_t i = 0;
	I2C_START();
	I2C_SendByte(device_addr);				//Բ�㲩ʿ:����������д��ַ
	I2C_SendByte(reg_addr);  //Բ�㲩ʿ:����������PWM��ַ
	for(i=0;i<reg_number;i++)
	{
		I2C_SendByte( (*(value_buf_addr++)) ); //Բ�㲩ʿ:����������PWMֵ
	}	
	I2C_STOP();	
}


/*************************************************************************************
����ԭ�ͣ�uint8_t I2C_Read_Single_Reg(uint8_t device_addr,uint8_t reg_addr) 
�������ܣ���ȡ���豸�ĵ����Ĵ���ֵ
���������uint8_t device_addr��		���豸��ַ
		  uint8_t reg_addr��   		���豸�мĴ�����ַ		   
���������uint8_t�����豸�Ĵ���ֵ
�汾��A1.0
**************************************************************************************/
uint8_t I2C_Read_Single_Reg(uint8_t device_addr,uint8_t reg_addr)   
{
	uint8_t reg_value;
	
	I2C_START();
	I2C_SendByte(device_addr);					//Բ�㲩ʿ:����������д��ַ
	I2C_SendByte(reg_addr);    				//Բ�㲩ʿ:���������ǼĴ�����ַ
	I2C_START();
	I2C_SendByte(device_addr+1);      			//Բ�㲩ʿ:���������Ƕ���ַ
	reg_value=I2C_ReceiveByte_NoACK();			//Բ�㲩ʿ:���������ǼĴ���ֵ
	I2C_STOP();
	//
	return reg_value;
}


/*************************************************************************************
����ԭ�ͣ�void I2C_Read_Multi_Reg(uint8_t device_addr,uint8_t reg_addr, uint8_t *value_buf_addr, uint8_t reg_number) 
�������ܣ���ȡ���豸����������Ĵ���ֵ
���������uint8_t device_addr��		���豸��ַ
					uint8_t reg_addr��   		���豸�мĴ�����ʼ��ַ
					uint8_t *value_buf_addr    �����洢�Ĵ���ֵ�������׵�ַ
					uint8_t reg_number         Ҫ��ȡ�ļĴ����ĸ���		   
���������uint8_t�����豸�Ĵ���ֵ
�汾��A1.0
**************************************************************************************/

uint16_t	uw_i2c_bus_err_cnt = 0;  		//test
void I2C_Read_Multi_Reg(uint8_t device_addr,uint8_t reg_addr, uint8_t *value_buf_addr, uint8_t reg_number)   
{
	uint8_t i = 0;
	uint8_t		uc_device_ack_data = 0xFF,	uc_reg_ack_data = 0xFF;    //test
	
	
//	do
//	{
		I2C_START();
		uc_device_ack_data = I2C_SendByte(device_addr);					//Բ�㲩ʿ:����������д��ַ
		uc_reg_ack_data = I2C_SendByte(reg_addr);    						//Բ�㲩ʿ:���������ǼĴ�����ַ
//	}while( (!uc_device_ack_data)||(!uc_reg_ack_data) );    //test
	
//	I2C_START();
//	do
//	{
		I2C_START();
		uc_reg_ack_data = I2C_SendByte(device_addr+1);      			//Բ�㲩ʿ:���������Ƕ���ַ
//	}while(!uc_reg_ack_data);
	
	
	for(i=0;i<reg_number-1;i++)
	{
		value_buf_addr[i] = I2C_ReceiveByte_WithACK();	
	}
	value_buf_addr[reg_number-1] = I2C_ReceiveByte_NoACK();
	I2C_STOP();
}




/*--------------------------------------------------ģ��I2C��ȡIcm20602����--------------------------------------*/
/**
  * @brief 	init Icm20602		
  */
uint8_t	uc_icm20602_reg_value_fdb_p; 
//u8 uc_mma8452_id=0;
void	Init_Icm20602_Soft(void)
{
		uint8_t	uc_icm20602_reg_config_value_p;
	
	
	uc_icm20602_reg_value_fdb_p = I2C_Read_Single_Reg(ICM20602_DEVICE_ADDRESS,	ICM20602_ID_REG_ADDRESS);
	
	/**
		* ICM20602_PWR_MGMT_1_REG_ADDR
		* bit7-RST			1-trig device reset  	0-no action
		* bit6-SLEEP		1-SLEEP								0-NORMAL
		* bit5-CYCLE		1-ENABLE							0-DISABLE ���ڲ���ACC
		* bit4-GYRO_STANDBY	 	1-GYRO_SLEEP  	0-GYRO_NROMAL
		* bit3-TMEP_DIS	1-DISBALE TEMP				0-ENABLE_TEMP
		* bit2:0-CLKSEL[2:0]	0or6-internal 20M  1~5-auto clk 7-no clk
		*/
	uc_icm20602_reg_config_value_p = 0x80;   //standby
	I2C_Write_Single_Reg(ICM20602_DEVICE_ADDRESS,	ICM20602_PWR_MGMT_1_REG_ADDR, uc_icm20602_reg_config_value_p);
	/*wait ICM20602 RESET completed*/
	while(I2C_Read_Single_Reg(ICM20602_DEVICE_ADDRESS,	ICM20602_PWR_MGMT_1_REG_ADDR)&0x80);
//	HAL_Delay(100);
	
	/*auto clk to achieve full gyroscope performance*/
	uc_icm20602_reg_config_value_p = 0x01;   
	I2C_Write_Single_Reg(ICM20602_DEVICE_ADDRESS,	ICM20602_PWR_MGMT_1_REG_ADDR, uc_icm20602_reg_config_value_p);

	/**
		* ICM20602_CFG_REG_ADDR
		* bit6-FIFO MODE					1-FIFO���˲�����д		0-FIFO�����滻
		* bit5:3-EXT_SYNC_SET			1��7-SYNC SOURCE			0-DISABLE 
		* bit2:0-DLPF_CFG[2:0]		DLPF����Ƶ������
		*/
	uc_icm20602_reg_config_value_p = 0x00;   
	I2C_Write_Single_Reg(ICM20602_DEVICE_ADDRESS,	ICM20602_CFG_REG_ADDR,	uc_icm20602_reg_config_value_p);
	
	/**
		* ICM20602_GYRO_CFG_REG_ADDR
		* bit7-XG_ST							X_GYRO self-test
		* bit6-YG_ST							Y_GYRO self-test
		* bit5-ZG_ST							Z_GYRO self-test
		* bit4:3-FS_SEL						00-��250dps	01-��500dps	10-��1000dps  11-��2000dps
		* bit1:0-FCHOICE_B				1��3-DLPF DISBALE      	0-DLPF ENABLE
		*/
	uc_icm20602_reg_config_value_p = 0x18;   
	I2C_Write_Single_Reg(ICM20602_DEVICE_ADDRESS,	ICM20602_GYRO_CFG_REG_ADDR,	uc_icm20602_reg_config_value_p);
	
	uc_icm20602_reg_value_fdb_p = I2C_Read_Single_Reg(ICM20602_DEVICE_ADDRESS,	ICM20602_GYRO_CFG_REG_ADDR);
	/*�������ݵķֱ���*/
	f_icm20602_gyro_scale = (1/131.f)*(0x01<<( (uc_icm20602_reg_config_value_p&0x18)>>3 ));
	
	/**
		* ICM20602_ACC_CFG_REG_ADDR
		* bit7-XA_ST							X_ACC self-test
		* bit6-YA_ST							Y_ACC self-test
		* bit5-ZA_ST							Z_ACC self-test
		* bit4:3-FS_SEL						00-��2g	01-��4g	10-��8g  11-��16g
		*/
	uc_icm20602_reg_config_value_p = 0x00;   
	I2C_Write_Single_Reg(ICM20602_DEVICE_ADDRESS,	ICM20602_ACC_CFG_REG_ADDR,	uc_icm20602_reg_config_value_p);
	
	/*�������ݵķֱ���*/
	f_icm20602_acc_scale = (1/16384.f)*(0x01<<( (uc_icm20602_reg_config_value_p&0x18)>>3 ));
}






/**
  * @brief 	get icm20602 data
	* @note	 	��Ҫ������λ�÷���icm20602
	*					���ٶ�����Ϊfloat���ͣ��洢��float *	p_f_icm20602_gyro_data_p (float	icm20602_gyro[3])
  */
uint8_t		uc_icm20602_data_p[14];  
float			f_icm20602_temp = 0;
void	Read_Icm20602_Data_Soft(float *	p_f_icm20602_gyro_data_p,	float *p_f_icm20602_acc_data_p)
{
	static	FlagStatus	st_first_read_icm20602_reg_data_flag_p = SET;
	
	
	
	if(SET == st_first_read_icm20602_reg_data_flag_p)
	{
		/*��ȡһ�νǶȼĴ�����ֵ��ʹauto_increment pointer������angle_reg,��֤������ȡ����Ҫ�����ڲ��Ĵ�����ַ*/
		I2C_Read_Multi_Reg(ICM20602_DEVICE_ADDRESS,	ICM20602_ACC_XOUT_H_REG_ADDR,	uc_icm20602_data_p,	14);	
		st_first_read_icm20602_reg_data_flag_p = RESET;
	}
	else
	{
		I2C_Read_Multi_Reg(ICM20602_DEVICE_ADDRESS,	ICM20602_ACC_XOUT_H_REG_ADDR,	uc_icm20602_data_p,	14);	
	}
	
	p_f_icm20602_acc_data_p[0] = -(float)( ( (int16_t)(int8_t)uc_icm20602_data_p[0] <<8 )|uc_icm20602_data_p[1] )*f_icm20602_acc_scale;
 	p_f_icm20602_acc_data_p[1] = (float)( ( (int16_t)(int8_t)uc_icm20602_data_p[2] <<8 )|uc_icm20602_data_p[3] )*f_icm20602_acc_scale;
	p_f_icm20602_acc_data_p[2] = -(float)( ( (int16_t)(int8_t)uc_icm20602_data_p[4] <<8 )|uc_icm20602_data_p[5] )*f_icm20602_acc_scale;	
	
	p_f_icm20602_gyro_data_p[0] = -(float)( ( (int16_t)(int8_t)uc_icm20602_data_p[8] <<8 )|uc_icm20602_data_p[9] )*f_icm20602_gyro_scale;
	p_f_icm20602_gyro_data_p[1] = (float)( ( (int16_t)(int8_t)uc_icm20602_data_p[10] <<8 )|uc_icm20602_data_p[11] )*f_icm20602_gyro_scale;
	p_f_icm20602_gyro_data_p[2] = -(float)( ( (int16_t)(int8_t)uc_icm20602_data_p[12] <<8 )|uc_icm20602_data_p[13] )*f_icm20602_gyro_scale;	

	f_icm20602_temp = (float)( ( (int16_t)(int8_t)uc_icm20602_data_p[6] <<8 )|uc_icm20602_data_p[7] )/326.8 + 25;
}


/**
  * @brief 	void	Gyro_Data_Fliter(float* p_f_gyro_in_p,	float* p_f_gyro_out_p,	FlagStatus	*en_first_run_flag_p)
	* @note	 	���������˲�
	* @param 	float* p_f_gyro_in_p��	ԭʼ����
	* @param 	float* p_f_gyro_out_p��	�˲�����
  */
#define		GYRO_FLT_BUF_LENGTH  ( (uint8_t)1 ) 
void	Gyro_Data_Fliter(float* p_f_gyro_in_p,	float* p_f_gyro_out_p,	FlagStatus	*en_first_run_flag_p)
{
	static	float			f_gyro_flt_buf_p[3][GYRO_FLT_BUF_LENGTH];
	static  uint8_t		gyro_flt_buf_index = 0;
	float							f_gyro_sum_p[3];
	uint8_t   i;
	/*��GYRO_FLT_BUF_LENGTH = 1ʱ���Ƿ���ģʽ�»����,ԭ��δ֪*/ 
	if(GYRO_FLT_BUF_LENGTH <= 1)
	{
		p_f_gyro_out_p[0] = p_f_gyro_in_p[0];
		p_f_gyro_out_p[1] = p_f_gyro_in_p[1];
		p_f_gyro_out_p[2] = p_f_gyro_in_p[2];
	}
	else
	{ 
		/*��¼���GYRO_FLT_BUF_LENGTH����������*/	
		f_gyro_flt_buf_p[0][gyro_flt_buf_index] = p_f_gyro_in_p[0];
		f_gyro_flt_buf_p[1][gyro_flt_buf_index] = p_f_gyro_in_p[1];
		f_gyro_flt_buf_p[2][gyro_flt_buf_index] = p_f_gyro_in_p[2];
		
		gyro_flt_buf_index++;
		gyro_flt_buf_index = gyro_flt_buf_index%GYRO_FLT_BUF_LENGTH;
		
		/*�ж��˲������Ƿ�>GYRO_FLT_BUF_LENGTH*/
		if(gyro_flt_buf_index >= (GYRO_FLT_BUF_LENGTH -1) )
		{
			*en_first_run_flag_p = RESET;
		}

		/*��һ���˲����ڣ����ݲ��㣬����õ�������˲�����*/
		if(SET == *en_first_run_flag_p)
		{
			p_f_gyro_out_p[0] = p_f_gyro_in_p[0];
			p_f_gyro_out_p[1] = p_f_gyro_in_p[1];
			p_f_gyro_out_p[2] = p_f_gyro_in_p[2];	
		}
		else
		{
			/*����˲����ֵ*/
			for( i=0;i<GYRO_FLT_BUF_LENGTH;i++)
			{
				f_gyro_sum_p[0] += f_gyro_flt_buf_p[0][i];
				f_gyro_sum_p[1] += f_gyro_flt_buf_p[1][i];
				f_gyro_sum_p[2] += f_gyro_flt_buf_p[2][i];
			}
			
			p_f_gyro_out_p[0] = f_gyro_sum_p[0]/GYRO_FLT_BUF_LENGTH;
			p_f_gyro_out_p[1] = f_gyro_sum_p[1]/GYRO_FLT_BUF_LENGTH;
			p_f_gyro_out_p[2] = f_gyro_sum_p[2]/GYRO_FLT_BUF_LENGTH;
		}
	}

}

void	Acc_Data_Fliter(float* p_f_acc_in_p,	float* p_f_acc_out_p,	FlagStatus *en_first_run_flag_p)
{
	/*filter*/
	if(SET == *en_first_run_flag_p)
	{
		p_f_acc_out_p[0] = p_f_acc_in_p[0];
		p_f_acc_out_p[1] = p_f_acc_in_p[1];
		p_f_acc_out_p[2] = p_f_acc_in_p[2];
		
		*en_first_run_flag_p = RESET;
	}
	else
	{
		p_f_acc_out_p[0] = 0.99*p_f_acc_out_p[0] + 0.01*p_f_acc_in_p[0];
		p_f_acc_out_p[1] = 0.99*p_f_acc_out_p[1] + 0.01*p_f_acc_in_p[1];
		p_f_acc_out_p[2] = 0.99*p_f_acc_out_p[2] + 0.01*p_f_acc_in_p[2];
	}
}


void	Cal_Filter_Sensor_Data(void)
{
	static	FlagStatus	en_gyro_data_first_flt_flag = SET,	en_acc_data_first_flt_flag = SET;
	
	/*����Ư��*/
	f_sensor_gyro_cal_data_array[0] = f_sensor_gyro_raw_data_array[0] + f_sensor_gyro_offset_data_array[0];
	f_sensor_gyro_cal_data_array[1] = f_sensor_gyro_raw_data_array[1] + f_sensor_gyro_offset_data_array[1];
	f_sensor_gyro_cal_data_array[2] = f_sensor_gyro_raw_data_array[2] + f_sensor_gyro_offset_data_array[2];
	
	/*�������ٶ�ƫ��*/
	f_sensor_acc_cal_data_array[0] = f_sensor_acc_raw_data_array[0] + f_sensor_acc_offset_data_array[0];
	f_sensor_acc_cal_data_array[1] = f_sensor_acc_raw_data_array[1] + f_sensor_acc_offset_data_array[1];
	f_sensor_acc_cal_data_array[2] = f_sensor_acc_raw_data_array[2] + f_sensor_acc_offset_data_array[2];
	
	/*�������ٶ�����*/
	f_sensor_acc_cal_data_array[0] *= f_sensor_acc_scale_data_array[0];
	f_sensor_acc_cal_data_array[1] *= f_sensor_acc_scale_data_array[1];
	f_sensor_acc_cal_data_array[2] *= f_sensor_acc_scale_data_array[2];
	
	Gyro_Data_Fliter(f_sensor_gyro_cal_data_array,	f_sensor_gyro_flt_data_array,	&en_gyro_data_first_flt_flag);  //�ٶ��˲�
	Acc_Data_Fliter(f_sensor_acc_cal_data_array,	f_sensor_acc_flt_data_array,	&en_acc_data_first_flt_flag);			//���ٶ��˲�
}

//
//
/**
  * @brief 	void	Task_Get_Imu_Sensor_Data(void)
	* @note	 	��ȡIMU����������
  */
void	Task_Get_Imu_Sensor_Data(void)
{
	//float	f_icm20602_gyro_data_array_p[3], 	f_icm20602_acc_data_array_p[3];
	//float	f_mma8452_acc_data_array_p[3];
	
//	Read_Mma8452_Acc_Data(&ACC_I2C_HANDLE,f_sensor_acc_raw_data_array);
	//Read_Icm20602_Data(&GYRO_I2C_HANDLE,f_sensor_gyro_raw_data_array,f_sensor_acc_raw_data_array);	
	
	Read_Icm20602_Data_Soft(f_sensor_gyro_raw_data_array,f_sensor_acc_raw_data_array);	//f_icm20602_acc_data_array_p); //�������������ݲɼ�
	//Read_Mma8452_Acc_Data_Soft(f_sensor_acc_raw_data_array);        									 //������ٶ����ݲɼ�
	
	Cal_Filter_Sensor_Data();																				//���ڻ����˲�
}
//
//
//
//
//
//

void	BondFloat_Ahrs(float *data,	float data_min,	float data_max)
{
	if(*data < data_min)
	{
		*data = data_min;
	}
	else if(*data > data_max)
	{
		*data = data_max;
	}
}



/**
  * @brief 	void Ahrs_Update_X_Right(	float gx,	float gy,	float gz,	float ax,	float ay,	float az,	float	dt)																			
	* @note	 	X_RIGHT����ϵ��Ԫ������
	* @param 	float gx gy gz����������
	* @param 	float ax ay az�����ٶ�����
	* @param 	float	dt��	����ʱ��
  */
float 	ex_int_x_right = 0,	ey_int_x_right =0 ,	ez_int_x_right=0; 		//test
float		err_int_x_right[3],	err_int_x_right_flt[3];										//test
FlagStatus		err_int_flt_flag = RESET;																//test
uint8_t				uc_err_int_flt_index = 0,	uc_err_int_flt_num = 200;			//test

void Ahrs_Update_X_Right(	float gx,	float gy,	float gz,	float ax,	float ay,	float az,	float	dt)
{	
	float norm;
	float vx, vy, vz;
	float ex, ey, ez; 	
	
	float q0q0,q0q1,q0q2,q0q3,q1q1,q1q2,q1q3,q2q2,q2q3,q3q3;
	
	float		ex_kp = 0,	ey_kp = 0,	ez_kp = 0;
	static	float 	ex_int = 0,	ey_int =0 ,	ez_int=0;
	
	q0q0 = f_quat_x_right_array[0] * f_quat_x_right_array[0];								
	q0q1 = f_quat_x_right_array[0] * f_quat_x_right_array[1];
	q0q2 = f_quat_x_right_array[0] * f_quat_x_right_array[2];
	q0q3 = f_quat_x_right_array[0] * f_quat_x_right_array[3];

	q1q1 = f_quat_x_right_array[1] * f_quat_x_right_array[1];
	q1q2 = f_quat_x_right_array[1] * f_quat_x_right_array[2];
	q1q3 = f_quat_x_right_array[1] * f_quat_x_right_array[3];
	
	q2q2 = f_quat_x_right_array[2] * f_quat_x_right_array[2];
	q2q3 = f_quat_x_right_array[2] * f_quat_x_right_array[3];

	q3q3 = f_quat_x_right_array[3] * f_quat_x_right_array[3];	
	
	/*���ٶȹ�һ��*/
	norm = invSqrt(ax*ax + ay* ay +  az*az);  
	ax = ax * norm;
	ay = ay * norm;
	az = az * norm; 
	
	/*g�ڻ�������ϵ�������ֵ*/
	vx = 2.f * (q1q3 - q0q2);												//????xyz???
  vy = 2.f * (q2q3 + q0q1);	
  vz = q0q0 - q1q1 - q2q2 + q3q3;
	
	//���ٶ�������,only acc
	ex = (ay*vz - az*vy);
	ey = (az*vx - ax*vz);
	ez = (ax*vy - ay*vx);
	
	ex_kp = acc_correction_gain*quad_kp*ex;
	ey_kp = acc_correction_gain*quad_kp*ey;
	ez_kp = acc_correction_gain*quad_kp*ez;
	
	if(acc_correction_gain > acc_correction_gain_thr)    //���ٶ�����ʱ�żӻ���ֵ�����򽫻���ֵ����
	{
		ex_int += acc_correction_gain*acc_correction_gain*quad_ki*ex*dt;
		ey_int += acc_correction_gain*acc_correction_gain*quad_ki*ey*dt;
		ez_int += acc_correction_gain*acc_correction_gain*quad_ki*ez*dt;
		
	}
	
	gx += (ex_kp + ex_int);
	gy += (ey_kp + ey_int);
	gz += (ez_kp + ez_int);
	
	/*test
	ex_int_x_right = ex_kp + ex_int;
	ey_int_x_right = ey_kp + ey_int;
	ez_int_x_right = ez_kp + ez_int;

	err_int_x_right[0] = ex_int_x_right;
	err_int_x_right[1] = ey_int_x_right;
	err_int_x_right[2] = ez_int_x_right;
	
	if(SET == err_int_flt_flag)
	{
		Average_Filter_Float(err_int_x_right,	err_int_x_right_flt,	&uc_err_int_flt_index,	uc_err_int_flt_num);
	}*/
	
	//��Ԫ������
	f_quat_x_right_array[0] = f_quat_x_right_array[0] + (-f_quat_x_right_array[1]*gx - f_quat_x_right_array[2]*gy - f_quat_x_right_array[3]*gz)*dt*0.5;
	f_quat_x_right_array[1] = f_quat_x_right_array[1] + ( f_quat_x_right_array[0]*gx + f_quat_x_right_array[2]*gz - f_quat_x_right_array[3]*gy)*dt*0.5;
	f_quat_x_right_array[2] = f_quat_x_right_array[2] + ( f_quat_x_right_array[0]*gy - f_quat_x_right_array[1]*gz + f_quat_x_right_array[3]*gx)*dt*0.5;
	f_quat_x_right_array[3] = f_quat_x_right_array[3] + ( f_quat_x_right_array[0]*gz + f_quat_x_right_array[1]*gy - f_quat_x_right_array[2]*gx)*dt*0.5;  	
	
	//��Ԫ����һ������
	norm = invSqrt(	f_quat_x_right_array[0]*f_quat_x_right_array[0] + f_quat_x_right_array[1]*f_quat_x_right_array[1] + 
									f_quat_x_right_array[2]*f_quat_x_right_array[2] + f_quat_x_right_array[3]*f_quat_x_right_array[3]);	
	
	f_quat_x_right_array[0] *= norm;
	f_quat_x_right_array[1] *= norm;
	f_quat_x_right_array[2] *= norm;
	f_quat_x_right_array[3] *= norm;
	
	f_euler_x_right_array[1] = -RADIAN_TO_DEG*asinf( 2.f * (q1q3 - q0q2) );
	f_euler_x_right_array[0] = RADIAN_TO_DEG*atan2f( 2.f * (q2q3 + q0q1), q0q0 - q1q1 - q2q2 + q3q3 );
	f_euler_x_right_array[2] = RADIAN_TO_DEG*atan2f( 2.f * (q1q2 + q0q3),	q0q0 + q1q1 - q2q2 - q3q3);
}



void Ahrs_Update_Detection(	float gx,	float gy,	float gz,	float ax,	float ay,	float az,	float	dt)
{	
	float norm;
	float vx, vy, vz;
	float ex, ey, ez; 	
	
	float q0q0,q0q1,q0q2,q0q3,q1q1,q1q2,q1q3,q2q2,q2q3,q3q3;
	
	float		ex_kp = 0,	ey_kp = 0,	ez_kp = 0;
	
	q0q0 = f_quad_detection_array[0] * f_quad_detection_array[0];								
	q0q1 = f_quad_detection_array[0] * f_quad_detection_array[1];
	q0q2 = f_quad_detection_array[0] * f_quad_detection_array[2];
	q0q3 = f_quad_detection_array[0] * f_quad_detection_array[3];

	q1q1 = f_quad_detection_array[1] * f_quad_detection_array[1];
	q1q2 = f_quad_detection_array[1] * f_quad_detection_array[2];
	q1q3 = f_quad_detection_array[1] * f_quad_detection_array[3];
	
	q2q2 = f_quad_detection_array[2] * f_quad_detection_array[2];
	q2q3 = f_quad_detection_array[2] * f_quad_detection_array[3];

	q3q3 = f_quad_detection_array[3] * f_quad_detection_array[3];

	f_rot_matrix_detection_array[0] = q0q0 + q1q1 - q2q2 - q3q3;		// 11
	f_rot_matrix_detection_array[1] = 2.f * (q1q2 + q0q3);			// 12
	f_rot_matrix_detection_array[2] = 2.f * (q1q3 - q0q2);			// 13
	f_rot_matrix_detection_array[3] = 2.f * (q1q2 - q0q3);			// 21
	f_rot_matrix_detection_array[4] = q0q0 - q1q1 + q2q2 - q3q3;		// 22
	f_rot_matrix_detection_array[5] = 2.f * (q2q3 + q0q1);			// 23
	f_rot_matrix_detection_array[6] = 2.f * (q1q3 + q0q2);			// 31
	f_rot_matrix_detection_array[7] = 2.f * (q2q3 - q0q1);			// 32
	f_rot_matrix_detection_array[8] = q0q0 - q1q1 - q2q2 + q3q3;		// 33	
	
	/*���ٶȹ�һ��*/
	norm = invSqrt(ax*ax + ay* ay +  az*az);  
	ax = ax * norm;
	ay = ay * norm;
	az = az * norm; 
	
	/*g�ڻ�������ϵ�������ֵ*/
	vx = 2.f * (q1q3 - q0q2);												//????xyz???
  vy = 2.f * (q2q3 + q0q1);	
  vz = q0q0 - q1q1 - q2q2 + q3q3;
	
	//���ٶ�������,only acc
	ex = (ay*vz - az*vy) ;
	ey = (az*vx - ax*vz) ;
	ez = (ax*vy - ay*vx) ;
	
	ex_kp = 1*ex;
	ey_kp = 1*ey;
	ez_kp = 1*ez;
	

	gx += ex_kp ;
	gy += ey_kp ;
	gz += ez_kp ;
	
	//��Ԫ������
	f_quad_detection_array[0] = f_quad_detection_array[0] + (-f_quad_detection_array[1]*gx - f_quad_detection_array[2]*gy - f_quad_detection_array[3]*gz)*dt*0.5;
	f_quad_detection_array[1] = f_quad_detection_array[1] + ( f_quad_detection_array[0]*gx + f_quad_detection_array[2]*gz - f_quad_detection_array[3]*gy)*dt*0.5;
	f_quad_detection_array[2] = f_quad_detection_array[2] + ( f_quad_detection_array[0]*gy - f_quad_detection_array[1]*gz + f_quad_detection_array[3]*gx)*dt*0.5;
	f_quad_detection_array[3] = f_quad_detection_array[3] + ( f_quad_detection_array[0]*gz + f_quad_detection_array[1]*gy - f_quad_detection_array[2]*gx)*dt*0.5;  	

	//��Ԫ����һ������
	norm = invSqrt(	f_quad_detection_array[0]*f_quad_detection_array[0] + f_quad_detection_array[1]*f_quad_detection_array[1] + 
									f_quad_detection_array[2]*f_quad_detection_array[2] + f_quad_detection_array[3]*f_quad_detection_array[3]);	
	
	f_quad_detection_array[0] *= norm;
	f_quad_detection_array[1] *= norm;
	f_quad_detection_array[2] *= norm;
	f_quad_detection_array[3] *= norm;
}




uint32_t ul_gyro_err_cnt = 0, i=0;    	//test
void	Gimbal_Ahrs_Update(float	dt)
{
	float	gx,gy,gz,ax,ay,az;
	
	float	gravity_x,		gravity_y,	gravity_z;
	float gravity_hor,	gravity_ver;
	float acc_correction_gain_hor,	acc_correction_gain_ver;
	
	f_gimbal_gyro_data_array[0] = f_sensor_gyro_flt_data_array[GIMBAL_GYRO_X_DATA_INDEX];
	f_gimbal_gyro_data_array[1] = f_sensor_gyro_flt_data_array[GIMBAL_GYRO_Y_DATA_INDEX];
	f_gimbal_gyro_data_array[2] = f_sensor_gyro_flt_data_array[GIMBAL_GYRO_Z_DATA_INDEX];
	
	f_gimbal_acc_data_array[0] = f_sensor_acc_flt_data_array[GIMBAL_ACC_X_DATA_INDEX];
	f_gimbal_acc_data_array[1] = f_sensor_acc_flt_data_array[GIMBAL_ACC_Y_DATA_INDEX];
	f_gimbal_acc_data_array[2] = f_sensor_acc_flt_data_array[GIMBAL_ACC_Z_DATA_INDEX];
	
	
	/*test*/  /*�����ֵ*/
	if( (fabs(f_gimbal_gyro_data_array[0]) > 100)||
			(fabs(f_gimbal_gyro_data_array[1]) > 100)||
			(fabs(f_gimbal_gyro_data_array[2]) > 100)
		)
	{		
		ul_gyro_err_cnt++;
		i++;
	}
	
	gx = f_gimbal_gyro_data_array[0]*DEG_TO_RADIAN;
	gy = f_gimbal_gyro_data_array[1]*DEG_TO_RADIAN;
	gz = f_gimbal_gyro_data_array[2]*DEG_TO_RADIAN;
	
	ax = f_gimbal_acc_data_array[0];
	ay = f_gimbal_acc_data_array[1];
	az = f_gimbal_acc_data_array[2];
	
	if(RESET == en_ahrs_init_flag)
	{
		quad_kp = 5; 
		quad_ki = 0.04;
//		quad_ki = 0;
	}
	else
	{
		quad_kp = 0.5;
		quad_ki = 0.004;
	
		/*�����˶����ٶ� �޸ļ��ٶȱ���*/
		Ahrs_Update_Detection(gx, gy, gz,	ax,  ay, az, dt);
		
		gravity_x = ax*f_rot_matrix_detection_array[0] + ay*f_rot_matrix_detection_array[3] + az*f_rot_matrix_detection_array[6];
		gravity_y = ax*f_rot_matrix_detection_array[1] + ay*f_rot_matrix_detection_array[4] + az*f_rot_matrix_detection_array[7];
		gravity_z = ax*f_rot_matrix_detection_array[2] + ay*f_rot_matrix_detection_array[5] + az*f_rot_matrix_detection_array[8];
		
		gravity_hor	=	sqrt(gravity_x*gravity_x + gravity_y*gravity_y);
		gravity_ver = fabs( fabs(gravity_z) - 1 );
			
		acc_correction_gain_hor = 1.0 - 4*gravity_hor;  //0.4g=0.8->gravity_gain_hor = 0.2
		acc_correction_gain_ver = 1.0 - 4*gravity_ver;
		
		BondFloat_Ahrs(&acc_correction_gain_hor,0.1,1);
		BondFloat_Ahrs(&acc_correction_gain_ver,0.1,1);
						
		acc_correction_gain = acc_correction_gain_hor<acc_correction_gain_ver?acc_correction_gain_hor:acc_correction_gain_ver;
	}
	
	/*����X_Right����ϵ��X_Front����ϵ����̬*/
	Ahrs_Update_X_Right(gx,   gy, gz,	ax,  ay, az, dt);
	
	//Ahrs_Update_X_Front(gy,  -gx, gz,	ay, -ax, az, dt);
}


/*---------------����ʱ�亯��-----------------*/
/**
  * @brief 	float Get_Function_Period(uint32_t	*ul_last_sys_micros)
	* @note	 	���㺯��ִ������(us)
  */
float Get_Function_Period(uint32_t	*ul_last_sys_micros)
{
	uint32_t	ul_now_sys_micros;
	float			period;
	
	ul_now_sys_micros = TIM_GetCounter(TIM3);
	
	if( ul_now_sys_micros < (*ul_last_sys_micros) )
	{ 
		period =  ((float)(ul_now_sys_micros + (0xffff - (*ul_last_sys_micros) ) ) / 1000000.0f);
	}
	else	
	{
		period =  ((float)(ul_now_sys_micros - (*ul_last_sys_micros) ) / 1000000.0f);
	}
	*ul_last_sys_micros = ul_now_sys_micros;	
	
	return(period);
}
/**
  * @brief 	void	Update_User_Timer_Cnt(User_Timer_Typedef *st_user_timer_p)
	* @note	 	�����û�TIMER(���)��CNT
  */
void	Update_User_Timer_Cnt(User_Timer_Typedef *st_user_timer_p)
{
	st_user_timer_p->ul_timer_once_cnt = 0;
	
	if(USER_TIMER_RUN == st_user_timer_p->en_timer_state)
	{
		st_user_timer_p->ul_timer_now_cnt = HAL_GetTick();
		if(st_user_timer_p->ul_timer_now_cnt == st_user_timer_p->ul_timer_start_cnt)
		{
			st_user_timer_p->ul_timer_once_cnt = 0;
		}
		else if(st_user_timer_p->ul_timer_now_cnt > st_user_timer_p->ul_timer_start_cnt)
		{
			st_user_timer_p->ul_timer_once_cnt = st_user_timer_p->ul_timer_now_cnt - st_user_timer_p->ul_timer_start_cnt;
		}
		else 
		{
			st_user_timer_p->ul_timer_once_cnt = st_user_timer_p->ul_timer_now_cnt + 0xFFFFFFFF - st_user_timer_p->ul_timer_start_cnt;
		}
		
		st_user_timer_p->ul_timer_cnt = st_user_timer_p->ul_timer_pause_cnt + st_user_timer_p->ul_timer_once_cnt;
	}
	else if(USER_TIMER_PAUSE == st_user_timer_p->en_timer_state)
	{
		st_user_timer_p->ul_timer_cnt = st_user_timer_p->ul_timer_pause_cnt;
	}
	else
	{
		st_user_timer_p->ul_timer_cnt = 0;
	}
}

/**
  * @brief 	void	Update_User_Timer_Cnt(User_Timer_Typedef *st_user_timer_p)
	* @note	 	�����û�TIMER(���)
  */
void	Start_User_Timer(User_Timer_Typedef *st_user_timer_p)
{
	if(USER_TIMER_STOP == st_user_timer_p->en_timer_state)
	{
		st_user_timer_p->en_timer_state = USER_TIMER_RUN;
		
		st_user_timer_p->ul_timer_start_cnt = HAL_GetTick();
		st_user_timer_p->ul_timer_now_cnt = HAL_GetTick();
		st_user_timer_p->ul_timer_pause_cnt = 0;
		Update_User_Timer_Cnt(st_user_timer_p);
	}
}
/**
  * @brief 	void	Stop_User_Timer(User_Timer_Typedef *st_user_timer_p)
	* @note	 	ֹͣ�û�TIMER(���)
  */
void	Stop_User_Timer(User_Timer_Typedef *st_user_timer_p)
{
	if(USER_TIMER_STOP != st_user_timer_p->en_timer_state)
	{
		st_user_timer_p->en_timer_state = USER_TIMER_STOP;
		st_user_timer_p->ul_timer_start_cnt = HAL_GetTick();
		st_user_timer_p->ul_timer_now_cnt = HAL_GetTick();
		st_user_timer_p->ul_timer_pause_cnt = 0;
		Update_User_Timer_Cnt(st_user_timer_p);
	}
}



/**
  * @brief 	void	Task_Get_Imu_Sensor_Data(void)
	* @note	 	��ȡIMU��̬������ϵ
  */

float		f_gimbal_ahrs_update_period;

void	Task_Get_Imu_Attitude(void)
{
	static	FlagStatus	en_first_run_this_task_flag = SET;
	
	/*����ahrs_update�ĸ�������*/
	static	uint32_t		ul_gimbal_ahrs_update_sys_micros;
	//	
	static	User_Timer_Typedef	st_ahrs_init_period_timer_p = USER_TIMER_INIT_VALUE;
	
	/*��һ��ִ�д˺��������ü��ٶȳ�ʼ����̬,��̬����*/
	if(SET == en_first_run_this_task_flag)
	{	
		en_first_run_this_task_flag = RESET;
//	Gimbal_Ahrs_Init();		
		en_ahrs_init_flag = RESET;
		return;
	}
	else
	{
	Start_User_Timer(&st_ahrs_init_period_timer_p);
		Update_User_Timer_Cnt(&st_ahrs_init_period_timer_p);
		if(st_ahrs_init_period_timer_p.ul_timer_cnt > 2000)
		{			
			/*��ʱ����д����Ҳ���Ի�����ʱ������kp=5����ʹ��Ԫ����������*/
			en_ahrs_init_flag = SET;
			Stop_User_Timer(&st_ahrs_init_period_timer_p);
		}
		//���㺯��ִ������
		f_gimbal_ahrs_update_period = Get_Function_Period(&ul_gimbal_ahrs_update_sys_micros);
		
		Gimbal_Ahrs_Update(f_gimbal_ahrs_update_period);
		
	}
				
	f_euler_array[0] = f_euler_x_right_array[0];
	f_euler_array[1] = f_euler_x_right_array[1];
	f_euler_array[2] = f_euler_x_right_array[2];
		
	f_quat_array[0] = f_quat_x_right_array[0];
	f_quat_array[1] = f_quat_x_right_array[1];
	f_quat_array[2] = f_quat_x_right_array[2];
	f_quat_array[3] = f_quat_x_right_array[3];
	
}




























































































































































