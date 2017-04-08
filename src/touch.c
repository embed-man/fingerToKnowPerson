#include "touch.h"
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int touch_init()
{
	int fd;
	
	fd = open("/dev/event0", O_RDWR);
	if (fd < 0)
	{
		perror("open error !\n");
		return -1;
	}
	
	return fd;
}

int get_touch_task(int fd)
{
	struct input_event ev;
	
	int ret;
	
	int x, y;
	
	while (1)
	{
		ret = read(fd, &ev, sizeof(ev));
		if (ret != sizeof(ev))
			continue;
		
		//判断是否是 触摸事件
		if ((ev.type == EV_ABS) && (ev.code == ABS_X))
		{
			x = ev.value;
			
		}
		else if ((ev.type == EV_ABS) && (ev.code == ABS_Y))
		{
			y = ev.value;
		}
		
		if ((ev.type == EV_ABS) && (ev.code == ABS_PRESSURE) && (ev.value == 0))
		{
			//手指已经弹起
			break;
		}
	}

	printf("x: %d\n", x);
	printf("y: %d\n", y);
	
	//如果是添加指纹
	if (x>200 && x<300 && y>380 && y<480)
	{
		return ADD_FINGER;
	}
	else if (x>500 && x<600 && y>380 && y<480) //如果是获取指纹
	{
		return GET_FINGER;
	}
}