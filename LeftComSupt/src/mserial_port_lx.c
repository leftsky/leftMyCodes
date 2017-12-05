/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.
*/
/*
    mserial_port.c

    Routines to operate serial ports

    2008-05-08	CF Lin
		new release
*/

#ifdef linux

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "mserial_port.h"
#include "os-support.h"

#define CMSPAR		010000000000

typedef struct _BAUDTAB
{
    int baudrate;
    int index;
} BAUDTAB;

/* do not change the order and cancel any one */
static BAUDTAB baudrate_table[] = {
	{ 0, B0 },
	{ 50, B50 },
	{ 75, B75 },
	{ 110, B110 },
	{ 134, B134 },
	{ 150, B150 },
	{ 200, B200 },
	{ 300, B300 },
	{ 600, B600 },
	{ 1200, B1200 },
	{ 1800, B1800 },
	{ 2400, B2400 },
	{ 4800, B4800 },
	{ 9600, B9600 }, 
	{ 19200, B19200 },
	{ 38400, B38400 },
	{ 57600, B57600 },
	{ 115200, B115200 },
	{ 230400, B230400 },
	{ 460800, B460800 },
	{ 500000, B500000 },
	{ 576000, B576000 },
	{ 921600, B921600 },
};

static int MAX_BAUDRATE_TABLE = (sizeof(baudrate_table)/sizeof(baudrate_table[0]));

/* set the baudrate on an open port
	Inputs:
		<fd> handler of the open port
		<baudrate> a specified baudrate
	Returns:
		0 on success, otherwise failure
*/
/*	baud rate setting (including the extented rates that are not included 
	in the speed table 
	<return> 0 on success, failure otherwise 
*/
int	
mxsp_set_baudrate ( unsigned int fd, int baudrate)
{
	struct termios termios;
	int i;

	/* check and setup configuration */
	if (tcgetattr(fd, &termios))
		return -1;
		
	/* check if the speed is one of the well-known baud rates */
	for (i=0; i < MAX_BAUDRATE_TABLE; i++)
	if (baudrate_table[i].baudrate == baudrate)
	{
		termios.c_cflag &= ~(CBAUD | CBAUDEX);
		termios.c_cflag |= baudrate_table[i].index;
		return tcsetattr(fd, TCSANOW, &termios)? -2:0;
	}
	/* a rate not one of them defined in the table */
	termios.c_cflag |= B4000000;
	return ioctl(fd, UC_SET_SPECIAL_BAUD_RATE, &baudrate)? -2:0;
}

/* get the baudrate on an open port
	Inputs:
		<fd> handler of the open port
	Returns:
		negative value on failure, otherwise the baudrate
*/
int	
mxsp_get_baudrate(unsigned int fd)
{
	struct termios termios;
	int i, speed, index;

	/* check and setup configuration */
	if (tcgetattr(fd, &termios))
		return -1;

	/* match one in the table */
	index = (termios.c_cflag & CBAUD);
	for (i=0; i < MAX_BAUDRATE_TABLE; i++)
	if (baudrate_table[i].index == index)
	{
	    return baudrate_table[i].baudrate;
	}

	if ( ioctl(fd, UC_GET_SPECIAL_BAUD_RATE, &speed) != 0 )
	{
		dbgprintf("extented baud rates are not supported");
		return -2;
	}
	return speed;
}

/* set the data bits on an open port
	Inputs:
		<fd> handler of the open port
		<bits> 5, 6, 7, or 8
	Returns:
		0 on success, otherwise failure
*/
int
mxsp_set_databits( unsigned int fd, int bits)
{
	struct termios	termios;

	if (tcgetattr(fd, &termios))
		return -1;

	termios.c_cflag &= ~CSIZE;
	if (bits==5) 		termios.c_cflag |= CS5;
	else if (bits==6)	termios.c_cflag |= CS6;
	else if (bits==7)	termios.c_cflag |= CS7;
	else			termios.c_cflag |= CS8;

	return tcsetattr(fd, TCSANOW, &termios)? -2:0;
}

/* get the data bits on an open port
	Inputs:
		<fd> handler of the open port
	Returns:
		5~8 on success, otherwise failure
*/
int
mxsp_get_databits( unsigned int fd )
{
	unsigned int flag;
	struct termios	termios;

	if (tcgetattr(fd, &termios))
		return -1;
	flag = termios.c_cflag & CSIZE;
	if (flag == CS8)		return 8;
	else if (flag == CS7)	return 7;
	else if (flag == CS6)	return 6;
	else if (flag == CS5)	return 5;
	else 	return -99;
}

/* set the # of stop bits on an open port
	Inputs:
		<fd> handler of the open port
		<bits> stop bits, 1, 2, or 3 (for 1.5)
	Returns:
		0 on success, otherwise failure
*/
int
mxsp_set_stopbits( unsigned int fd, int bits)
{
	struct termios	termios;

	if (tcgetattr( fd, &termios))
		return -1;

	if ( bits == 2 ) 
		termios.c_cflag |= CSTOPB;
	else
		termios.c_cflag &= ~CSTOPB;

	return tcsetattr(fd, TCSANOW, &termios)? -2:0;
}

/* get the # of stop bits of an open port
	Inputs:
		<fd> handler of the open port
	Returns:
		negative value on failure, or stop bits, 1, 2, and 3 (for 1.5)
*/
int
mxsp_get_stopbits( unsigned int fd)
{
	struct termios	termios;
	int		bits;

	/* check and setup configuration */
	if (tcgetattr(fd, &termios)==0)
		return -1;

	if (termios.c_cflag & CSTOPB)
		bits = 2;
	else
		bits = 1;

	return bits;
}

/* set the parity of an open port
	Inputs:
		<fd> handler of the open port
		<parity> parity code , 0:none, 1:odd, 2:even, 3:space, 4:mark, otherwise:none
	Returns:
		negative value on failure, or the parity code
*/
int
mxsp_set_parity( unsigned int fd, int parity)
{
	struct termios	termios;

	/* check and setup configuration */
	if (tcgetattr(fd, &termios))
		return -1;

	termios.c_cflag &= ~(PARENB|PARODD|CMSPAR);

	if (parity == MSP_PARITY_ODD)		termios.c_cflag |= (PARENB|PARODD);
	else if (parity == MSP_PARITY_EVEN)	termios.c_cflag |= PARENB;
	else if (parity == MSP_PARITY_SPACE)termios.c_cflag |= (PARENB|CMSPAR);
	else if (parity == MSP_PARITY_MARK)	termios.c_cflag |= (PARENB|CMSPAR|PARODD);

	return (tcsetattr(fd, TCSANOW, &termios)) ? -2 : 0;
}

/* get the parity of an open port
	Inputs:
		<fd> handler of the open port
	Returns:
		negative value on failure, or the parity code
*/
int
mxsp_get_parity( unsigned int fd)
{
	int	parity;
	struct termios	termios;

	/* check and setup configuration */
	if (tcgetattr(fd, &termios))
		return -1;

	if ( termios.c_cflag & PARENB ) 
	{
		if ( termios.c_cflag & CMSPAR ) 
			parity = ( termios.c_cflag & PARODD )? MSP_PARITY_MARK:MSP_PARITY_SPACE;
		else 
			parity = ( termios.c_cflag & PARODD )? MSP_PARITY_ODD:MSP_PARITY_EVEN;
	} 
	else
		parity = MSP_PARITY_NONE;
	return parity;
}

void
mxsp_set_nonblocking(unsigned int fd)
{
	fcntl( fd, F_SETFL, FNDELAY);
}

/* open a serial port by the port number, not the name 
	Inputs:
		<port> port number
                ***** added by Lock on 2011-06-25 ****
		<flag> 0: ttyM*       
                       1: ttyS*
                       2: console, ttyS1
	Returns:
		0 on failure, otherwise the handle of an open port
	Notes:
		the default communication setting: RS232/HW Flow Control/N/8/1/9600
*/
unsigned int
mxsp_open(int port,int flag)
{
	int	fd;
	char	ttyname[64];

	if(flag==0)
		sprintf(ttyname, "/dev/ttyM%d", port-1);
	else if (flag==1)
		sprintf(ttyname, "/dev/ttyS%d", port-1);
	else if (flag==2)
		sprintf(ttyname, "/dev/ttyS1");
	fd = open(ttyname, O_RDWR|O_NOCTTY);
	if ( fd <= 0 ) 
	{
		dbgprintf("fail to open serial port %s",ttyname);
		return 0;
	}
	else
	{
		struct termios	tmio;

		tcflush(fd, TCIOFLUSH);

		tcgetattr(fd, &tmio);
		tmio.c_lflag = 0;
		tmio.c_oflag = 0;
		tmio.c_iflag = 0;
		tmio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
		tmio.c_cc[VTIME] = 0;
		tmio.c_cc[VMIN] = 0;
		tcsetattr(fd, TCSANOW, &tmio);

		//mxsp_set_interface((unsigned int) fd, MSP_RS232_MODE);
		return (unsigned int) fd;
	}
}

/* byte to byte timeout  = ReadIntervalTimeout */
/* read operation timeout  = ReadTotalTimeoutConstant + ReadTotalTimeoutMultiplier x (bytes to read) */
void 
mxsp_set_timeout_read(unsigned int fd, int mSec)
{
	(void) fd;
	(void) mSec;
}

/* write operation timeout = WriteTotalTimeoutConstant + WriteTotalTimeoutMultiplier x (bytes to write) */
static void 
mxsp_set_timeout_write(unsigned int fd, int mSec)
{
	(void) fd;
	(void) mSec;
}

int
mxsp_write( unsigned int fd, char *buf, int len, void *dummy)
{
    (void) dummy;

    return write(fd, buf, len);
}

int
mxsp_read( unsigned int fd, char *buf, int len, void *dummy)
{
    (void) dummy;

    return read(fd, buf, len);
}

int 
mxsp_set_xonxoff( unsigned int fd ,char xon, char xoff)
{
	return 0;
}

int
mxsp_get_errors(unsigned int fd, unsigned int *err)
{

	return 0;
}

int	
mxsp_set_break(unsigned int fd)
{
	return (ioctl(fd, TIOCSBRK, NULL)? -1:0);
}

int	
mxsp_clear_break(unsigned int fd)
{
	return (ioctl(fd, TIOCCBRK, NULL)? -1:0);
}

#endif
