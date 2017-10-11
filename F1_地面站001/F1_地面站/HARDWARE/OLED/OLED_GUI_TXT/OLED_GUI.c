#include "OLED_GUI.h"
#include "oledfont.h" 
#include "stdlib.h"	 
#include "math.h"

  
static void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode,u8 play);  //��ʾһ���ַ�  16 8*16 ����  8  6*8    mode 1 ����  0��ɫ

//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  2 ȡ��
//play: 1��ʾ�Դ�   0����ʾ
void OLED_Fills(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot,u8 play)  
{  
	u8 x,y;  
	if(x1>x2)  //��֤ X1<X2
	{
   x=x1;
	 x1=x2;
	 x2=x1;
  }
	if(y1>y2)   //��֤ Y1<Y2
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
	 OLED_Gram_Update();//������ʾ
 }
}

//����
//x1,y1:�������
//x2,y2:�յ����� 
//n��ʾ��ʵ�ĵ㻹�ǿ��ĵ�
void OLED_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 n)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		OLED_DrawPoint(uRow,uCol,n);//���� 
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


//��ԲȦ
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

//��ʵ��ԲȦ
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

////�����ľ���
void OLED_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	OLED_DrawLine(x1,y1,x2,y1,1);
	OLED_DrawLine(x1,y1,x1,y2,1);
	OLED_DrawLine(x1,y2,x2,y2,1);
	OLED_DrawLine(x2,y1,x2,y2,1);
  OLED_Gram_Update();
}


//��ָ��
void OLED_DrawHand(u8 x1,u8 y1,u8 R,u8 T,u16 S,u8 n)
{
	u16 x,y;
	x=R*cos(T*S*pi/180-pi/2)+x1;
	y=R*sin(T*S*pi/180-pi/2)+y1;
	OLED_DrawLine(x1,y1,x,y,n);
}




//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16 8*16   8  6*8 
static void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode,u8 play)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 x0=x;  //��ʼ���걸��
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ	
  switch(size)
	{
    case 8:
		{
      for(t=0;t<6;t++)
			{
        temp=OLED_F6X8[(u16)chr*6+t];  //����1206����
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
			  temp=OLED_F8X16[(u16)chr*16+t+8];  //����8*16����
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
        temp=OLED_F8X16[(u16)chr*16+t];  //����8*16����
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

//m^n����
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//��ʾ1������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//play:1��ʾ���� 0����ʾ   2ˢ��ȫ����ʾ
//num:��ֵ(0~4294967295);	 		  
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
//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//play:  1��ʾ����   0����ʾ   2 ȫ��ˢ����ʾ
//��16����
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
      if(x>(X_WIDTH-6))  //����
      {
       x=x0;
		   y+=8;
      }
			
		  if(y>(Y_WIDTH-8))  //�����߽�
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
      if(x>(X_WIDTH-8))  //����
      {
       x=x0;
		   y+=16;
      }
			
		  if(y>(Y_WIDTH-16))  //�����߽�
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
  for(x=0;x<128;x++)  //X��������
  {   
		for(y=0;y<8;y++)  //Y���� ����
		{
		 temp=*BMP++;  //�õ�ͼƬ��һ���ֽ�����	
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

/**************************ʵ�ֺ���********************************************
*����ԭ��:	   	void Movie_Show_Img(u16 x,u16 y,u32 picx)
*��������:	   	OLED��ʾ����
*���������  		x:0~127
								y:0~63
								picx:ͼƬ���	
*���������  		��
*******************************************************************************/
void Movie_Show_Img(u32 picx)
{      			    
	Movie_Show_BMP(&movie_data[picx*1024]);
}

