
#include "LeftSerialPort.h"
#ifdef _LEFT_SP_OS_WIN
#include <Windows.h>
#include <iostream>

#include "PCOMM.H"
#include "LeftMyCodes/MyCodes.h"

int leftCom::moxaWriteCom(char* buf, int len, int com) {
	if (buf == NULL && com <= 0) 
		return LEFT_SILPT_ARGVS;
	//std::cout << "Write com:: " << com << "\tbuf:: " << buf << std::endl;
	//char* Str = new char[strlen(buf)/2 + 1];
	//StrToHex(buf, Str, (int)(strlen(buf)/ 2 + 1));
	return sio_write(com, buf, len);
}

LEFT_SILPT_ERROR leftCom::moxaOpenAndInitCom(int bitlv, int com) {
	if (com <= 0 || com >= 15)
		return LEFT_SILPT_ARGVS;
	std::cout << "Init com:: " << com << "\tbitlv " << bitlv << std::endl;
	//return true;
	bitlv = leftCom::choseDefineBitlv(bitlv);
	if (bitlv == -1) 
		return LEFT_SILPT_ARGVS;
	//std::cout << "blv: " << bitlv << std::endl;
	int error = sio_open(com);
	if (error != SIO_OK)
		return LEFT_SILPT_FAILED;
	int mode = P_NONE | BIT_8 | STOP_1;
	error = sio_ioctl(com, bitlv, mode);
	if (error != SIO_OK) {
		sio_close(com);
		return LEFT_SILPT_FAILED;
	}
	return LEFT_SILPT_SUCCESS;
}

int leftCom::moxaRead(int com, char *buf, unsigned int len) {
	return sio_read(com, buf, len);
}

char* leftCom::moxaReadComWithAnswer(
	char* buf, int com, int Len, char* Answer, int AnswerLen) {
	if (buf == NULL || com <= 0 || com >= 15)
		return "left_error";
	//std::cout << "Read com:: " << com << std::endl;
	//return NULL;
	memset(buf, 0, Len);
	int len = sio_read(com, buf, Len);
	if (len > 0) {
		return leftName::HexToStr(buf, len, Answer, AnswerLen);
	}
	else return NULL;
}

LEFT_SILPT_ERROR leftCom::moxaCloseCom(int com) {
	std::cout << "Close com:: " << com << std::endl;
	//return true;
	return sio_close(com) == SIO_OK ? TRUE : FALSE;
}

int leftCom::choseDefineBitlv(int bitlv) {
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

#endif

