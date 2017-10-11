#include "OLED_GUI.h"
#include "oledfont.h" 
#include "stdlib.h"	 
#include "math.h"

  
static void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode,u8 play);  //显示一个字符  16 8*16 或者  8  6*8    mode 1 正常  0反色

//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  2 取反
//play: 1显示显存   0不显示
void OLED_Fills(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot,u8 play)  
{  
	u8 x,y;  
	if(x1>x2)  //保证 X1<X2
	{
   x=x1;
	 x1=x2;
	 x2=x1;
  }
	if(y1>y2)   //保证 Y1<Y2
	{
   y=y1;
	 y1=y2;
	 y2=y1;
  }
	
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
		{
			OLED_DrawPoint(x,y,dot);
    }
	}
	
 if(play)	
 {
	 OLED_Gram_Update();//更新显示
 }
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标 
//n表示是实心点还是空心点
void OLED_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 n)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		OLED_DrawPoint(uRow,uCol,n);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 


//画圆圈
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int x1,y1,r1;
		x1=0;
		y1=r;
		r1=1-r;
	while(x1<=y1)
	{
		OLED_DrawPoint(x+x1,y+y1,1);
		OLED_DrawPoint(x+y1,y+x1,1);
		OLED_DrawPoint(x-y1,y+x1,1);
		OLED_DrawPoint(x-x1,y+y1,1);
		OLED_DrawPoint(x-x1,y-y1,1);
		OLED_DrawPoint(x-y1,y-x1,1);
		OLED_DrawPoint(x+y1,y-x1,1);
		OLED_DrawPoint(x+x1,y-y1,1);
		if(r1<0)
		{
			r1+=2*x1+3;
		}
		else
		{
			r1+=2*(x1-y1)+5;
			y1--;
		}
		x1++;
	}
}

//画实心圆圈
void OLED_DrawFillCircle(u8 x,u8 y,u8 r,u8 n)
{
	int x1,y1,r1;
	x1=0;
	y1=r;
	r1=1-r;
	while(x1<=y1)
	{
		OLED_DrawLine(x+x1,y+y1,x-x1,y+y1,n);
		OLED_DrawLine(x+x1,y-y1,x-x1,y-y1,n);
		OLED_DrawLine(x+y1,y+x1,x-y1,y+x1,n);
		OLED_DrawLine(x+y1,y-x1,x-y1,y-x1,n);
		
		if(r1<0)
		{
			r1+=2*x1+3;
		}
		else
		{
			r1+=2*(x1-y1)+5;
			y1--;
		}
		x1++;
	}
}

////画空心矩形
void OLED_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	OLED_DrawLine(x1,y1,x2,y1,1);
	OLED_DrawLine(x1,y1,x1,y2,1);
	OLED_DrawLine(x1,y2,x2,y2,1);
	OLED_DrawLine(x2,y1,x2,y2,1);
  OLED_Gram_Update();
}


//画指针
void OLED_DrawHand(u8 x1,u8 y1,u8 R,u8 T,u16 S,u8 n)
{
	u16 x,y;
	x=R*cos(T*S*pi/180-pi/2)+x1;
	y=R*sin(T*S*pi/180-pi/2)+y1;
	OLED_DrawLine(x1,y1,x,y,n);
}




//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16 8*16   8  6*8 
static void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode,u8 play)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 x0=x;  //起始坐标备份
	chr=chr-' ';//得到偏移后的值	
  switch(size)
	{
    case 8:
		{
      for(t=0;t<6;t++)
			{
        temp=OLED_F6X8[(u16)chr*6+t];  //调用1206字体
        for(t1=0;t1<8;t1++)
		    {
		      if(temp&0x80)
			    {
				     OLED_DrawPoint(x,y,mode);
          }
			    else 
			    {
				    OLED_DrawPoint(x,y,!mode);
          }
		     temp<<=1;
		     y++;
		   } 
			 x++;
			 y=y0;
     }
    }			
		break;
		
		case 16:
		{
      for(t=0;t<8;t++)
			{
			  temp=OLED_F8X16[(u16)chr*16+t+8];  //调用8*16字体
        for(t1=0;t1<8;t1++)
		    {
		      if(temp&0x80)
			    {
				    OLED_DrawPoint(x,y,mode);
          }
			    else 
			    {
				    OLED_DrawPoint(x,y,!mode);
          }
		      temp<<=1;
		      y++;
		    } 
        temp=OLED_F8X16[(u16)chr*16+t];  //调用8*16字体
        for(t1=0;t1<8;t1++)
		    {
		      if(temp&0x80)
			    {
				     OLED_DrawPoint(x,y,mode);
          }
			    else 
			    {
				    OLED_DrawPoint(x,y,!mode);
          }
		      temp<<=1;
		      y++;			
		    } 
			 x++;
			 y=y0;
     }
    }				
		break;

    default:
		break;
  }  
	
 if(play)
 {
	switch(size)
	{
		case 8:
			  	OLED_SHOW_GRAM(x0,x0+8,y0);
		break;
		
		case 16:
			    OLED_SHOW_GRAM(x0,x0+16,y0);
		break;
	 
	 	default:
		break;
	}
 }	
}

//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//显示1个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//play:1显示缓存 0不显示   2刷新全屏显示
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size,u8 play)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(size==8)
				{  
					OLED_ShowChar(x+6*t,y,' ',size,1,(play==1)?1:0);
        }
				else if(size==16)
				{
          OLED_ShowChar(x+8*t,y,' ',size,1,play);
        }
				continue;
			}else enshow=1; 	 	 
		}
		
	  if(size==8)
	  {
	  	OLED_ShowChar(x+6*t,y,temp+'0',size,1,(play==1)?1:0); 
	  }
	  else if(size==16)
	  {
   	  OLED_ShowChar(x+8*t,y,temp+'0',size,1,(play==1)?1:0); 
    }
   }
	if(play == 2 )
	{
	  OLED_Gram_Update();
	}
 }
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//play:  1显示缓存   0不显示   2 全屏刷新显示
//用16字体
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size,u8 mode,u8 play)
{   
  u8 x0=x;
	u8 err=0;
  while((*p!='\0')&&(err==0))   
  {  
   switch(size)
   {
    case 8:
		{
      if(x>(X_WIDTH-6))  //换行
      {
       x=x0;
		   y+=8;
      }
			
		  if(y>(Y_WIDTH-8))  //超出边界
      {
			 err = 1;
			 break;
      }
			
		  OLED_ShowChar(x,y,*p,8,mode,(play==1)?1:0);	 
		  x+=6;
      p++; 
     }
		break;
		
		case 16:
		{
      if(x>(X_WIDTH-8))  //换行
      {
       x=x0;
		   y+=16;
      }
			
		  if(y>(Y_WIDTH-16))  //超出边界
      {
			 err = 1;
			 break;
      }
			
		 OLED_ShowChar(x,y,*p,16,mode,(play==1)?1:0);	 
		 x+=8;
     p++; 
   }	
	 break;
		
	default:
	break;
  }		
 }
	
 if(play == 2 )
 {
	 OLED_Gram_Update();
 }	
} 



void Movie_Show_BMP(const u8 *BMP)
{
	u8 x=0,y=0;
	u8  temp=0;
	u8  t1=0;  
  for(x=0;x<128;x++)  //X坐标数据
  {   
		for(y=0;y<8;y++)  //Y坐标 数据
		{
		 temp=*BMP++;  //得到图片的一个字节数据	
     for(t1=0;t1<8;t1++)
		 {
			 if(temp&0x80)
			 {
			 	OLED_DrawPoint(x,63-y*8-t1,1); 	
			 }
			 else
			 {				
				OLED_DrawPoint(x,63-y*8-t1,0);
			 }				
			 temp<<=1;
		 }
	  } 
	 }
	OLED_Gram_Update();	
}

/**************************实现函数********************************************
*函数原型:	   	void Movie_Show_Img(u16 x,u16 y,u32 picx)
*功　　能:	   	OLED显示动画
*输入参数：  		x:0~127
								y:0~63
								picx:图片编号	
*输出参数：  		无
*******************************************************************************/
void Movie_Show_Img(u32 picx)
{      			    
	Movie_Show_BMP(&movie_data[picx*1024]);
}

