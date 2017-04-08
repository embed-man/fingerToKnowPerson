#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sql.h"

sqlite3 *sqlite3_init(const char *path)
{
	int ret;
	sqlite3 *db;
	
	//�������ߴ�һ�����ݿ��ļ�
	ret = sqlite3_open(path, &db);
	if (ret != SQLITE_OK)
	{
		printf("sqlite3_open error !\n");
		return 0;
	}
	
	return db;
}

//��ѧ���������Ϣ��ָ�Ƶ�����ֵ �洢�� sqlite3��
void sqlite3_add_user(sqlite3 *db, const unsigned char *name, unsigned short id, const unsigned char spec[])
{
	int ret;
	char sql_cmd[256];
	
	unsigned char tmp[193];
	memset(tmp,0, sizeof(tmp));
	strncpy(tmp, spec, 193);
	
	memset(sql_cmd, 0, sizeof(sql_cmd));
	sprintf(sql_cmd, "insert into student values('%s', %d, '%s')", name, id, tmp);
	
	ret = sqlite3_exec(db, sql_cmd, NULL, NULL, NULL);
	if (ret != SQLITE_OK)
	{
		printf("sqlite3_exec error !\n");
		return ;
	}
}