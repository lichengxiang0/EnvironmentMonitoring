#include "stm32_bsp_timer.h"

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

__weak void HAL_IncTick(void)
{
  uwTick++;
}
__weak uint32_t HAL_GetTick(void)
{
  return uwTick;
}

/*---------------其他时间函数-----------------*/
/**
  * @brief 	float Get_Function_Period(uint32_t	*ul_last_sys_micros)
	* @note	 	计算函数执行周期(us)
  */
float Get_Function_Period(uint32_t	*ul_last_sys_micros)
{
	uint32_t	ul_now_sys_micros;
	float			period;
	
	ul_now_sys_micros = TIM_GetCounter(TIM2);
	
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
	* @note	 	更新用户TIMER(秒表)的CNT
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
	* @note	 	启动用户TIMER(秒表)
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
	* @note	 	停止用户TIMER(秒表)
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