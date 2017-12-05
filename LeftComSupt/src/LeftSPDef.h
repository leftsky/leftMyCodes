
#ifndef _LEFT_SERIAL_PORT_DEFS_H_
#define _LEFT_SERIAL_PORT_DEFS_H_

#ifdef _WIN32
#define _LEFT_SP_OS_WIN
#endif
#ifdef __linux__
#define _LEFT_SP_OS_LINUX					1991
#endif

#define LEFT_SILPT_ERROR					unsigned long
#define LEFT_SILPT_SUCCESS					0x00000000L
#define LEFT_SILPT_FAILED					-0x00000001L
#define LEFT_SILPT_ARGVS					0xFFFF0001L

#endif
