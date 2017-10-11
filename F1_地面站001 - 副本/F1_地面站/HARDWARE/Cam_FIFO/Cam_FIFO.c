#include "Cam_FIFO.h"

void Cam_FIFO_CLR(Cam_FIFO_typ * TIME_FIFO)                           //���FIFO
{
  TIME_FIFO->STA =  Cam_FIFO_Free ;   //����   �ɹ�
  TIME_FIFO->DATA_Number = 0;         //��Ч��������
	TIME_FIFO->R_Number = 0;            //��ָ������
  TIME_FIFO->W_Number = 0;            //дָ��
}

void Cam_FIFO_CLR_STA(Cam_FIFO_typ * TIME_FIFO,u8 Cam_FIFO_STA)      //���FIFO״̬���
{
   TIME_FIFO->STA &=~ Cam_FIFO_STA;
}

void Cam_FIFO_SET_STA(Cam_FIFO_typ * TIME_FIFO,u8 Cam_FIFO_STA)       //����FIFO״̬
{
  TIME_FIFO->STA |= Cam_FIFO_STA;
}

u8   Cam_FIFO_RETURN_STA(Cam_FIFO_typ * TIME_FIFO)                   //���ص�ǰFIFO״̬
{
  return TIME_FIFO->STA ;
}

u16  Cam_FIFO_BUF_Valid_NUM(Cam_FIFO_typ * TIME_FIFO)                 //���ص�ǰ������Ч����
{
   return (u16)TIME_FIFO->DATA_Number;
}

u8   Cam_FIFO_STA_UP(Cam_FIFO_typ * TIME_FIFO)                        //����FIFO״̬������
{
  if(Cam_FIFO_BUF_Valid_NUM(TIME_FIFO)>0)
	{
	  if(Cam_FIFO_BUF_Valid_NUM(TIME_FIFO) == Cam_TIME_MAX)   //��
		{
		 Cam_FIFO_SET_STA(TIME_FIFO,Cam_FIFO_over) ;
		}
		else   //������
		{
		 Cam_FIFO_SET_STA(TIME_FIFO,Cam_FIFO_DATA) ;
		}
	}
	else   //û������  
	{
	 Cam_FIFO_SET_STA(TIME_FIFO,Cam_FIFO_NUM) ;
	}
	
	return Cam_FIFO_RETURN_STA(TIME_FIFO);
}

u8 Cam_FIFO_WRITE(Cam_FIFO_typ * TIME_FIFO,_calendar_obj * TIME)      //���� 0  �ɹ�  ����ʧ��
{
  if(Cam_FIFO_BUF_Valid_NUM(TIME_FIFO)>=Cam_TIME_MAX)  //���������Ч������
	{
		 return Cam_FIFO_over;     //���ص�ǰ״̬   ������״̬
	}
	
 //д������------------------------------------------------------------
  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].hour =  TIME->hour ;           

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].min =  TIME->min ;   

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].sec =  TIME->sec ;   

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].micro_sec =  TIME->micro_sec ;   

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].w_year =  TIME->w_year ; 
  
  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].w_month =  TIME->w_month ;   

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].w_date =  TIME->w_date ;   

  TIME_FIFO->TIME_BUF[TIME_FIFO->W_Number].week =  TIME->week ;   

//д�������----------------------------------------------------------------------	
  TIME_FIFO->DATA_Number++;      //������Ч���ݼ�һ     һ��Ҫ��д�� �ٵ�ַ���
	TIME_FIFO->W_Number++ ;	   //дָ���һ
	if(TIME_FIFO->W_Number>= Cam_TIME_MAX)  //������˻���β
	{
	    TIME_FIFO->W_Number = 0 ;             //�ع鵽����ͷ
	}
	return  Cam_FIFO_OK;      //���ص�ǰ״̬
}


u8 Cam_FIFO_READ(Cam_FIFO_typ * TIME_FIFO,_calendar_obj * TIME)    //���� 0  �ɹ�  ����ʧ��
{
  if(Cam_FIFO_BUF_Valid_NUM(TIME_FIFO)==0)  //�������û������
	{
		 return Cam_FIFO_NUM;     //���ص�ǰ״̬  �������ݲ���
	}
	
 //������------------------------------------------------------------
  TIME->hour = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].hour ;           

  TIME->min = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].min   ;   

  TIME->sec = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].sec  ;   

  TIME->micro_sec = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].micro_sec  ;   

  TIME->w_year = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].w_year  ; 
  
  TIME->w_month  = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].w_month  ;   

  TIME->w_date  =  TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].w_date  ;   

  TIME->week  = TIME_FIFO->TIME_BUF[TIME_FIFO->R_Number].week     ;   

//д�������----------------------------------------------------------------------	
  TIME_FIFO->DATA_Number--;      //������Ч���ݼ�һ
	TIME_FIFO->R_Number++ ;	       //дָ���һ
	
	if(TIME_FIFO->R_Number>= Cam_TIME_MAX)  //������˻���β
	{
	    TIME_FIFO->R_Number = 0 ;             //�ع鵽����ͷ
	}
	
	return  Cam_FIFO_OK;      //���ص�ǰ״̬
}

















