#include"IWDG.H"
void IWDG_Configuration(void)
{
RCC_LSICmd(ENABLE);//打开LSI
while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);//等待LSI稳定

/* 写入0x5555,用于允许狗狗寄存器写入功能 */
IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

/* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/
IWDG_SetPrescaler(IWDG_Prescaler_256);

/* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/
IWDG_SetReload(781);

/* 喂狗*/
IWDG_ReloadCounter();

/* 使能狗狗*/
IWDG_Enable();
}

