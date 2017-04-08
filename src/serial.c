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

	/*���ô���
	  ������:19200
	  ����λ:8
	  У��λ:��Ҫ
	  ֹͣλ:1
	  ����������:��
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
	 finger_fd= open("/dev/s3c2410_serial1", O_RDWR);//��ָ��ģ���豸�ļ�
	if (finger_fd == -1)
	{
		perror("open error:");
		return -1;
	}

	struct termios myserial;
	//��սṹ��
	memset(&myserial, 0, sizeof (myserial));
	//O_RDWR               
	myserial.c_cflag |= (CLOCAL | CREAD);
	//���ÿ���ģʽ״̬���������ӣ�����ʹ��
	//���� ����λ
	myserial.c_cflag &= ~CSIZE;   //�������λ
	myserial.c_cflag &= ~CRTSCTS; //��Ӳ��������
	myserial.c_cflag |= CS8;      //����λ:8

	myserial.c_cflag &= ~CSTOPB;//   //1λֹͣλ
	myserial.c_cflag &= ~PARENB;  //��ҪУ��
	//myserial.c_iflag |= IGNPAR;   //��ҪУ��
	//myserial.c_oflag = 0;  //����ģʽ
	//myserial.c_lflag = 0;  //�������ն�ģʽ

	cfsetospeed(&myserial, B19200);  //���ò�����
	cfsetispeed(&myserial, B19200);

	/* ˢ���������,��������ܵ����� */
	tcflush(finger_fd, TCIFLUSH);

	/* �ı����� */
	tcsetattr(finger_fd, TCSANOW, &myserial);

	return finger_fd;
}