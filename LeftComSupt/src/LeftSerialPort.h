

#ifndef _MOXA_SUPPORT_H
#define _MOXA_SUPPORT_H

#include "LeftSPDef.h"

#ifdef _LEFT_SP_OS_LINUX
#include "mserial_port.h"
#endif


namespace leftCom {
#ifdef _LEFT_SP_OS_WIN
	LEFT_SILPT_ERROR moxaOpenAndInitCom(int bitlv, int com);
	int moxaWriteCom(char* buf, int len, int com);
	int moxaRead(int com, char *buf, unsigned int len);
	char *moxaReadComWithAnswer(char* buf, int com, int Len, char* Answer, int AnswerLen);
	LEFT_SILPT_ERROR moxaCloseCom(int com);
	int choseDefineBitlv(int bitlv);
#endif
#ifdef _LEFT_SP_OS_LINUX
	void set_speed(int fd, int speed);
	int set_Parity(int fd, int databits, int stopbits, int parity);
	int ComInit(char* comName, int bitlv);
#endif
}

#endif
