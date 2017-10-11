#include "Cam_FIFO.h"

void Cam_FIFO_CLR(Cam_FIFO_typ * TIME_FIFO)                           //清空FIFO
{
  TIME_FIFO->STA =  Cam_FIFO_Free ;   //空闲   成功
  TIME_FIFO->DATA_Number = 0;         //有效数据清零
	TIME_FIFO->R_Number = 0;            //读指针清零
  TIME_FIFO->W_Number = 0;            //写指针
}

void Cam_FIFO_CLR_STA(Cam_FIFO_typ * TIME_FIFO,u8 Cam_FIFO_STA)      //清除FIFO状态标记
{
   TIME_FIFO->STA &=~ Cam_FIFO_STA;
}

void Cam_FIFO_SET_STA(Cam_FIFO_typ * TIME_FIFO,u8 Cam_FIFO_STA)       //设置FIFO状态
{
  TIME_FIFO->STA |= Cam_FIFO_STA;
}

u8   Cam_FIFO_RETURN_STA(Cam_FIFO_typ * TIME_FIFO)                   //返回当前FIFO状态
{
  return TIME_FIFO->STA ;
}

u16  Cam_FIFO_BUF_Valid_NUM(Cam_FIFO_typ * TIME_FIFO)                 //返回当前缓存有效数据
{
   return (u16)TIME_FIFO->DATA_Number;
}

u8   Cam_FIFO_STA_UP(Cam_FIFO_typ * TIME_FIFO)                        //更新FIFO状态并返回
{
  if(Cam_FIFO_BUF_Valid_NUM(TIME_FIFO)>0)
	{
	  if(Cam_FIFO_BUF_Valid_NUM(TIME_FIFO) == Cam_TIME_MAX)   //满
		{
		 Cam_FIFO_SET_STA(TIME_FIFO,Cam_FIFO_over) ;
		}
		else   //有数据
		{
		 Cam_FIFO_SET_STA(TIME_FIFO,Cam_FIFO_DATA) ;
		}
	}
	else   //没有数据  
	{
	 Cam_FIFO_SET_STA(TIME_FIFO,Cam_FIFO_NUM) ;
	}
	
	return Cam_FIFO_RETURN_STA(TIME_FIFO);
}

u8 Cam_FIFO_WRITE(Cam_FIFO_typ * TIME_FIFO,_calendar_obj * TIME)      //返回 0  成功  其它失败
{
  if(Cam_FIFO_BUF_Valid_NUM(TIME_FIFO)>=Cam_TIME_MAX)  //如果缓存有效数据满
	{
		 return Cam_FIFO_over;     //返回当前状态   返回满状态
	}
	
 //写入数据------------------------------------------------------------
  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].hour =  TIME->hour ;           

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].min =  TIME->min ;   

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].sec =  TIME->sec ;   

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].micro_sec =  TIME->micro_sec ;   

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].w_year =  TIME->w_year ; 
  
  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].w_month =  TIME->w_month ;   

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].w_date =  TIME->w_date ;   

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].week =  TIME->week ;   

//写数据完成----------------------------------------------------------------------	
  TIME_FIFO->DATA_Number++;      //缓存有效数据加一     一定要先写完 再地址变更
	TIME_FIFO->W_Number++ ;	   //写指针加一
	if(TIME_FIFO->W_Number>= Cam_TIME_MAX)  //如果到了缓存尾
	{
	    TIME_FIFO->W_Number = 0 ;             //回归到缓存头
	}
	return  Cam_FIFO_OK;      //返回当前状态
}


u8 Cam_FIFO_READ(Cam_FIFO_typ * TIME_FIFO,_calendar_obj * TIME)    //返回 0  成功  其它失败
{
  if(Cam_FIFO_BUF_Valid_NUM(TIME_FIFO)==0)  //如果缓存没有数据
	{
		 return Cam_FIFO_NUM;     //返回当前状态  返回数据不足
	}
	
 //读数据------------------------------------------------------------
  TIME->hour = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].hour ;           

  TIME->min = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].min   ;   

  TIME->sec = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].sec  ;   

  TIME->micro_sec = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].micro_sec  ;   

  TIME->w_year = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].w_year  ; 
  
  TIME->w_month  = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].w_month  ;   

  TIME->w_date  =  TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].w_date  ;   

  TIME->week  = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].week     ;   

//写数据完成----------------------------------------------------------------------	
  TIME_FIFO->DATA_Number--;      //缓存有效数据减一
	TIME_FIFO->R_Number++ ;	       //写指针加一
	
	if(TIME_FIFO->R_Number>= Cam_TIME_MAX)  //如果到了缓存尾
	{
	    TIME_FIFO->R_Number = 0 ;             //回归到缓存头
	}
	
	return  Cam_FIFO_OK;      //返回当前状态
}

















