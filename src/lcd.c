#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>
#include "bmp.h"
#include "lcd.h"

static int *plcd;

/*
	@brief 在坐标为(x, y)的像素点画一个颜色为color的点
	@x 横坐标
	@y 纵坐标
	@color 要显示的颜色
*/
static void lcd_draw_point(int x, int y, int color)
{
	//plcd[800*y + x] = color;
	*(plcd + 800*y + x) = color;	
}

// 清屏
void lcd_clear(int color)
{
	int i, j;
	
	for (i = 0; i < 480; i++)
	{
		for (j = 0; j < 800; j++)
			lcd_draw_point(j, i, color);
	}
}

/*
	@brief 在顶点为（x0,y0）的位置开始画一个宽为w，高为h，颜色为color的矩形
*/
void lcd_draw_rect(int x0, int y0, int w, int h, int color)
{
	int x, y;
	
	for (y = y0; y < y0+h; y++)
	{
		for (x = x0; x < x0+w; x++)
			lcd_draw_point(x, y, color);
	}
}

//显示一个字
void lcd_draw_word1(unsigned char *ch, int color, int len,  int w, int x, int y)
{
	int i, j;
	
	int high;
	
	//32 * 64
	// 一个字的显示：画点（一个一个像素点）
	// 八个点用一个 unsigned char 
	// 已知：数组长度：len => 像素点的个数：len * 8
	// 已知：字的宽度=> 高度：len*8/w 
	high = len*8 / w; //64 
	
	int flag;
	flag = w/8; // 4
	
	for (i = 0; i < len; i++)
	{
		for (j = 7; j >= 0; j--)
		{
			if ((ch[i] >> j) & 1 == 1)
			{
				if (i%flag == 0)
					lcd_draw_point(7-j+x, i/flag+y, color);
				else
					lcd_draw_point(8*(i%flag)+7-j+x, i/flag+y, color);
			}
		}
	}
}

//初始化LCD
int lcd_init(int color)
{
	//第一步：初始化 LCD
	int fd;
	fd = open("/dev/fb0", O_RDWR);
	if (fd < 0)
		return -1;
	
	//把lcd //映射到进程的地址空间中，这块地址空间的首地址保///存在plcd中（plcd这个指针指向了映射的地址空间，///意味着，我们操作plcd这个指针就相当于操作lcd：在//lcd上画点）
	plcd = mmap(NULL, 800*480*4, 
	            PROT_WRITE | PROT_READ,
	            MAP_SHARED,
		        fd, 0);
	
	lcd_clear(color);
	
	return 0;
}

void lcd_design()
{
	int i,n,k=0;
	unsigned char data;
	for(i=0;i<48000;i++)
	{
		data = mod_bmp[i];		
		for(n = 7;n >=0;n--)
			{
				if((data >> n) & 1)
				{
					*(plcd+k) = 0xabcdef;
					k++;
				}
				else
				{
					*(plcd+k) = 0xffffff;
					k++;
				}
			}
	}
}

void lcd_draw_finger(unsigned char *finger, int x, int y)
{
	unsigned char gray;
	int i, j;
	int pos;
	int color;
	
	for (i = 0; i < 124; i++)
	{
		for (j = 0; j < 148; j++)
		{
			//每一行占多少个字节：124 / 2
			pos = (124/2) * j + i/2;
			
			if (i % 2 == 0)
				gray = finger[pos] & 0x0f; //取低4位
			else
				gray = (finger[pos] >> 4) & 0x0f;
			
			gray <<= 4;
			
			//组成一个4字节的颜色
			color = (gray << 16) | (gray << 8) | gray;
			
			lcd_draw_point(i+x, j+y, color);
			lcd_draw_point(i+1+x, j+y, color);
			lcd_draw_point(i+x, j+1+y, color);
			lcd_draw_point(i+1+x, j+1+y, color);
		}
	}	
}