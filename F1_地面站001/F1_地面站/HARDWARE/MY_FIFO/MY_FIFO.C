#include "MY_FIFO.H"
#include "PLAY_DATA.H"
//------------底层----------------------------------------------------------
static void FIFO_WRITE_BYTE_NO(MY_FIFO_typ * FIFO,u8 date)
{
	FIFO->FIFO_BUFF[FIFO->W_ADDR_NUM] = date ;   //写入数据
	 
  FIFO->DATA_NUM++;      //缓存有效数据加一     一定要先写完 再地址变更
	 
	if(FIFO->W_ADDR_NUM>=(FIFO_BUF_MAX-1))  //如果到了缓存尾
	{
	    FIFO->W_ADDR_NUM = 0 ;             //回归到缓存头
	}
	else
	{
	    FIFO->W_ADDR_NUM++ ;	 
	}
}

static void FIFO_READ_BYTE_NO(MY_FIFO_typ * FIFO,u8 *date_Byte) 
{
  *date_Byte = FIFO->FIFO_BUFF[FIFO->R_ADDR_NUM] ;   //读取数据
	 
	FIFO->DATA_NUM--;      //缓存有效数据加一     一定要先读完 再地址变更
	 
	if(FIFO->R_ADDR_NUM>=(FIFO_BUF_MAX-1))  //如果到了缓存尾
	{
	 FIFO->R_ADDR_NUM = 0 ;             //回归到缓存头
	}
	else
  {
	 FIFO->R_ADDR_NUM++ ;	 
	}
}
//----------------------------------------------------------------



void FIFO_CLR_BUF(MY_FIFO_typ * FIFO)   //清空FIFO
{
	FIFO->FIFO_STA = FIFO_Free;    //状态为空闲
  FIFO->DATA_NUM = 0;    //有效数据清零
	FIFO->W_ADDR_NUM = 0;  //写指针清零
  FIFO->R_ADDR_NUM = 0;  //读指针清零
}



u8 FIFO_WRITE_Byte(MY_FIFO_typ * FIFO,u8 date)              //返回 0  成功  其它失败
{
   if(FIFO_BUF_Valid_NUM(FIFO)>=FIFO_BUF_MAX)  //如果缓存有效数据满
	 {
		 return  FIFO_over;     //返回当前状态   返回满状态
	 }
   FIFO_WRITE_BYTE_NO(FIFO,date);
	 return  FIFO_OK;      //返回当前状态
}

u8  FIFO_WRITE_BUF(MY_FIFO_typ * FIFO,u8 *buf,u16 lenght)    //返回 0  成功  其它失败
{
   u16 i=0;
	 if((FIFO_BUF_Valid_NUM(FIFO)+lenght)>FIFO_BUF_MAX)   //如果缓存有效数据数量与写入总和  超过最大有效数据
	 {
	   return  FIFO_over;      //返回满
	 }
	 
	 for(i=0;i<lenght;i++)
	 {
	    FIFO_WRITE_BYTE_NO(FIFO,buf[i]);  
	 }
    return   FIFO_OK;
}


u8  FIFO_READ_Byte(MY_FIFO_typ * FIFO,u8 *date_Byte)       //返回 0  成功  其它失败
{
   if(FIFO_BUF_Valid_NUM(FIFO)==0)     //如果缓存有效数据为空
	 {
		 return  FIFO_NUM;    //返回当前状态    
	 }
   FIFO_READ_BYTE_NO(FIFO,date_Byte); 
	 return  FIFO_OK;      //返回当前状态
}

u8  FIFO_READ_BUF(MY_FIFO_typ * FIFO,u8 *buf,u16 lenght)    //返回 0  成功  其它失败
{
   u16 i=0;
	 if(FIFO_BUF_Valid_NUM(FIFO)<lenght)   //如果缓存有效数据数量小于要读取的数据数量
	 {
	   return  FIFO_NUM;      //返回当前状态
	 }
	 
	 for(i=0;i<lenght;i++)
	 {
	    FIFO_READ_BYTE_NO(FIFO,&buf[i]);  
	 }
    return   FIFO_OK;
}

u8   FIFO_RETURN_STA(MY_FIFO_typ * FIFO)                             //返回当前FIFO状态
{
    return   FIFO->FIFO_STA ;
}

void FIFO_CLR_STA(MY_FIFO_typ * FIFO,u8 FIFO_STA)                  //清除FIFO状态标记
{
  FIFO->FIFO_STA = FIFO_STA_UP(FIFO)&(~FIFO_STA);      //更新FIFO状态再清除
}

void FIFO_SET_STA(MY_FIFO_typ * FIFO,u8 FIFO_STA)          //设置FIFO状态
{
   FIFO->FIFO_STA = FIFO_STA_UP(FIFO)| FIFO_STA;      //更新FIFO状态再设置
}


u8 FIFO_STA_UP(MY_FIFO_typ * FIFO)                                 //更新FIFO状态并返回
{
	if(FIFO_BUF_Valid_NUM(FIFO) == 0)
	{
	  FIFO->FIFO_STA = FIFO_Free;	  //空闲
	}
	else if(FIFO_BUF_Valid_NUM(FIFO) >=FIFO_BUF_MAX)
	{
	  FIFO->FIFO_STA = FIFO_over ;   //满
	}
	return FIFO_OK;
}

u16  FIFO_BUF_Valid_NUM(MY_FIFO_typ * FIFO)                //返回当前缓存有效数据
{
  return  FIFO->DATA_NUM;
}

