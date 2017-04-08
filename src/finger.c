#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "finger.h"
#include "lcd.h"

enum err_no
{
	SUCCESS = 0,
	FAIL,
	FULL,
	NO_USER,
	USER_OPD,
	FIN_OPD,
	TIME_OUT
};

char *err_msg[] = 
{
	"success",
	"fail",
	"NULL",
	"NULL",
	"full",
	"no user",
	"user opd",
	"finger opd",
	"time out"
};

//计算校验
static int check_sum(unsigned char pack[], unsigned short len)
{
	unsigned char chk = 0;
	int i;
	
	for (i = 0; i < len ; i++)
	{
		chk = chk ^ pack[i];
	}

	return chk;
}

unsigned char check_sum1(unsigned char pack[], int len)
{
	int i; 
	unsigned char chk;
	for (i = 0; i < len; i++)
	{
		chk = chk ^ pack[i];
	}
	
	return chk;
}

//向dsp发送命令
int cmd_send(unsigned char cmd[], int len, int finger_fd)
{
	int r;
	int sum = 0;
	
	while (sum < len)
	{
		r = write(finger_fd, cmd + sum, len - sum);
		if (r > 0)
		{
			sum += r;
		}
	}
}

//接收 dsp回复的数据
void cmd_resp(int finger_fd, unsigned char resp[], int len)
{
	int r;
	int total_read = 0;
	while (total_read < len)
	{
		r = read(finger_fd, resp + total_read, len - total_read);
		if (r > 0)
		{
			total_read += r;
			
		}else if (r < 0)
		{
			return ;
		}
	}
}

//处理添加指纹的命令
int handle_add_cmd(int finger_fd, unsigned char cmd_no, unsigned short id)
{
	unsigned char cmd[8];
	unsigned char resp[8];
	int i = 0;
	
	cmd[i++] = 0xF5;
	cmd[i++] = cmd_no;
	cmd[i++] = (id >> 8) & 0xFF;
	cmd[i++] = id & 0xFF;
	cmd[i++] = 3;
	cmd[i++] = 0;
	
	cmd[i] = check_sum(&cmd[1], i-1);
	cmd[++i] = 0xF5;
	
	//发包
	cmd_send(cmd, 8, finger_fd);
	printf("plz press your finger\n");
	
	cmd_resp(finger_fd, resp, 8);
	
	for (i = 0; i < 8; i++)
		printf("%x ", resp[i]);
	printf("\n");
	
	if (resp[4] != 0)
	{
		printf("### add finger error: %s ! ###\n", err_msg[resp[4]]);
	}
	
	return resp[4];
}

//添加指纹
int add_finger(unsigned short id, int finger_fd)
{
	//组包
	int ret;
	
	ret = handle_add_cmd(finger_fd, FCMD_ADD_FINGER1, id);
	if (ret != SUCCESS)
	{
		return -1;
	}
	
	handle_add_cmd(finger_fd, FCMD_ADD_FINGER2, id);
	if (ret != SUCCESS)
	{
		return -1;
	}
	
	handle_add_cmd(finger_fd, FCMD_ADD_FINGER3, id);
	if (ret != SUCCESS)
	{
		return -1;
	}

	printf("Add user: %d success !\n", id);
	
	return 0;
}

//删除指定的 用户ID 的指纹
void delete_spec_user(int finger_fd, unsigned short id)
{
	unsigned char cmd[8];
	unsigned char resp[8];
	int i = 0;
	
	cmd[i++] = 0xF5;
	cmd[i++] = DEL_SPEC_USER;
	cmd[i++] = (id >> 8) & 0xFF;
	cmd[i++] = id & 0xFF;
	cmd[i++] = 0;
	cmd[i++] = 0;
	
	cmd[i] = check_sum(&cmd[1], i-1);
	cmd[++i] = 0xF5;
	
	cmd_send(cmd, 8, finger_fd);
	
	cmd_resp(finger_fd, resp, 8);
	
	for (i = 0; i < 8; i++)
		printf("%x ", resp[i]);
	printf("\n");
	
	if (resp[4] != 0)
	{
		printf("### delete user error: %s ! ###\n", err_msg[resp[4]]);
	}
	else 
		printf("dele user: %d success !\n", id);
}

//删除所有的用户指纹
void delete_all_user(int finger_fd)
{
	unsigned char cmd[8];
	unsigned char resp[8];
	int i = 0;
	
	cmd[i++] = 0xF5;
	cmd[i++] = DEL_ALL_USERS;
	cmd[i++] = 0;
	cmd[i++] = 0;
	cmd[i++] = 0;
	cmd[i++] = 0;
	
	cmd[i] = check_sum(&cmd[1], i-1);
	cmd[++i] = 0xF5;
	
	cmd_send(cmd, 8, finger_fd);
	
	cmd_resp(finger_fd, resp, 8);
	
	for (i = 0; i < 8; i++)
		printf("%x ", resp[i]);
	printf("\n");
	
	if (resp[4] != 0)
	{
		printf("### delete user error: %s ! ###\n", err_msg[resp[4]]);
	}
	else
	{
		printf("delete all users success !\n");
	}
}

//获得 指纹的图像
int get_finger_pic(int finger_fd)
{
	unsigned char cmd[8];
	unsigned char resp[9179];
	int i = 0;
	
	cmd[i++] = 0xF5;
	cmd[i++] = GET_FINGER_PIC;
	cmd[i++] = 0;
	cmd[i++] = 0;
	cmd[i++] = 0;
	cmd[i++] = 0;
	
	cmd[i] = check_sum(&cmd[1], i-1);
	cmd[++i] = 0xF5;
	
	cmd_send(cmd, 8, finger_fd);
	
	cmd_resp(finger_fd, resp, 8);
	
	for (i = 0; i < 8; i++)
		printf("%x ", resp[i]);
	printf("\n");
	
	if (resp[4] != 0)
	{
		printf("### get finger error: %s ! ###\n", err_msg[resp[4]]);
	}
	
	cmd_resp(finger_fd, resp, 9179);
	//void lcd_draw_rect(int x0, int y0, int w, int //h, int color)
	//lcd_draw_rect(0, 0, 248, 296, 0xFFFFFFFF);
	
	lcd_draw_finger(resp+1, 340, 310);
}

//获取指纹的特征值
void get_finger_spec(int finger_fd, unsigned char spec[])
{
	unsigned char cmd[8];
	unsigned char resp[8];
	int i = 0;
	
	cmd[i++] = 0xF5;
	cmd[i++] = GET_FINGER_SPEC;
	cmd[i++] = 0;
	cmd[i++] = 0;
	cmd[i++] = 0;
	cmd[i++] = 0;
	
	cmd[i] = check_sum(&cmd[1], i-1);
	cmd[++i] = 0xF5;
	
	cmd_send(cmd, 8, finger_fd);
	
	cmd_resp(finger_fd, resp, 8);
	
	for (i = 0; i < 8; i++)
		printf("%x ", resp[i]);
	printf("\n");
	
	if (resp[4] != 0)
	{
		printf("### get finger spec error: %s ! ###\n", err_msg[resp[4]]);
	}
	
	cmd_resp(finger_fd, spec, 199);
	printf("finger spec array: \n");
	for (i = 0; i < 199; i++)
		printf("%x ", spec[i]);
	printf("\n");
}

//比较特征值
int cmp_spec(int finger_fd, unsigned char spec[])
{
	unsigned char cmd[199];
	unsigned char resp[8];
	int i = 0;
	
	cmd[i++] = 0xF5;
	cmd[i++] = 0x44;
	cmd[i++] = 0;
	cmd[i++] = 0xC4;
	cmd[i++] = 0;
	cmd[i++] = 0;
	
	cmd[i] = check_sum(&cmd[1], i-1);
	cmd[++i] = 0xF5;
	
	cmd_send(cmd, 8, finger_fd);
	
	cmd[0] = 0xF5;
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = 0;
	
	for (i = 0; i < 193; i++)
	{
		cmd[4+i] = spec[i];
	}
	
	cmd[197] = check_sum(&cmd[1], 196);
	cmd[198] = 0xF5;
	cmd_send(cmd, 199, finger_fd);
	
	cmd_resp(finger_fd, resp, 8);
	
	for (i = 0; i < 8; i++)
		printf("%x ", resp[i]);
	printf("\n");
	
	return resp[4];
}

//通过指纹获取对于的学生的相关信息
STU get_id_from_finger(int finger_fd, sqlite3 *db)
{
	//遍历数据库中的所有的特征值 //将特征值下发给指纹模块与采集的指纹进行比较
	// 查询database中内容
	int nRow, nColumn;
	int ret;
	char **dbResult = NULL;
	STU stu;
	
	ret = sqlite3_get_table(db, "select * from student;", &dbResult, &nRow, &nColumn, NULL);
	if (ret != SQLITE_OK)
	{
		printf("sqlite3_get_table error !\n");
		return stu;
	}
	
	printf("nRow: %d, nColumn: %d\n", nRow, nColumn);
	
	if (nRow <= 0)
	{
		printf("NO info ..\n");
		return stu;
	}
	
	int index, i, j;
	index = nColumn;
	
	for (i = 0; i < nRow; i++)
	{	
		for (j = 0; j < nColumn; j++)
		{
			if (strcmp("spec", dbResult[j]) == 0)
			{
				
				memset(stu.spec, 0, sizeof(stu.spec));
				strcpy(stu.spec, dbResult[index]);
			
				printf("cmp: %s %s\n", stu.name, stu.id);
				ret = cmp_spec(finger_fd, stu.spec);
				if (ret == 0)
				{
					printf("**** get the right id: %s***\n", stu.id);
					sqlite3_free_table(dbResult);
					return stu;
				}
			}
			else if (strcmp("name", dbResult[j]) == 0)
			{
				memset(stu.name, 0, sizeof(stu.name));
				strcpy(stu.name, dbResult[index]);
			}
			else if (strcmp("id", dbResult[j]) == 0)
			{
				memset(stu.id, 0, sizeof(stu.id));
				strcpy(stu.id, dbResult[index]);
			}
			index++;
		}
	}
	
	sqlite3_free_table(dbResult);
}