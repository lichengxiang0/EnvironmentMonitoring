#ifndef _ADCDMA_H_
#define _ADCDMA_H_
#include"config.h"
#define ADC1_DR_Address     ((u32)0x4001244C)

void ADCGPIO_config(void);
void ADC1_config(void);
void DMA_config(void);

//vu16 AD_Value[3]={1,1,1};//定义存储ADC转换结果的变量
#endif
