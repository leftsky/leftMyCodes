
#ifndef MESSAGE_FORMAT
#define MESSAGE_FORMAT

typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned int UINT;

/* =========================================================================== */
/* Ethernet �� ֡��ʽ */
typedef struct _Ethernet2Struct{
	/* Ŀ��MAC��ַ */
	BYTE DstMAC[6];
	/* ԴMAC��ַ */
	BYTE SrcMAC[6];
	/* ��̫֡�ĳ��Ȼ���Э������ */
	BYTE Ptcol_Len[2];
	/* ���ݺ���� 46~1500 
	���֡С��64�ֽ�����Ҫ�����64 */
	BYTE* Data;
	/* ֡У������FCS */
	BYTE FCS[4];
} Ethernet2, *pEthernet2;

#endif
/* Type/Length�������Э������� */
#define ETH2_PTCOL_IP 0x0800
#define ETH2_PTCOL_ARP 0x0806
#define ETH2_PTCOL_RARP 0x8035
#define ETH2_PTCOL_PPPOE_discovery_stage 0x8863
#define ETH2_PTCOL_PPPOE_session_stage 0x8864
#define ETH2_VLAN

/* =========================================================================== */
/* MTU */
/*
��ͨ��					65535
16Mb/s���ƻ�(IBM)		17914
4Mb/s���ƻ�(IEEE 802.5)	4464
FDDI					4352
��̫��					1500
IEEE 802.3/802/2		1492
X.25					576
��Ե�(����ʱ)			296
*/
/* =========================================================================== */
/* UDP��TCP���ϲ�Э���볣��Ӧ�� */
/*
UDP:	RIP  �㲥���ಥ  TFTP(S:69)  BOOTP(S:67 D:68)  SNMP(Admin:162 Proxy:161)
TCP:	FTP(21)  TELNET(23)  Rlogin  SMTP
*/
/* =========================================================================== */
/* PPP ���ݰ���ʽ */
typedef struct _PPPStruct {
	/* ��־7E */
	BYTE Start;
	/* ��ַFF */
	BYTE Address;
	/* ����03 */
	BYTE Control;
	/* Э�� */
	BYTE Protocol[2];
	/* ���� */
	BYTE* msg;
	/* У�� */
	BYTE CRC[2];
	/* ��־7E */
	BYTE End;
} PPP, *pPPP;
/* PPPЭ���ֶκ� */
#define PPP_TYPE_IP 0x0021
#define PPP_TYPE_IPCP 0x8021
#define PPP_TYPE_LCP 0xC021
#define PPP_TYPE_PAP 0xC023
#define PPP_TYPE_LQR 0xC025
#define PPP_TYPE_CHAP 0xC223
#define PPP_TYPE_OSICP 0x8023
#define PPP_TYPE_OSI 0x0023

/* =========================================================================== */
/* ARP���ĸ�ʽ(RFC826)
   ARP����ǰ������̫֡�ײ� */
typedef struct _ARP_ETHERNET2_STRUCT {
	/* Ӳ������ */
	BYTE HardWareType[2];
	/* Э������ */
	BYTE Protocol[2];
	/* Ӳ����ַ���� */
	BYTE HardWareAddressLen;
	/* Э���ַ���� */
	BYTE PtcolLen;
	/* �������� */
	BYTE AskType[2];
	/* ���Ͷ�Ӳ����ַ */
	BYTE srcMAC[6];
	/* ���Ͷ�IP��ַ*/
	BYTE srcIP[6];
	/* Ŀ��Ӳ����ַ */
	BYTE dstMAC[6];
	/* Ŀ��IP��ַ */
	BYTE dstIP[6];
} ETH_ARP, *pETH_ARP;

/* =========================================================================== */
/* IP���ĸ�ʽ(RFC791) 20byte */
typedef struct _IP_ETHERNET2_STRUCT {
	/* �汾 */
	BYTE Version : 4;
	/* �ײ����� */
	BYTE HeadLen : 4;
	/* ��������TOS */
	BYTE TOS;
	/* �ܳ��� */
	BYTE Len[2];
	/* ��ʶIdent */
	BYTE Ident[2];
	/* ��־ */
	USHORT Flag : 3;
	/* Ƭƫ�� */
	USHORT Off : 13;
	/* TTL */
	BYTE TTL;
	/* Э��� */
	BYTE Protocol;
	/* �ײ�У��� */
	BYTE Check[2];
	/* ԴIP��ַ */
	BYTE SrcIP[4];
	/* Ŀ��IP��ַ */
	BYTE dstIP[4];
} ETH_IP, *pETH_IP;

/* =========================================================================== */
/* ICMP ping���ĸ�ʽ 8byte */
typedef struct _PINGICMP_IP_ETHERNET2_STRUCT {
	/* ���� 0��8 */
	BYTE Type;
	/* ����0 */
	BYTE Code;
	/* У��� */
	BYTE Check[2];
	/* ��ʶ��Identi */
	BYTE Identi[2];
	/* ��� */
	BYTE Ordinal[2];
} PING, *pPING;

/* =========================================================================== */
/* TCP���ĸ�ʽ����ҪIP��װ�� 20byte */
typedef struct _TCP_IP_ETHERNET2_STRUCT {
	/* Դ�˿ں� */
	BYTE SrcHost[2];
	/* Ŀ�Ķ˿ں� */
	BYTE DstHost[2];
	/* ���к� */
	BYTE Ordinal[4];
	/* ȷ�����к� */
	BYTE CheckOrdinal[4];
	/* �ײ����� */
	BYTE Len : 4;
	/* ���� */
	BYTE Hold : 6;
	BYTE URG : 1;
	BYTE ACK : 1;
	BYTE PSH : 1;
	BYTE RST : 1;
	BYTE SYN : 1;
	BYTE FIN : 1;
	/* ���ڴ�С */
	BYTE FromSize[2];
	/* У��� */
	BYTE Check[2];
	/* ����ָ�� */
	BYTE EmergencyPointer[2];
} TCP, *pTCP;

/* =========================================================================== */
/* UDP���ĸ�ʽ����IP��װ�� 8byte */
typedef struct _UDP_IP_ETHERNET2_STRUCT {
	/* Դ�˿� */
	BYTE SrcHost[2];
	/* Ŀ�Ķ˿� */
	BYTE DstHOst[2];
	/* UDP���� */
	BYTE Len[2];
	/* UDPУ��� */
	BYTE Check[2]
} UDP, *pUDP;

/* =========================================================================== */
/* MPLS���ĸ�ʽ 
	����ͷ+MPLS��ǩ�������ж����+IP���� */
typedef struct _MPLS_UNKNOW_STRUCT {
	/* Label value �������� */
	UINT LabelValue : 20;
	/* EXP */
	UINT EXP : 3;
	/* ջ�ױ�� */
	UINT Sign : 1;
	/* TTL��ʱ */
	BYTE TTL;
} MPLS, *pMPLS;

/* =========================================================================== */