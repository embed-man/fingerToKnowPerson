#include "serial.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

int init_serial()
{

	/*设置串口
	  波特率:19200
	  数据位:8
	  校验位:不要
	  停止位:1
	  数据流控制:无
	  */
#if 0
	struct termios
	{
		tcflag_t c_iflag;       /* input mode flags */
		tcflag_t c_oflag;       /* output mode flags */
		tcflag_t c_cflag;       /* control mode flags */
		tcflag_t c_lflag;       /* local mode flags */
		cc_t c_line;            /* line discipline */
		cc_t c_cc[NCCS];        /* control characters */
		speed_t c_ispeed;       /* input speed */
		speed_t c_ospeed;       /* output speed */
#define _HAVE_STRUCT_TERMIOS_C_ISPEED 1
#define _HAVE_STRUCT_TERMIOS_C_OSPEED 1
	};
#endif
	
	int finger_fd;
	 finger_fd= open("/dev/s3c2410_serial1", O_RDWR);//打开指纹模块设备文件
	if (finger_fd == -1)
	{
		perror("open error:");
		return -1;
	}

	struct termios myserial;
	//清空结构体
	memset(&myserial, 0, sizeof (myserial));
	//O_RDWR               
	myserial.c_cflag |= (CLOCAL | CREAD);
	//设置控制模式状态，本地连接，接受使能
	//设置 数据位
	myserial.c_cflag &= ~CSIZE;   //清空数据位
	myserial.c_cflag &= ~CRTSCTS; //无硬件流控制
	myserial.c_cflag |= CS8;      //数据位:8

	myserial.c_cflag &= ~CSTOPB;//   //1位停止位
	myserial.c_cflag &= ~PARENB;  //不要校验
	//myserial.c_iflag |= IGNPAR;   //不要校验
	//myserial.c_oflag = 0;  //输入模式
	//myserial.c_lflag = 0;  //不激活终端模式

	cfsetospeed(&myserial, B19200);  //设置波特率
	cfsetispeed(&myserial, B19200);

	/* 刷新输出队列,清楚正接受的数据 */
	tcflush(finger_fd, TCIFLUSH);

	/* 改变配置 */
	tcsetattr(finger_fd, TCSANOW, &myserial);

	return finger_fd;
}