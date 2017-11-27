/* �ļ���:LeftWindows.h */

#ifndef LEFT_WINDOWS
#define LEFT_WINDOWS

#include <Windows.h>
#include <shellapi.h>

/* LeftWindows���ڴ��� */
#define LEFT_WINDOWS_ERROR -1

/* Ex��չ��־ */
#define LWEX_SIZECHANGE 0x00000001

/* һЩ��ݵĺ� */
#define WindowTop(Hwnd) SetWindowPos(Hwnd, HWND_TOPMOST, \
			NULL, NULL, NULL, NULL, SWP_NOSIZE|SWP_NOMOVE)
#define StartThread(Proc) CreateThread(NULL, NULL, \
			Proc, NULL, NULL, NULL);
#define GetSTCCursorPos(win, pt) GetCursorPos(pt);ScreenToClient(win, pt)
#define GetCTSCursorPos(win, pt) GetCursorPos(pt);ClientToScreen(win, pt)

#define SIMPLEMSG


typedef struct _CopyScreemStruct {
	HWND	inWindow;								// ����ͼ�Ĵ��� ��ΪNULL���ǳ�������������һ�������
	HWND	offWindow;								// �����ͼ�Ĵ���
	UINT	time;									// ˢ�µĵȴ�ʱ�� ��������50����
} CopyScrm, *pCopyScrm;

typedef struct _FATHER_WINDOW_STRUCT {
	HWND	Self;									// ����Ĵ��ھ��
	WCHAR*	WinName;								// ��������
	_FATHER_WINDOW_STRUCT *Head;					// �����������������ͷ
	_FATHER_WINDOW_STRUCT *Tail;					// �����������������β
	_FATHER_WINDOW_STRUCT *Last;					// �����������һ���ڵ�
	_FATHER_WINDOW_STRUCT *Next;					// �����������һ���ڵ�
	_FATHER_WINDOW_STRUCT *ParentHead;				// �����ڽṹ����Ϣ����ͷ
	_FATHER_WINDOW_STRUCT *ParentTail;				// �����ڽṹ����Ϣ����β
	_FATHER_WINDOW_STRUCT *ChildsHead;				// �Ӵ��ڽṹ����Ϣ����ͷ
	_FATHER_WINDOW_STRUCT *ChildsTail;				// �Ӵ��ڽṹ����Ϣ����β
} LeftWins, *pLeftWins;

class LeftWindows {
public:
	// ���������
	LeftWindows(HINSTANCE hinstance, UINT ClsStyle, WCHAR* ClsName, 
		LRESULT CALLBACK ExWinProc(HWND, UINT, WPARAM, LPARAM));
	virtual ~LeftWindows();

	ATOM RegClass;
	PWNDCLASSEX pWndClsEx;
	pLeftWins ParentsWinsHead;
	// LeftWindows�Ļ�����ʼ������
	static void Init();
	// ���һ��Ĭ�ϵ��·�����ڴ��PWNDCLASSEXָ��
	static PWNDCLASSEX GetDefWndClsEx();
	// ע����ǰ��ע���ಢ��������������ע��һ��������
	bool reRegClass(WCHAR* ClsName);
	// ����һ���������ڣ������뱾����ĸ�����������
	HWND CreateParentsWin(DWORD ExStyle, WCHAR* WinName, DWORD Style);
	// ���ݴ��봰�ھ�����һ���ʹ��������HDC��ͨ��ChangeOfBitmap���Ըı���λͼ
	static HDC GetNewHdc(HWND window, 
		void ChangeOfBitmap(HWND window, DWORD* buf, DWORD Width, 
			DWORD Height, DWORD size));


	// ��չEx
	// ���ޱ߿�(WS_POPUP)�Ĵ����޸�λ�úʹ�С���߳�,��Ȼ�б߿�Ҳ�����޸�
	static DWORD _stdcall ExSizeChange(LPVOID win);
	// ���ݴ����־������ָ����Ҫ��LeftWindowsEx�߳�
	DWORD PleaseExs(UINT ExCode, HWND hwnd);

	// С����Fun
	// ���ƴ��ڵı���֧��
	static bool CopyScreem(HWND inWindow, HWND offWindow, UINT time);
	// �����ǰ��꼤��ʱ�������Ϣ
	static DWORD _stdcall WindowsInfo(LPVOID nothing);
	// ���������Ϣ��������⣬��Щ��ע����
	static bool OutputWindowCallbackMacro(int Msg);

/* �ǹ��� �����޸� ������ά���� */
private:
	/* С����FUN���ڲ�֧�� */
	// ���ƴ��ڵ��ڲ�֧��
	static DWORD _stdcall CopyScreem(LPVOID CopyScreemStu);
	static HANDLE DeleteSureLock;

	static void DefHandDropFiles(HDROP hDrop);
	// �������Ĭ�ϵĴ�����Ϣ�ص�����
	static LRESULT CALLBACK DefWinProc(
		_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);
	// ��չ����Ȼ��Ԥ����Ĵ��ڻص�����
	static LRESULT CALLBACK ExWinProc(
		_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);
	static WCHAR DefClsName[];
	static bool staticInit;
};

class GraphicsAlgorithm {
public:
	// ���������
	GraphicsAlgorithm();
	virtual ~GraphicsAlgorithm();
	void SetAlgorithmName(WCHAR* AlgorithmName);
	WCHAR* GetAlgorithmName();
/* �ǹ��� �����޸� ������ά���� */
private:
	WCHAR *AlgorithmName;
};

class CircularChart:private GraphicsAlgorithm {
public:
	CircularChart();
	virtual ~CircularChart();
	WCHAR* GetGAName();
private:
};

#endif