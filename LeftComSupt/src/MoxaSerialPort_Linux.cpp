
#include "LeftSerialPort.h"

#ifdef _LEFT_SP_OS_LINUX

namespace leftSilPt {
	int left_SP_ioctl(int port, int baud, int Bit, int Stop, int parity) {
		if (mxsp_set_baudrate(port, baud) < 0) return -1;
		if (mxsp_set_parity(port, parity) < 0) return -1;
		if (mxsp_set_databits(port, Bit) < 0) return -1;
		if (mxsp_set_stopbits(port, Stop) < 0) return -1;
		return 0;
	}
}

#endif
