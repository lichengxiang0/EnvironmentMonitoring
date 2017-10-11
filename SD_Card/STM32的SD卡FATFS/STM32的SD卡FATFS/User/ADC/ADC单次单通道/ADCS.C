#include"ADCS.H"
void ADCGPIO_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
  /* Configure PC.04 (ADC Channel14) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//PC.4为ADC1的14通道
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//端口输入模式为模拟输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);   
}

 void ADC1_config(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC的时钟为PCLK2经6分频后得到
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//打开ADC的时钟   
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	                /* 独立模式 */
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;			                /* 单通道模式 */
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	                /* 单次转换 */
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   /* 软件触发转换*/
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		        /* 右对齐 */
  ADC_InitStructure.ADC_NbrOfChannel = 1;					            /* 扫描通道数 */
  ADC_Init(ADC1, &ADC_InitStructure);

 /* ADC1 regular channel14 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_239Cycles5);//ADC1,14通道，转换次序为1，周期为239.5
  ADC_Cmd(ADC1, ENABLE);                                                /* Enable ADC1 */                      

}

       //ADC_SoftwareStartConvCmd(ADC1,ENABLE);    /* 软件触发一次转换开始 */
	/*if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==SET)
	{
       AD_value=ADC_GetConversionValue(ADC1);
        }*/
