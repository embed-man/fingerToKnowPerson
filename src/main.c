/*
	��LCD����ʾ һ��ָ������Ӧ�������Ϣ������ ѧ�ţ�
	
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
	//��������
	//playmusic();
	
	//��һ������ʼ�� LCD
	int ret;
	
	ret = lcd_init(0xFFFFFFFF);
	if (ret < 0)
	{
		printf("lcd_init error ...\n");
		return 0;
	}
	
	//�ڶ�����LCD�Ĳ���
	lcd_design();
	
	//����������ʼ�� sqlite���ݿ�
	sqlite3 *db;
	db = sqlite3_init(DB_PATH);
	
	//���Ĳ�����ʼ������
	int finger_fd;
	finger_fd = init_serial();
	if (finger_fd < 0)
	{
		printf("init_serial error\n");
		return 0;
	}
	
	//���岽����ʼ�� ������
	int touch_fd;
	touch_fd = touch_init();
	if (touch_fd < 0)
	{
		printf("touch_init error\n");
		return 0;
	}
	
	//�������������� ��������ֵ �ж�Ҫ���������
	int task;
	unsigned short id;
	
	//ɾ�����ܣ�
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
			//ɾ�����ݿ��ж�Ӧ������
			//delete * from student where id = xx
		}
		else if (func_no == 2)
		{	
			delete_all_user(finger_fd);	
			//ɾ�����ݿ������е�����
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
				
				//��ȡָ�Ƶ�ͼ��
				get_finger_pic(finger_fd);
				
				//���ָ��
				int ret = add_finger(id, finger_fd);
				if (ret < 0)
					continue;
				
				//��ȡָ�Ƶ�����ֵ
				memset(spec, 0, sizeof(spec));
				get_finger_spec(finger_fd, spec);
				//����Ϣ��ӵ�sqlite3��
				sqlite3_add_user(db, name, id, spec+4);
				break;
			case GET_FINGER:
				get_finger_pic(finger_fd);
				//��ȡָ�Ƶ�����ֵ(�����ݿ���)
				STU stu;
				stu = get_id_from_finger(finger_fd, db);
				
				// ���� stu�ṹ���ڵ�������ID //��ʾ��LCD��Ӧ��λ����
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