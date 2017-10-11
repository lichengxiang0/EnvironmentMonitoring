#ifndef	_SOFT_IIC_H
#define _SOFT_IIC_H
//#include "stm32f1xx_hal.h"
#include "stm32f10x.h"
#include "sys.h"
//typedef enum {I2C_READY = 0,I2C_BUS_BUSY,I2C_BUS_ERROR,I2C_NACK,I2C_ACK} I2C_Status;

static __IO uint32_t uwTick;

typedef enum{USER_TIMER_STOP = 0,	USER_TIMER_RUN,	USER_TIMER_PAUSE}User_Timer_State_Typedef;

typedef	struct
{
	User_Timer_State_Typedef	en_timer_state;	
	uint32_t						ul_timer_start_cnt;
	uint32_t						ul_timer_now_cnt;
	uint32_t						ul_timer_once_cnt;
	uint32_t						ul_timer_pause_cnt;
	uint32_t						ul_timer_cnt;
}User_Timer_Typedef;


#define		ICM20602_DEVICE_ADDRESS										(0xD0)  //0xD0@SA0=0  0xD2@SA0=1 

#define		ICM20602_ID_REG_ADDRESS										(0x75)  //who am i

/*ICM20602配置寄存器*/
#define 	ICM20602_CFG_REG_ADDR											(0x1A)
#define 	ICM20602_GYRO_CFG_REG_ADDR								(0x1B)
#define 	ICM20602_ACC_CFG_REG_ADDR									(0x1C)
#define 	ICM20602_ACC_CFG2_REG_ADDR								(0x1D)
#define 	ICM20602_GYRO_LPM_CFG_REG_ADDR						(0x1E)


/*ICM20602的数据寄存器*/
#define 	ICM20602_ACC_XOUT_H_REG_ADDR							(0x3B)
#define 	ICM20602_ACC_XOUT_L_REG_ADDR							(0x3C)
#define 	ICM20602_ACC_YOUT_H_REG_ADDR							(0x3D)
#define 	ICM20602_ACC_YOUT_L_REG_ADDR							(0x3E)
#define 	ICM20602_ACC_ZOUT_H_REG_ADDR							(0x3F)
#define 	ICM20602_ACC_ZOUT_L_REG_ADDR							(0x40)

#define 	ICM20602_TMEP_OUT_H_REG_ADDR							(0x41)
#define 	ICM20602_TMEP_OUT_L_REG_ADDR							(0x42)

#define 	ICM20602_GYRO_XOUT_H_REG_ADDR							(0x43)
#define 	ICM20602_GYRO_XOUT_L_REG_ADDR							(0x44)
#define 	ICM20602_GYRO_YOUT_H_REG_ADDR							(0x45)
#define 	ICM20602_GYRO_YOUT_L_REG_ADDR							(0x46)
#define 	ICM20602_GYRO_ZOUT_H_REG_ADDR							(0x47)
#define 	ICM20602_GYRO_ZOUT_L_REG_ADDR							(0x48)

/*ICM20602的电源寄存器*/
#define 	ICM20602_USER_CTRL_REG_ADDR								(0x6A)
#define 	ICM20602_PWR_MGMT_1_REG_ADDR							(0x6B)
#define 	ICM20602_PWR_MGMT_2_REG_ADDR							(0x6C)



/*icm20602*/
#define	GIMBAL_GYRO_X_DATA_INDEX		((uint8_t)0)
#define	GIMBAL_GYRO_Y_DATA_INDEX		((uint8_t)1)
#define	GIMBAL_GYRO_Z_DATA_INDEX		((uint8_t)2)	 
/*mma8452 ps test*/
#define	GIMBAL_ACC_X_DATA_INDEX			((uint8_t)0)
#define	GIMBAL_ACC_Y_DATA_INDEX			((uint8_t)1)
#define	GIMBAL_ACC_Z_DATA_INDEX			((uint8_t)2)	
	
	
	
static		float	f_icm20602_gyro_scale = 1/16.4f;    	//default 2000dps
static		float	f_icm20602_acc_scale = 1/16384.f;		//default ±2g


//#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
//#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

#define SDA_IN() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)3<<12;}

//IO操作函数	 
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //输入SDA 

//IIC所有操作函数
void I2C_Configuration_1(void);		 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

void AT24CXX_WriteOneByte(u8 WriteAddr,u8 DataToWrite);		//指定地址写入一个字节


void I2C_Configuration(void);
void TIM3_Configuration(void);
void Tim4_Init(u16 period_num);
void I2C_Write_Single_Reg(uint8_t device_addr,uint8_t reg_addr,uint8_t reg_value);
void I2C_Write_Multi_Reg(uint8_t device_addr,uint8_t reg_addr,uint8_t *value_buf_addr, uint8_t reg_number);
uint8_t I2C_Read_Single_Reg(uint8_t device_addr,uint8_t reg_addr);
void I2C_Read_Multi_Reg(uint8_t device_addr,uint8_t reg_addr, uint8_t *value_buf_addr, uint8_t reg_number); 
void	Gyro_Data_Fliter(float* p_f_gyro_in_p,	float* p_f_gyro_out_p,	FlagStatus	*en_first_run_flag_p);
void	Cal_Filter_Sensor_Data(void);
void	Read_Icm20602_Data_Soft(float *	p_f_icm20602_gyro_data_p,	float *p_f_icm20602_acc_data_p);
void	Acc_Data_Fliter(float* p_f_acc_in_p,	float* p_f_acc_out_p,	FlagStatus *en_first_run_flag_p);
void	Init_Icm20602_Soft(void);
void	Task_Get_Imu_Sensor_Data(void);

void HAL_IncTick(void);
uint32_t HAL_GetTick(void);
float Get_Function_Period(uint32_t	*ul_last_sys_micros);
void	Update_User_Timer_Cnt(User_Timer_Typedef *st_user_timer_p);
void	Start_User_Timer(User_Timer_Typedef *st_user_timer_p);
void	Stop_User_Timer(User_Timer_Typedef *st_user_timer_p);
void	Task_Get_Imu_Attitude(void);
#endif
