
#ifndef MESSAGE_FORMAT
#define MESSAGE_FORMAT

typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned int UINT;

/* =========================================================================== */
/* Ethernet Ⅱ 帧格式 */
typedef struct _Ethernet2Struct{
	/* 目标MAC地址 */
	BYTE DstMAC[6];
	/* 源MAC地址 */
	BYTE SrcMAC[6];
	/* 以太帧的长度或者协议类型 */
	BYTE Ptcol_Len[2];
	/* 数据和填充 46~1500 
	如果帧小于64字节则需要填充至64 */
	BYTE* Data;
	/* 帧校验序列FCS */
	BYTE FCS[4];
} Ethernet2, *pEthernet2;

#endif
/* Type/Length所定义的协议宏类型 */
#define ETH2_PTCOL_IP 0x0800
#define ETH2_PTCOL_ARP 0x0806
#define ETH2_PTCOL_RARP 0x8035
#define ETH2_PTCOL_PPPOE_discovery_stage 0x8863
#define ETH2_PTCOL_PPPOE_session_stage 0x8864
#define ETH2_VLAN

/* =========================================================================== */
/* MTU */
/*
超通道					65535
16Mb/s令牌环(IBM)		17914
4Mb/s令牌环(IEEE 802.5)	4464
FDDI					4352
以太网					1500
IEEE 802.3/802/2		1492
X.25					576
点对点(低延时)			296
*/
/* =========================================================================== */
/* UDP和TCP的上层协议与常见应用 */
/*
UDP:	RIP  广播、多播  TFTP(S:69)  BOOTP(S:67 D:68)  SNMP(Admin:162 Proxy:161)
TCP:	FTP(21)  TELNET(23)  Rlogin  SMTP
*/
/* =========================================================================== */
/* PPP 数据包格式 */
typedef struct _PPPStruct {
	/* 标志7E */
	BYTE Start;
	/* 地址FF */
	BYTE Address;
	/* 控制03 */
	BYTE Control;
	/* 协议 */
	BYTE Protocol[2];
	/* 数据 */
	BYTE* msg;
	/* 校验 */
	BYTE CRC[2];
	/* 标志7E */
	BYTE End;
} PPP, *pPPP;
/* PPP协议字段宏 */
#define PPP_TYPE_IP 0x0021
#define PPP_TYPE_IPCP 0x8021
#define PPP_TYPE_LCP 0xC021
#define PPP_TYPE_PAP 0xC023
#define PPP_TYPE_LQR 0xC025
#define PPP_TYPE_CHAP 0xC223
#define PPP_TYPE_OSICP 0x8023
#define PPP_TYPE_OSI 0x0023

/* =========================================================================== */
/* ARP报文格式(RFC826)
   ARP报文前加入以太帧首部 */
typedef struct _ARP_ETHERNET2_STRUCT {
	/* 硬件类型 */
	BYTE HardWareType[2];
	/* 协议类型 */
	BYTE Protocol[2];
	/* 硬件地址长度 */
	BYTE HardWareAddressLen;
	/* 协议地址长度 */
	BYTE PtcolLen;
	/* 操作类型 */
	BYTE AskType[2];
	/* 发送端硬件地址 */
	BYTE srcMAC[6];
	/* 发送端IP地址*/
	BYTE srcIP[6];
	/* 目的硬件地址 */
	BYTE dstMAC[6];
	/* 目的IP地址 */
	BYTE dstIP[6];
} ETH_ARP, *pETH_ARP;

/* =========================================================================== */
/* IP报文格式(RFC791) 20byte */
typedef struct _IP_ETHERNET2_STRUCT {
	/* 版本 */
	BYTE Version : 4;
	/* 首部长度 */
	BYTE HeadLen : 4;
	/* 服务类型TOS */
	BYTE TOS;
	/* 总长度 */
	BYTE Len[2];
	/* 标识Ident */
	BYTE Ident[2];
	/* 标志 */
	USHORT Flag : 3;
	/* 片偏移 */
	USHORT Off : 13;
	/* TTL */
	BYTE TTL;
	/* 协议号 */
	BYTE Protocol;
	/* 首部校验和 */
	BYTE Check[2];
	/* 源IP地址 */
	BYTE SrcIP[4];
	/* 目的IP地址 */
	BYTE dstIP[4];
} ETH_IP, *pETH_IP;

/* =========================================================================== */
/* ICMP ping报文格式 8byte */
typedef struct _PINGICMP_IP_ETHERNET2_STRUCT {
	/* 类型 0或8 */
	BYTE Type;
	/* 代码0 */
	BYTE Code;
	/* 校验和 */
	BYTE Check[2];
	/* 标识符Identi */
	BYTE Identi[2];
	/* 序号 */
	BYTE Ordinal[2];
} PING, *pPING;

/* =========================================================================== */
/* TCP报文格式（需要IP封装） 20byte */
typedef struct _TCP_IP_ETHERNET2_STRUCT {
	/* 源端口号 */
	BYTE SrcHost[2];
	/* 目的端口好 */
	BYTE DstHost[2];
	/* 序列号 */
	BYTE Ordinal[4];
	/* 确认序列号 */
	BYTE CheckOrdinal[4];
	/* 首部长度 */
	BYTE Len : 4;
	/* 保留 */
	BYTE Hold : 6;
	BYTE URG : 1;
	BYTE ACK : 1;
	BYTE PSH : 1;
	BYTE RST : 1;
	BYTE SYN : 1;
	BYTE FIN : 1;
	/* 窗口大小 */
	BYTE FromSize[2];
	/* 校验和 */
	BYTE Check[2];
	/* 紧急指针 */
	BYTE EmergencyPointer[2];
} TCP, *pTCP;

/* =========================================================================== */
/* UDP报文格式（需IP封装） 8byte */
typedef struct _UDP_IP_ETHERNET2_STRUCT {
	/* 源端口 */
	BYTE SrcHost[2];
	/* 目的端口 */
	BYTE DstHOst[2];
	/* UDP长度 */
	BYTE Len[2];
	/* UDP校验和 */
	BYTE Check[2]
} UDP, *pUDP;

/* =========================================================================== */
/* MPLS报文格式 
	二层头+MPLS标签（可以有多个）+IP报文 */
typedef struct _MPLS_UNKNOW_STRUCT {
	/* Label value 连接属性 */
	UINT LabelValue : 20;
	/* EXP */
	UINT EXP : 3;
	/* 栈底标记 */
	UINT Sign : 1;
	/* TTL超时 */
	BYTE TTL;
} MPLS, *pMPLS;

/* =========================================================================== */