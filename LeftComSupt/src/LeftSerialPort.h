

#ifndef _MOXA_SUPPORT_H
#define _MOXA_SUPPORT_H

#include "LeftSPDef.h"

#ifdef _LEFT_SP_OS_LINUX
#include "mserial_port.h"
#endif


namespace leftSilPt {
#ifdef _LEFT_SP_OS_WIN
#include <Windows.h>
#include "PCOMM.H"
#define LeftSPIoctl(port, baud, Bit, Stop, P)	\
						sio_ioctl(port, baud, Bit|Stop|P)
#define LeftSPOpen(port)				sio_open(port)
#define LeftSPClose(port)				sio_close(port)
#define LeftSPWrite(port, buf, len)		sio_write(port, buf, len)
#define LeftSPRead(port, buf, len)		sio_read(port, buf, len)
	int ChoseBaud(int Baud);
#endif
#ifdef _LEFT_SP_OS_LINUX
#include "mserial_port.h"

/*	MODE setting		*/
#define BIT_5		5				/* Word length define	*/
#define BIT_6		6
#define BIT_7		7
#define BIT_8		8

#define STOP_1		1				/* Stop bits define	*/
#define STOP_2		2

#define P_EVEN		MSP_PARITY_EVEN
#define P_ODD		MSP_PARITY_ODD
#define P_SPC		MSP_PARITY_SPACE
#define P_MRK		MSP_PARITY_MARK
#define P_NONE		0x00

#define LeftSPIoctl(port, baud, Bit, Stop, P)	\
						left_SP_ioctl(port, baud, Bit, Stop, P)
#define LeftSPOpen(port)				mxsp_open(port, 0)
#define LeftSPClose(port)				mxsp_close(port)
#define LeftSPWrite(port, buf, len)		mxsp_write(port, buf, len, NULL)
#define LeftSPRead(port, buf, len)		mxsp_read(port, buf, len, NULL)
#define ChoseBaud(baud)					baud

	int left_SP_ioctl(int port, int baud, int Bit, int Stop, int parity);
	int set_Parity(int fd, int databits, int stopbits, int parity);
	int ComInit(char* comName, int bitlv);
	void set_speed(int fd, int speed);


#endif
}

#endif
