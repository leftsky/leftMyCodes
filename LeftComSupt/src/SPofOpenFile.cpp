
#include "LeftSerialPort.h"

/* Deal serial port with the way open file */
#ifdef _LEFT_SP_OS_LINUX

#include <sys/types.h>  

#include <sys/stat.h>  
#include <fcntl.h>  
#include <termios.h>  
#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include "LeftComSupport.h"

//#define BAUDRATE        B57600
//#define UART_DEVICE     "/dev/ttyM0"  

#define FALSE  -1  
#define TRUE   0  
////////////////////////////////////////////////////////////////////////////////  
/**
*@brief  ���ô���ͨ������
*@param  fd     ���� int  �򿪴��ڵ��ļ����
*@param  speed  ���� int  �����ٶ�
*@return  void
*/
int speed_arr[] = { B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300,
B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = { 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200,  300,
115200, 38400, 19200, 9600, 4800, 2400, 1200,  300, };

using namespace leftCom;

void leftCom::set_speed(int fd, int speed) {
	int   i;
	int   status;
	struct termios   Opt;
	tcgetattr(fd, &Opt);
	for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++) {
		if (speed == name_arr[i]) {
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if (status != 0) {
				perror("tcsetattr fd1");
				return;
			}
			tcflush(fd, TCIOFLUSH);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////  
/**
*@brief   ���ô�������λ��ֹͣλ��Ч��λ
*@param  fd     ����  int  �򿪵Ĵ����ļ����
*@param  databits ����  int ����λ   ȡֵ Ϊ 7 ����8
*@param  stopbits ����  int ֹͣλ   ȡֵΪ 1 ����2
*@param  parity  ����  int  Ч������ ȡֵΪN,E,O,,S
*/
int leftCom::set_Parity(int fd, int databits, int stopbits, int parity)
{
	struct termios options;
	if (tcgetattr(fd, &options) != 0) {
		perror("SetupSerial 1");
		return(FALSE);
	}
	options.c_cflag &= ~CSIZE;
	switch (databits) /*��������λ��*/
	{
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr, "Unsupported data size\n"); return (FALSE);
	}
	switch (parity)
	{
	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB); /* ����Ϊ��Ч��*/
		options.c_iflag |= INPCK;             /* Disnable parity checking */
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;     /* Enable parity */
		options.c_cflag &= ~PARODD;   /* ת��ΪżЧ��*/
		options.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
	case 'S':
	case 's':  /*as no parity*/
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB; break;
	default:
		fprintf(stderr, "Unsupported parity\n");
		return (FALSE);
	}
	/* ����ֹͣλ*/
	switch (stopbits)
	{
	case 1:
		options.c_cflag &= ~CSTOPB;
		break;
	case 2:
		options.c_cflag |= CSTOPB;
		break;
	default:
		fprintf(stderr, "Unsupported stop bits\n");
		return (FALSE);
	}
	/* Set input parity option */
	if (parity != 'n')
		options.c_iflag |= INPCK;
	tcflush(fd, TCIFLUSH);
	options.c_cc[VTIME] = 150; /* ���ó�ʱ15 seconds*/
	options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
	if (tcsetattr(fd, TCSANOW, &options) != 0)
	{
		perror("SetupSerial 3");
		return (FALSE);
	}
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
	options.c_oflag &= ~OPOST;   /*Output*/
	return (TRUE);
}
////////////////////////////////////////////////////////////////////////////////  
int leftCom::ComInit(char* comName, int bitlv) {

	int		fd, c = 0, res;
	char	buf[256];

	printf("Start...\n");
	fd = open(comName, O_RDWR);

	if (fd < 0) {
		perror(comName);
		exit(1);
	}

	printf("Open...\n");
	set_speed(fd, bitlv);
	if (set_Parity(fd, 8, 1, 'N') == FALSE) {
		printf("Set Parity Error\n");
		exit(0);
	}
	printf("Write...\n");
	while(1) {
		write(fd, "Hello world!", 12);
		usleep(100 * 1000);
		break;
	}


	printf("Reading...\n");
	while (1) {
		res = read(fd, buf, 255);

		if (res == 0)
			continue;
		else
			std::cout << "res: " << res << std::endl;
		//buf[res] = 0;

		for(int i = 0; i<res; i++)
			printf("%02X", buf[i]);
		printf("\n");

		//if (buf[0] == 0x0d)
		//	printf("\n");

		//if (buf[0] == '@') break;
	}

	//printf("Close...\n");
	//close(fd);

	return fd;
}


#endif
