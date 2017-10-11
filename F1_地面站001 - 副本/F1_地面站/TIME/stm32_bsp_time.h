#ifndef _STM32_BSP_TIMER_H
#define _STM32_BSP_TIMER_H



void HAL_IncTick(void)£»
uint32_t HAL_GetTick(void)£»
float Get_Function_Period(uint32_t	*ul_last_sys_micros)£»
void	Update_User_Timer_Cnt(User_Timer_Typedef *st_user_timer_p)£»
void	Start_User_Timer(User_Timer_Typedef *st_user_timer_p)£»
void	Stop_User_Timer(User_Timer_Typedef *st_user_timer_p)£»


#endif