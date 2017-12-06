
#include "LeftSerialPort.h"

#ifdef _LEFT_SP_OS_WIN


namespace leftSilPt {

	int ChoseBaud(int bitlv) {
		switch (bitlv) {
		case 50: return 0;
		case 75: return 1;
		case 110: return 2;
		case 134: return 3;
		case 150: return 4;
		case 300: return 5;
		case 600: return 6;
		case 1200: return 7;
		case 1800: return 8;
		case 4800: return 0xA;
		case 7200: return 0xB;
		case 9600: return 0xC;
		case 19200: return 0xD;
		case 38400: return 0xE;
		case 57600: return 0xF;
		case 115200: return 0x10;
		case 230400: return 0x11;
		case 460800: return 0x12;
		case 921600: return 0x13;
		default: return -1;
		}
	}

}

#endif
