/*
	在LCD上显示 一个指纹所对应的相关信息（姓名 学号）
	
	arm-linux-gcc main.c lcd.c sql.c serial.c touch.c finger.c -I /home/gec/sqlite/include -I ../include -L /home/gec/sqlite/lib -lsqlite3 -o touch
	
	arm-linux-gcc main.c lcd.c sql.c serial.c touch.c finger.c -I ../lib/sqlite/include -I ../include -L ../lib/sqlite/lib -lsqlite3 -o touch
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "lcd.h"
#include "sql.h"
#include "serial.h"
#include "touch.h"
#include "finger.h"
#include "number.h"
#include "mp3.h"
#define DB_PATH "./1.db"

int main(int argc, char *argv[])
{
	//播放音乐
	//playmusic();
	
	//第一步：初始化 LCD
	int ret;
	
	ret = lcd_init(0xFFFFFFFF);
	if (ret < 0)
	{
		printf("lcd_init error ...\n");
		return 0;
	}
	
	//第二步：LCD的布局
	lcd_design();
	
	//第三步：初始化 sqlite数据库
	sqlite3 *db;
	db = sqlite3_init(DB_PATH);
	
	//第四步：初始化串口
	int finger_fd;
	finger_fd = init_serial();
	if (finger_fd < 0)
	{
		printf("init_serial error\n");
		return 0;
	}
	
	//第五步：初始化 触摸屏
	int touch_fd;
	touch_fd = touch_init();
	if (touch_fd < 0)
	{
		printf("touch_init error\n");
		return 0;
	}
	
	//捕获触摸屏的坐标 根据坐标值 判断要处理的任务
	int task;
	unsigned short id;
	
	//删除功能：
	while (1)
	{   printf("\t");
         printf("\t");
          printf("\t");
		printf("## plz input func no 1/2/3 ##\n");
		printf("\t");
         printf("\t");
          printf("\t");
		printf("## 1: Delete user          ##\n");
		printf("\t");
         printf("\t");
          printf("\t");
		printf("## 2: Delete all users     ##\n");
		printf("\t");
         printf("\t");
          printf("\t");
		printf("## 3: Default              ##\n");
		
		int func_no;
		
		scanf("%d", &func_no);
		if (func_no == 1)
		{
			printf("plz input user ID to be deleted: ");
			scanf("%d", &id);
			delete_spec_user(finger_fd, id);
			//删除数据库中对应的数据
			//delete * from student where id = xx
		}
		else if (func_no == 2)
		{	
			delete_all_user(finger_fd);	
			//删除数据库中所有的数据
			//delete from student;
		}
		else 
		{
			break;
		}
	}
	
	unsigned char name[32];
	unsigned char spec[199];
	int i;
	while (1)
	{
		task = get_touch_task(touch_fd);
		switch (task)
		{
			case ADD_FINGER:
				printf("*** add finger ***\n");
				printf("plz input name and ID(0x0 ~ 0xFFF): example(zhangsan 101)\n");
				memset(name, 0, sizeof(name));
				scanf("%s %d", name, &id);
				
				//获取指纹的图像
				get_finger_pic(finger_fd);
				
				//添加指纹
				int ret = add_finger(id, finger_fd);
				if (ret < 0)
					continue;
				
				//获取指纹的特征值
				memset(spec, 0, sizeof(spec));
				get_finger_spec(finger_fd, spec);
				//将信息添加到sqlite3中
				sqlite3_add_user(db, name, id, spec+4);
				break;
			case GET_FINGER:
				get_finger_pic(finger_fd);
				//获取指纹的特征值(从数据库中)
				STU stu;
				stu = get_id_from_finger(finger_fd, db);
				
				// 根据 stu结构体内的姓名和ID //显示在LCD对应的位置上
				//lcd_draw_rect(240, 160, 220, 170, 0xffffffff);
				for (i = 0; i < strlen(stu.name); i++)
				{
					lcd_draw_word1( mod_letter[stu.name[i]-97],0x00ff0000,144,24,(110+i*24),150);
				}
				
				for (i = 0; i < strlen(stu.id); i++)
				{
					lcd_draw_word1( mod_dig[stu.id[i]-48],0x00ff0000,144,24,(110+i*24),250);
				}

				//lcd_huoqurebound();
				
				break;
			default:
				break;
		}
	}
	
	return 0;
}