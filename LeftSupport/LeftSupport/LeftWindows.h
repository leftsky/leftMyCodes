/* 文件名:LeftWindows.h */

#ifndef LEFT_WINDOWS
#define LEFT_WINDOWS

#include <Windows.h>
#include <shellapi.h>

/* LeftWindows类内错误 */
#define LEFT_WINDOWS_ERROR -1

/* Ex扩展标志 */
#define LWEX_SIZECHANGE 0x00000001

/* 一些便捷的宏 */
#define WindowTop(Hwnd) SetWindowPos(Hwnd, HWND_TOPMOST, \
			NULL, NULL, NULL, NULL, SWP_NOSIZE|SWP_NOMOVE)
#define StartThread(Proc) CreateThread(NULL, NULL, \
			Proc, NULL, NULL, NULL);
#define GetSTCCursorPos(win, pt) GetCursorPos(pt);ScreenToClient(win, pt)
#define GetCTSCursorPos(win, pt) GetCursorPos(pt);ClientToScreen(win, pt)

#define SIMPLEMSG


typedef struct _CopyScreemStruct {
	HWND	inWindow;								// 被截图的窗口 若为NULL则是除输出窗口外的上一个活动窗口
	HWND	offWindow;								// 输出截图的窗口
	UINT	time;									// 刷新的等待时间 若非正则50毫秒
} CopyScrm, *pCopyScrm;

typedef struct _FATHER_WINDOW_STRUCT {
	HWND	Self;									// 自身的窗口句柄
	WCHAR*	WinName;								// 自身窗口名
	_FATHER_WINDOW_STRUCT *Head;					// 自身所处链表的链表头
	_FATHER_WINDOW_STRUCT *Tail;					// 自身所处链表的链表尾
	_FATHER_WINDOW_STRUCT *Last;					// 自身链表的上一个节点
	_FATHER_WINDOW_STRUCT *Next;					// 自身链表的下一个节点
	_FATHER_WINDOW_STRUCT *ParentHead;				// 父窗口结构体信息链表头
	_FATHER_WINDOW_STRUCT *ParentTail;				// 父窗口结构体信息链表尾
	_FATHER_WINDOW_STRUCT *ChildsHead;				// 子窗口结构体信息链表头
	_FATHER_WINDOW_STRUCT *ChildsTail;				// 子窗口结构体信息链表尾
} LeftWins, *pLeftWins;

class LeftWindows {
public:
	// 构造和析构
	LeftWindows(HINSTANCE hinstance, UINT ClsStyle, WCHAR* ClsName, 
		LRESULT CALLBACK ExWinProc(HWND, UINT, WPARAM, LPARAM));
	virtual ~LeftWindows();

	ATOM RegClass;
	PWNDCLASSEX pWndClsEx;
	pLeftWins ParentsWinsHead;
	// LeftWindows的基本初始化工作
	static void Init();
	// 获得一个默认的新分配堆内存的PWNDCLASSEX指针
	static PWNDCLASSEX GetDefWndClsEx();
	// 注销以前的注册类并根据新类名重新注册一个窗口类
	bool reRegClass(WCHAR* ClsName);
	// 创建一个父级窗口，并加入本自类的父窗口链表中
	HWND CreateParentsWin(DWORD ExStyle, WCHAR* WinName, DWORD Style);
	// 根据传入窗口句柄获得一个和窗口适配的HDC并通过ChangeOfBitmap可以改变其位图
	static HDC GetNewHdc(HWND window, 
		void ChangeOfBitmap(HWND window, DWORD* buf, DWORD Width, 
			DWORD Height, DWORD size));


	// 扩展Ex
	// 给无边框(WS_POPUP)的窗口修改位置和大小的线程,当然有边框也可以修改
	static DWORD _stdcall ExSizeChange(LPVOID win);
	// 根据传入标志打开所有指定需要的LeftWindowsEx线程
	DWORD PleaseExs(UINT ExCode, HWND hwnd);

	// 小工具Fun
	// 复制窗口的表面支持
	static bool CopyScreem(HWND inWindow, HWND offWindow, UINT time);
	// 输出当前鼠标激活时窗体的信息
	static DWORD _stdcall WindowsInfo(LPVOID nothing);
	// 输出传入消息的中文详解，有些被注释了
	static bool OutputWindowCallbackMacro(int Msg);

/* 非公开 不可修改 类自身维护的 */
private:
	/* 小工具FUN的内部支持 */
	// 复制窗口的内部支持
	static DWORD _stdcall CopyScreem(LPVOID CopyScreemStu);
	static HANDLE DeleteSureLock;

	static void DefHandDropFiles(HDROP hDrop);
	// 最基础即默认的窗口消息回调函数
	static LRESULT CALLBACK DefWinProc(
		_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);
	// 扩展但依然是预定义的窗口回调函数
	static LRESULT CALLBACK ExWinProc(
		_In_ HWND, _In_ UINT, _In_ WPARAM, _In_ LPARAM);
	static WCHAR DefClsName[];
	static bool staticInit;
};

class GraphicsAlgorithm {
public:
	// 构造和析构
	GraphicsAlgorithm();
	virtual ~GraphicsAlgorithm();
	void SetAlgorithmName(WCHAR* AlgorithmName);
	WCHAR* GetAlgorithmName();
/* 非公开 不可修改 类自身维护的 */
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