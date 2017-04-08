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
	@brief ������Ϊ(x, y)�����ص㻭һ����ɫΪcolor�ĵ�
	@x ������
	@y ������
	@color Ҫ��ʾ����ɫ
*/
static void lcd_draw_point(int x, int y, int color)
{
	//plcd[800*y + x] = color;
	*(plcd + 800*y + x) = color;	
}

// ����
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
	@brief �ڶ���Ϊ��x0,y0����λ�ÿ�ʼ��һ����Ϊw����Ϊh����ɫΪcolor�ľ���
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

//��ʾһ����
void lcd_draw_word1(unsigned char *ch, int color, int len,  int w, int x, int y)
{
	int i, j;
	
	int high;
	
	//32 * 64
	// һ���ֵ���ʾ�����㣨һ��һ�����ص㣩
	// �˸�����һ�� unsigned char 
	// ��֪�����鳤�ȣ�len => ���ص�ĸ�����len * 8
	// ��֪���ֵĿ��=> �߶ȣ�len*8/w 
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

//��ʼ��LCD
int lcd_init(int color)
{
	//��һ������ʼ�� LCD
	int fd;
	fd = open("/dev/fb0", O_RDWR);
	if (fd < 0)
		return -1;
	
	//��lcd //ӳ�䵽���̵ĵ�ַ�ռ��У�����ַ�ռ���׵�ַ��///����plcd�У�plcd���ָ��ָ����ӳ��ĵ�ַ�ռ䣬///��ζ�ţ����ǲ���plcd���ָ����൱�ڲ���lcd����//lcd�ϻ��㣩
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
			//ÿһ��ռ���ٸ��ֽڣ�124 / 2
			pos = (124/2) * j + i/2;
			
			if (i % 2 == 0)
				gray = finger[pos] & 0x0f; //ȡ��4λ
			else
				gray = (finger[pos] >> 4) & 0x0f;
			
			gray <<= 4;
			
			//���һ��4�ֽڵ���ɫ
			color = (gray << 16) | (gray << 8) | gray;
			
			lcd_draw_point(i+x, j+y, color);
			lcd_draw_point(i+1+x, j+y, color);
			lcd_draw_point(i+x, j+1+y, color);
			lcd_draw_point(i+1+x, j+1+y, color);
		}
	}	
}