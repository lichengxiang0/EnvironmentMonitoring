#include "IAP_PLAY.H"
#include "IAP_DATA.H"
#include "IAP_EXE.H"
#include "ff.h"
#include "sys.h"
#include "delay.h"
#include "sys.h"
#include "Led.h"
#include "Oled.h"
#include "Malloc.h"
#include "sdio_sdcard.h"
#include <string.h>
#include "OLED_GUI.h"
#include "STM32_CRC.h"
#include "stmflash.h"
#include "IAP_PLAY.H"

//显示当前字体更新进度
//x,y:坐标
//size:字体大小
//fsize:整个文件大小
//pos:当前文件指针位置
void IAP_EXE_PALY_prog(u8 x,u8 y,u32 fsize,u32 pos)
{
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		OLED_ShowString(x+24,y,"%",16,1,1);		
		t=prog;
		if(t>100)
		{
			t=100;
		}
		OLED_ShowNum(x,y,t,3,16,1);//显示数值
 //显示进度条----------------------------------------------------------
		OLED_Fills(14,y-4-20 ,14+t,y-4,1,0);    //填充     高度20个像素
		OLED_DrawRectangle(14,y-4-20 ,114,y-4); //画空心矩形  刷新缓存
	}					    
} 










