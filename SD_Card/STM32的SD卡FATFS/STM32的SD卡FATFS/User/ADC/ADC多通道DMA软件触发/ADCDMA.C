#include"ADCDMA.H"
void ADCGPIO_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  /* Configure PA.0 (ADC Channel0) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//端口输入模式为模拟输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
}
void ADC1_config(void)
{
     ADC_InitTypeDef ADC_InitStructure;
	 RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC的时钟为PCLK2经6分频后得到
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//开启ADC1的时钟
     ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立工作模式
     ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描方式
     ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //连续转换禁止
     ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发
     ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐方式
     ADC_InitStructure.ADC_NbrOfChannel = 2;      //设置转换序列长度为2
     ADC_Init(ADC1, &ADC_InitStructure);//调用函数，初始化ADC1

     //ADC内置温度传感器使能（要使用片内温度传感器，切忌要开启它）
     ADC_TempSensorVrefintCmd(ENABLE);

     //规则转换序列1：通道0
     ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);
     //常规转换序列2：通道16（内部温度传感器），采样时间>2.2us,(239.5cycles)
     ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 2, ADC_SampleTime_239Cycles5);
     // 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）

     ADC_DMACmd(ADC1, ENABLE);

     // Enable ADC1
     ADC_Cmd(ADC1, ENABLE);



     // 下面是ADC自动校准，需执行一次，保证精度
     // Enable ADC1 reset calibaration register 
     ADC_ResetCalibration(ADC1);
     // Check the end of ADC1 reset calibration register
     while(ADC_GetResetCalibrationStatus(ADC1));
     // Start ADC1 calibaration
     ADC_StartCalibration(ADC1);
     // Check the end of ADC1 calibration
     while(ADC_GetCalibrationStatus(ADC1));
     // ADC自动校准结束---------------
     }

void DMA_config(void)
{
     DMA_InitTypeDef DMA_InitStructure;	
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//开启DMA的时钟
     DMA_DeInit(DMA1_Channel1);
     DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//外设地址
     DMA_InitStructure.DMA_MemoryBaseAddr = (u32)AD_Value;//内存地址/*自己开辟的数组*/
     DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//DMA单向传输方式

     DMA_InitStructure.DMA_BufferSize = 2;//BufferSize=2，因为ADC转换序列有2个通道
    //如此设置，使序列1结果放在AD_Value[0]，序列2结果放在AD_Value[1]

     DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设置DMA外设递增模式禁止
     DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //设置DMA内存递增模式使能
     DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设字长为半字（16位）
     DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//内存地址
     //循环模式开启，Buffer写满后，自动回到初始地址开始传输
     DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 设置传输模式连续不断的循环模式
     DMA_InitStructure.DMA_Priority = DMA_Priority_High;//设置DMA优先级
     DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;// 设置DMA的2个memory中的变量互相访问，内存到内存禁止
     DMA_Init(DMA1_Channel1, &DMA_InitStructure);//配置完成后，启动DMA通道
     DMA_Cmd(DMA1_Channel1, ENABLE);
}

