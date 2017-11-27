/* 文件名：LeftWidnows.cpp */



#include "LeftWindows.h"

#include <shlobj.h>
#include <iostream>



// 构造
LeftWindows::LeftWindows(HINSTANCE hInstance, UINT ClsStyle, WCHAR* ClsName,
	LRESULT CALLBACK ExWinProc(HWND, UINT, WPARAM, LPARAM)) {
	/* 初始化自身类 */
	if (!LeftWindows::staticInit)
		Init();
	pWndClsEx = LeftWindows::GetDefWndClsEx();
	pWndClsEx->hInstance = hInstance;
	pWndClsEx->style |= ClsStyle;
	pWndClsEx->lpszClassName = ClsName ? ClsName : LeftWindows::DefClsName;
	pWndClsEx->lpfnWndProc = ExWinProc ? ExWinProc : LeftWindows::ExWinProc;
	RegClass = RegisterClassEx(pWndClsEx);

	/* 初始化父窗体链表 */
	LeftWins *Head = new LeftWins;
	LeftWins *Tail = new LeftWins;
	memset(Head, 0, sizeof(LeftWins));
	memset(Tail, 0, sizeof(LeftWins));
	Head->Head = Head;
	Head->Tail = Tail;
	Head->WinName = L"ParentHead";
	Head->Next = Tail;
	Tail->Head = Head;
	Tail->Tail = Tail;
	Tail->WinName = L"ParentTail";
	Tail->Last = Head;
	ParentsWinsHead = Head;

}

// 析构
LeftWindows::~LeftWindows() {
	delete[] pWndClsEx;
}

// 初始化静态变量
WCHAR LeftWindows::DefClsName[] = L"default";
bool LeftWindows::staticInit = false;
/* 表曾支持与内部delete的同步锁 */
HANDLE LeftWindows::DeleteSureLock = CreateEvent(NULL, FALSE, TRUE, NULL);

// LeftWindows的基本初始化工作
void LeftWindows::Init() {
	setlocale(LC_ALL, "");
	LeftWindows::staticInit = true;
}

/*在ExWinProc中演示拖放事件*/
void LeftWindows::DefHandDropFiles(HDROP hDrop) {
	UINT FileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);
	std::cout << "收到的拖放文件总数: " << FileCount << std::endl;
	WCHAR buf[1991];
	for (UINT i = 0; i < FileCount; i++) {
		if (DragQueryFile(hDrop, i, (LPTSTR)buf, 1990 * sizeof(WCHAR)) > 1990) {
			std::cout << "文件名过长" << std::endl;
			return;
		}
		std::wcout << i + 1 << ": " << (WCHAR*)buf << std::endl;
	}
}

PWNDCLASSEX LeftWindows::GetDefWndClsEx() {
	PWNDCLASSEX wndClsEx = new WNDCLASSEX;
	wndClsEx->cbSize = sizeof(WNDCLASSEX);
	wndClsEx->style = CS_DBLCLKS | CS_OWNDC | CS_SAVEBITS;
	wndClsEx->lpfnWndProc = LeftWindows::DefWinProc;
	wndClsEx->cbClsExtra = 0;
	wndClsEx->cbWndExtra = 0;
	wndClsEx->hInstance = NULL;
	wndClsEx->hIcon = NULL; 
	wndClsEx->hCursor =  LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	wndClsEx->hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndClsEx->lpszMenuName = NULL;
	wndClsEx->lpszClassName = NULL;
	wndClsEx->hIconSm = NULL;
	return wndClsEx;
}

bool LeftWindows::reRegClass(WCHAR* ClsName) {
	UnregisterClass(pWndClsEx->lpszClassName, pWndClsEx->hInstance);
	pWndClsEx->lpszClassName = ClsName ? ClsName : pWndClsEx->lpszClassName;
	RegClass = RegisterClassEx(pWndClsEx);
	return true;
}

HWND LeftWindows::CreateParentsWin(DWORD ExStyle, WCHAR* WinName, DWORD Style) {
	HWND hwnd = CreateWindowEx(ExStyle, (LPCWSTR)RegClass,
		WinName, Style,
		200, 200, 400, 400, NULL, NULL, pWndClsEx->hInstance, NULL);
	if (!hwnd)
		return NULL;
	LeftWins *Head = new LeftWins;
	LeftWins *Tail = new LeftWins;
	LeftWins *Parents = new LeftWins;
	memset(Head, 0, sizeof(LeftWins));
	memset(Tail, 0, sizeof(LeftWins));
	memset(Parents, 0, sizeof(LeftWins));
	Head->Head = Head;
	Head->Tail = Tail;
	Head->Next = Tail;
	Tail->Head = Head;
	Tail->Tail = Tail;
	Tail->Last = Head;
	Parents->Self = hwnd;
	Parents->WinName = WinName;
	Parents->Head = ParentsWinsHead;
	Parents->Tail = ParentsWinsHead->Tail;
	Parents->Last = ParentsWinsHead->Tail->Last;
	Parents->Next = ParentsWinsHead->Tail;
	Parents->ChildsHead = Head;
	Parents->ChildsTail = Tail;
	ParentsWinsHead->Tail->Last->Next = Parents;
	ParentsWinsHead->Tail->Last = Parents;
	UpdateWindow(hwnd);
	return hwnd;
}


void ChangeOfBitmap(HWND window, DWORD* buf, DWORD Width, DWORD Height, DWORD size) {}

HDC LeftWindows::GetNewHdc(HWND window, void ChangeOfBitmap(
	HWND window, DWORD* buf, DWORD Width, DWORD Height, DWORD size)) {
	if (!window)
		return NULL;
	HDC WinDC = GetDC(window);

	RECT rect;
	GetWindowRect(window, &rect);
	DWORD Width = rect.right - rect.left;
	DWORD Height = rect.bottom - rect.top;
	DWORD size = Width*Height;
	DWORD *buf = new DWORD[size];

	HDC change = CreateCompatibleDC(WinDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(WinDC, Width, Height);
	SelectObject(change, hBitmap);
	StretchBlt(change, 0, 0, Width, Height, WinDC, 0, 0, Width, Height, SRCCOPY);

	GetBitmapBits(hBitmap, size * sizeof(DWORD), buf);
	ChangeOfBitmap(window, buf, Width, Height, size);
	SetBitmapBits(hBitmap, size * sizeof(DWORD), buf);
	SelectObject(change, hBitmap);

	DeleteObject(hBitmap);
	DeleteDC(WinDC);
	delete[] buf;
	return change;
}

HBITMAP GetSrcBit(HDC hDC, DWORD BitWidth, DWORD BitHeight) {

	HDC hBufDC;
	HBITMAP hBitmap, hBitTemp;
	//创建设备上下文(HDC)
	hBufDC = CreateCompatibleDC(hDC);
	//创建HBITMAP
	hBitmap = CreateCompatibleBitmap(hDC, BitWidth, BitHeight);
	hBitTemp = (HBITMAP)SelectObject(hBufDC, hBitmap);
	//得到位图缓冲区
	StretchBlt(hBufDC, 0, 0, BitWidth, BitHeight,
		hDC, 0, 0, BitWidth, BitHeight, SRCCOPY);
	//得到最终的位图信息
	hBitmap = (HBITMAP)SelectObject(hBufDC, hBitTemp);
	//释放内存
	DeleteObject(hBitTemp);
	::DeleteDC(hBufDC);
	return hBitmap;
}

LRESULT CALLBACK LeftWindows::DefWinProc(
	_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
};

/* LeftWindowsEx -------------------------------------------------------*/
LRESULT CALLBACK LeftWindows::ExWinProc(
	_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	OutputWindowCallbackMacro(uMsg);
	RECT rect;
	GetWindowRect(hwnd, &rect);
	LONG Width = rect.right - rect.left;
	LONG Height = rect.bottom - rect.top;
	switch (uMsg) {
	case WM_PAINT: {
		DWORD size = Width*Height;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		HDC change = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, Width, Height);
		DWORD *buf = new DWORD[size];
		//GetBitmapBits(hBitmap, size * sizeof(DWORD), buf);
		for (size_t i = 0; i < size; i++)
			buf[i] = 0xFF4040;
		SetBitmapBits(hBitmap, size * sizeof(DWORD), buf);
		SelectObject(change, hBitmap);
		StretchBlt(hdc, 0, 0, Width, Height, change, 0, 0, Width, Height, SRCCOPY);
		delete[] buf;
		DeleteObject(hBitmap);
		DeleteDC(change);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_ERASEBKGND:
		break;
	case WM_CTLCOLORBTN:
		return (LRESULT)GetStockObject(RGB(0, 0, 255));
	case WM_DROPFILES:						// 文件拖放消息
		std::cout << "收到文件拖放消息" << std::endl;
		LeftWindows::DefHandDropFiles((HDROP)wParam);
		DragFinish((HDROP)wParam);
		break;
	case WM_DESTROY:						// 销毁窗口消息
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
};

DWORD _stdcall LeftWindows::ExSizeChange(LPVOID win) {
	if (win == NULL)
		return (DWORD)LEFT_WINDOWS_ERROR;
	HWND window = (HWND)win;
	RECT rect;
	POINT pt;
	bool fget1 = true;
	bool fget2 = true;
	for (;;) {
		Sleep(50);
		if (GetForegroundWindow() != win)
			continue;
		while (GetAsyncKeyState(1) & 0x8000) {
			Sleep(5);
			if (fget1) {
				fget1 = false;
				GetWindowRect(window, &rect);
				GetCursorPos(&pt);
				ScreenToClient(window, &pt);
				if (pt.x<0 || pt.y<0 || pt.x>rect.right - rect.left 
					|| pt.y>rect.bottom - rect.top)
					break;
			}
			POINT rt;
			GetCursorPos(&rt);
			ScreenToClient(window, &rt);
			if (rt.x == pt.x&&rt.y == pt.y)
				continue;
			rect.left += rt.x - pt.x;
			rect.right += rt.x - pt.x;
			rect.top += rt.y - pt.y;
			rect.bottom += rt.y - pt.y;
			SetWindowPos(window, NULL, rect.left, rect.top, NULL, NULL, SWP_NOSIZE);
		}
		fget1 = true;
		while (GetAsyncKeyState(2) & 0x8000) {
			Sleep(5);
			if (fget2) {
				fget2 = false;
				GetWindowRect(window, &rect);
				GetCursorPos(&pt);
				ScreenToClient(window, &pt);
				if (pt.x<0 || pt.y<0 || pt.x>rect.right - rect.left 
					|| pt.y>rect.bottom - rect.top)
					break;
			}
			POINT rt;
			GetCursorPos(&rt);
			ScreenToClient(window, &rt);
			if (rt.x == pt.x&&rt.y == pt.y)
				continue;
			rect.right += rt.x - pt.x;
			rect.bottom += rt.y - pt.y;
			SetWindowPos(window, NULL, NULL, NULL,
				(rect.right - rect.left), (rect.bottom - rect.top), SWP_NOMOVE);
			pt.x = rt.x;
			pt.y = rt.y;
		}
		fget2 = true;
	}
}

bool LeftWindows::CopyScreem(HWND inWindow, HWND offWindow, UINT time) {
	if (!offWindow)
		return false;
	WaitForSingleObject(LeftWindows::DeleteSureLock, INFINITE);
	CopyScrm *cp = new CopyScrm;
	//memset(cp, 0, sizeof(CopyScrm));
	cp->time = time <= 0 ? 50 : time;
	cp->inWindow = inWindow;
	cp->offWindow = offWindow;
	CreateThread(NULL, NULL, LeftWindows::CopyScreem, cp, NULL, NULL);
	return true;
}

DWORD LeftWindows::PleaseExs(UINT ExCode, HWND hwnd) {
	if (!hwnd)
		return (DWORD)LEFT_WINDOWS_ERROR;
	if (ExCode & LWEX_SIZECHANGE)
		CreateThread(NULL, NULL, LeftWindows::ExSizeChange, (LPVOID)hwnd, NULL, NULL);
	return 0;
}

DWORD _stdcall  LeftWindows::CopyScreem(LPVOID CopyScreemStu) {
	pCopyScrm pCSstu = (pCopyScrm)CopyScreemStu;
	HWND offHwnd = pCSstu->offWindow;
	HWND inHwnd = pCSstu->inWindow;
	int time = pCSstu->time;
	delete[] CopyScreemStu;
	SetEvent(LeftWindows::DeleteSureLock);

	HDC offHwndDC = GetDC(offHwnd);
	HDC inHwndDC = inHwnd ? GetDC(inHwnd) : NULL;
	HWND FgrndWin = NULL;
	RECT rect;
	GetWindowRect(inHwnd, &rect);
	for (;;) {
		if (!inHwnd) {
			FgrndWin = GetForegroundWindow();
			if (FgrndWin && FgrndWin != offHwnd) {
				DeleteDC(inHwndDC);
				inHwndDC = GetDC(FgrndWin);
				GetWindowRect(FgrndWin, &rect);
			}
		}
		BitBlt(offHwndDC, 0, 0, rect.right - rect.left,
			rect.bottom - rect.top, inHwndDC, 0, 0, SRCCOPY);
		Sleep(time);
	}
}

DWORD _stdcall  LeftWindows::WindowsInfo(LPVOID nothing) {
	HWND Win = NULL;
	HWND FgrndWin = NULL;
	WCHAR name[512];
	int Style = 0;
	for (;;) {
		FgrndWin = GetForegroundWindow();
		if (FgrndWin == Win) {
			Sleep(50);
			continue;
		}
		GetClassName(FgrndWin, name, 512);
		std::wcout << "ClassName: " << name << std::endl;
		std::cout << "Window Style: ";
		Win = FindWindow(name, NULL);
		Style = GetWindowLong(Win, GWL_STYLE);
		if (Style & 0x00000000L) std::cout << " | WS_OVERLAPPED";
		if (Style & 0x80000000L) std::cout << " | WS_POPUP";
		if (Style & 0x40000000L) std::cout << " | WS_CHILD";
		if (Style & 0x20000000L) std::cout << " | WS_MINIMIZE";
		if (Style & 0x10000000L) std::cout << " | WS_VISIBLE";
		if (Style & 0x08000000L) std::cout << " | WS_DISABLED";
		if (Style & 0x04000000L) std::cout << " | WS_CLIPSIBLINGS";
		if (Style & 0x02000000L) std::cout << " | WS_CLIPCHILDREN";
		if (Style & 0x01000000L) std::cout << " | WS_MAXIMIZE";
		if (Style & 0x00C00000L) std::cout << " | WS_CAPTION";
		if (Style & 0x00800000L) std::cout << " | WS_BORDER";
		if (Style & 0x00400000L) std::cout << " | WS_DLGFRAME";
		if (Style & 0x00200000L) std::cout << " | WS_VSCROLL";
		if (Style & 0x00100000L) std::cout << " | WS_HSCROLL";
		if (Style & 0x00080000L) std::cout << " | WS_SYSMENU";
		if (Style & 0x00040000L) std::cout << " | WS_THICKFRAME";
		if (Style & 0x00020000L) std::cout << " | WS_GROUP";
		if (Style & 0x00010000L) std::cout << " | WS_TABSTOP";
		if (Style & 0x00020000L) std::cout << " | WS_MINIMIZEBOX";
		if (Style & 0x00010000L) std::cout << " | WS_MAXIMIZEBOX";
		std::cout << std::endl;

		std::cout << "Window ExStyle: ";
		Style = GetWindowLong(Win, GWL_EXSTYLE);
		if (Style & 0x00000001L) std::cout << " | WS_EX_DLGMODALFRAME";
		if (Style & 0x00000004L) std::cout << " | WS_EX_NOPARENTNOTIFY";
		if (Style & 0x00000008L) std::cout << " | WS_EX_TOPMOST";
		if (Style & 0x00000010L) std::cout << " | WS_EX_ACCEPTFILES";
		if (Style & 0x00000020L) std::cout << " | WS_EX_TRANSPARENT";
		if (Style & 0x00000040L) std::cout << " | WS_EX_MDICHILD";
		if (Style & 0x00000080L) std::cout << " | WS_EX_TOOLWINDOW";
		if (Style & 0x00000100L) std::cout << " | WS_EX_WINDOWEDGE";
		if (Style & 0x00000200L) std::cout << " | WS_EX_CLIENTEDGE";
		if (Style & 0x00000400L) std::cout << " | WS_EX_CONTEXTHELP";
		if (Style & 0x00001000L) std::cout << " | WS_EX_RIGHT";
		if (Style & 0x00000000L) std::cout << " | WS_EX_LEFT";
		if (Style & 0x00002000L) std::cout << " | WS_EX_RTLREADING";
		if (Style & 0x00000000L) std::cout << " | WS_EX_LTRREADING";
		if (Style & 0x00004000L) std::cout << " | WS_EX_LEFTSCROLLBAR";
		if (Style & 0x00000000L) std::cout << " | WS_EX_RIGHTSCROLLBAR";
		if (Style & 0x00010000L) std::cout << " | WS_EX_CONTROLPARENT";
		if (Style & 0x00020000L) std::cout << " | WS_EX_STATICEDGE";
		if (Style & 0x00040000L) std::cout << " | WS_EX_APPWINDOW";
		if (Style & 0x00080000)  std::cout << " | WS_EX_LAYERED";
		if (Style & 0x00100000L) std::cout << " | WS_EX_NOINHERITLAYOUT";
		if (Style & 0x00200000L) std::cout << " | WS_EX_NOREDIRECTIONBITMAP";
		if (Style & 0x00400000L) std::cout << " | WS_EX_LAYOUTRTL";
		if (Style & 0x02000000L) std::cout << " | WS_EX_COMPOSITED";
		if (Style & 0x08000000L) std::cout << " | WS_EX_NOACTIVATE";
		std::cout << std::endl;

		std::cout << "Style: ";
		Style = GetClassLong(Win, GCL_STYLE);
		if (Style & 0x0001) std::cout << " | CS_VREDRAW";
		if (Style & 0x0002) std::cout << " | CS_HREDRAW";
		if (Style & 0x0008) std::cout << " | CS_DBLCLKS";
		if (Style & 0x0020) std::cout << " | CS_OWNDC";
		if (Style & 0x0040) std::cout << " | CS_CLASSDC";
		if (Style & 0x0080) std::cout << " | CS_PARENTDC";
		if (Style & 0x0200) std::cout << " | CS_NOCLOSE";
		if (Style & 0x0800) std::cout << " | CS_SAVEBITS";
		if (Style & 0x1000) std::cout << " | CS_BYTEALIGNCLIENT";
		if (Style & 0x2000) std::cout << " | CS_BYTEALIGNWINDOW";
		if (Style & 0x4000) std::cout << " | CS_GLOBALCLASS";
		if (Style & 0x00010000) std::cout << " | CS_IME";
		if (Style & 0x00020000) std::cout << " | CS_DROPSHADOW";
		std::cout << std::endl;

		std::cout << std::endl;
		
	}
}

bool LeftWindows::OutputWindowCallbackMacro(int Msg) {
	switch (Msg) {
	case WM_NULL: std::cout << "空消息,可检测程序是否有响应等" << std::endl; return true;
	case WM_CREATE: std::cout << "新建一个窗口" << std::endl; return true;
	case WM_DESTROY: std::cout << "销毁一个窗口" << std::endl; return true;

#ifndef SIMPLEMSG
	case WM_MOVE: std::cout << "移动一个窗口" << std::endl; return true;
	case WM_SIZE: std::cout << "改变一个窗口的大小" << std::endl; return true;
#endif
	case WM_ACTIVATE: std::cout << "一个窗口被激活或失去激活状态" << std::endl; return true;
	case WM_SETFOCUS: std::cout << "将焦点转向一个窗口" << std::endl; return true;
	case WM_KILLFOCUS: std::cout << "使一个窗口失去焦点" << std::endl; return true;

	case WM_ENABLE: std::cout << "使一个窗口处于可用状态" << std::endl; return true;
	case WM_SETREDRAW: std::cout << "设置窗口是否能重绘" << std::endl; return true;
	case WM_SETTEXT: std::cout << "设置一个窗口的文本" << std::endl; return true;
	case WM_GETTEXT: std::cout << "复制窗口的文本到缓冲区" << std::endl; return true;
	case WM_GETTEXTLENGTH: std::cout << "得到窗口的文本长度(不含结束符)" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_PAINT: std::cout << "窗口重绘" << std::endl; return true;
#endif
	case WM_CLOSE: std::cout << "用户关闭窗口时会发送本消息,紧接着会发送WM_DESTROY消息" << std::endl; return true;
	case WM_QUERYENDSESSION: std::cout << "关机或注销时系统会按优先级给各进程发送WM_QUERYENDSESSION,告诉应用程序要关机或注销了" << std::endl; return true;
	case WM_QUIT: std::cout << "关闭消息循环结束程序的运行" << std::endl; return true;
	case WM_QUERYOPEN: std::cout << "最小化的窗口即将被恢复以前的大小位置" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_ERASEBKGND: std::cout << "当一个窗口的背景必须被擦除时本消息会被触发(如:窗口大小改变时)" << std::endl; return true;
#endif
	case WM_SYSCOLORCHANGE: std::cout << "当系统颜色改变时,发送本消息给所有顶级窗口" << std::endl; return true;
	case WM_ENDSESSION: std::cout << "关机或注销时系统会发出WM_QUERYENDSESSION消息,然后将本消息发送给应用程序,通知程序会话结束" << std::endl; return true;

	case WM_SHOWWINDOW: std::cout << "发送本消息给一个窗口,以便隐藏或显示该窗口" << std::endl; return true;

	case WM_WININICHANGE: std::cout << "读写\"win.ini\"时会发送本消息给所有顶层窗口,通知其它进程该文件已被更改" << std::endl; return true;
	case WM_DEVMODECHANGE: std::cout << "改变设备模式设置(\"win.ini\")时,处理本消息的应用程序可重新初始化它们的设备模式设置" << std::endl; return true;
	case WM_ACTIVATEAPP: std::cout << "窗口进程激活状态改动,正被激活的窗口属于不同的应用程序 " << std::endl; return true;
	case WM_FONTCHANGE: std::cout << "当系统的字体资源库变化时发送本消息给所有顶级窗口" << std::endl; return true;
	case WM_TIMECHANGE: std::cout << "当系统的时间变化时发送本消息给所有顶级窗口" << std::endl; return true;
	case WM_CANCELMODE: std::cout << "发送本消息来取消某种正在进行的模态(操作)(如鼠示捕获),例如:启动一个模态窗口时,父窗会收到本消息;该消息无参数" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_SETCURSOR: std::cout << "若鼠标光标在某窗口内移动且鼠标没被捕获时,就会发送本消息给某个窗口" << std::endl; return true;
#endif
	case WM_MOUSEACTIVATE: std::cout << "当鼠标光标在某个未激活窗口内,而用户正按着鼠标的某个键时,会发送本消息给当前窗口" << std::endl; return true;
	case WM_CHILDACTIVATE: std::cout << "点击窗口标题栏或当窗口被激活、移动、大小改变时,会发送本消息给MDI子窗口" << std::endl; return true;
	case WM_QUEUESYNC: std::cout << "本消息由基于计算机的训练程序发送,通过WH_JOURNALPALYBACK的HOOK程序分离出用户输入消息" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_GETMINMAXINFO: std::cout << "当窗口将要改变大小或位置时,由系统发送本消息给窗口,用户拖动一个可重置大小的窗口时便会发出本消息" << std::endl; return true;
#endif

	case WM_PAINTICON: std::cout << "当一个最小化的窗口图标将被重绘时发送本消息" << std::endl; return true;
	case WM_ICONERASEBKGND: std::cout << "本消息发送给某个最小化的窗口,仅当它在画图标前它的背景必须被重画" << std::endl; return true;
	case WM_NEXTDLGCTL: std::cout << "发送本消息给一个对话框程序窗口过程,以便在各控件间设置键盘焦点位置" << std::endl; return true;

	case WM_SPOOLERSTATUS: std::cout << "每当打印管理列队增加或减少一条作业时就会发出本消息" << std::endl; return true;
	case WM_DRAWITEM: std::cout << "按钮、组合框、列表框、菜单的外观改变时会发送本消息给这些控件的所有者" << std::endl; return true;
	case WM_MEASUREITEM: std::cout << "按钮、组合框、列表框、列表控件、菜单项被创建时会发送本消息给这些控件的所有者" << std::endl; return true;
	case WM_DELETEITEM: std::cout << "当列表框或组合框被销毁或通过LB_DELETESTRING、LB_RESETCONTENT、CB_DELETESTRING或CB_RESETCONTENT消息删除某些项时,会发送本消息给这些控件的所有者" << std::endl; return true;
	case WM_VKEYTOITEM: std::cout << "LBS_WANTKEYBOARDINPUT风格的列表框会发出本消息给其所有者,以便响应WM_KEYDOWN消息" << std::endl; return true;
	case WM_CHARTOITEM: std::cout << "LBS_WANTKEYBOARDINPUT风格的列表框会发送本消息给其所有者,以便响应WM_CHAR消息" << std::endl; return true;
	case WM_SETFONT: std::cout << "指定控件所用字体" << std::endl; return true;
	case WM_GETFONT: std::cout << "得到当前控件绘制其文本所用的字体" << std::endl; return true;
	case WM_SETHOTKEY: std::cout << "为某窗口关联一个热键" << std::endl; return true;
	case WM_GETHOTKEY: std::cout << "确定某热键与某窗口是否相关联" << std::endl; return true;



	case WM_QUERYDRAGICON: std::cout << "本消息发送给最小化的窗口(iconic),当该窗口将被拖放而其窗口类中没有定义图标,应用程序能返回一个图标或光标的句柄。当用户拖放图标时系统会显示这个图标或光标" << std::endl; return true;

	case WM_COMPAREITEM: std::cout << "可发送本消息来确定组合框(CBS_SORT)或列表框(LBS_SORT)中新增项的相对位置" << std::endl; return true;



	case WM_GETOBJECT: std::cout << "\"oleacc.dll\"(COM组件)(Microsoft Active Accessibility:方便残疾人使用电脑的一种技术)发送本消息激活服务程序以便获取它所包含的关联对象的信息" << std::endl; return true;



	case WM_COMPACTING: std::cout << "显示内存已经很少了" << std::endl; return true;


	case WM_COMMNOTIFY: std::cout << "Win3.1中,当串口事件产生时,通讯设备驱动程序发送消息本消息给系统,指示输入输出队列的状态" << std::endl; return true;

#ifndef SIMPLEMSG
	case WM_WINDOWPOSCHANGING: std::cout << "本消息会发送给那些大小和位置(Z_Order)将被改变的窗口,以调用SetWindowPos函数或其它窗口管理函数" << std::endl; return true;
	case WM_WINDOWPOSCHANGED: std::cout << "本消息会发送给那些大小和位置(Z_Order)已被改变的窗口,以调用SetWindowPos函数或其它窗口管理函数" << std::endl; return true;
#endif
	case WM_POWER: std::cout << "当系统将要进入暂停状态时发送本消息(适用于16位的windows)" << std::endl; return true;

	case WM_COPYDATA: std::cout << "当一个应用程序传递数据给另一个应用程序时发送本消息" << std::endl; return true;
	case WM_CANCELJOURNAL: std::cout << "当用户取消程序日志激活状态时,发送本消息给那个应用程序。该消息使用空窗口句柄发送" << std::endl; return true;


	case WM_NOTIFY: std::cout << "当某控件的某事件已发生或该控件需得到一些信息时,发送本消息给其父窗" << std::endl; return true;

	case WM_INPUTLANGCHANGEREQUEST: std::cout << "当用户通过过单击任务栏上的语言指示符或某快捷键组合选择改变输入法时系统会向焦点窗口发送本消息" << std::endl; return true;
	case WM_INPUTLANGCHANGE: std::cout << "切换输入法后,系统会发送本消息给受影响的顶层窗口" << std::endl; return true;
	case WM_TCARD: std::cout << "程序已初始化windows帮助例程时会发送本消息给应用程序" << std::endl; return true;
	case WM_HELP: std::cout << "按下<F1>后,若某菜单是激活的,就发送本消息给此窗口关联的菜单;否则就发送给有焦点的窗口;若当前都没有焦点,就把本消息发送给当前激活的窗口" << std::endl; return true;
	case WM_USERCHANGED: std::cout << "当用户已登入或退出后发送本消息给所有窗口;当用户登入或退出时系统更新用户的具体设置信息,在用户更新设置时系统马上发送本消息" << std::endl; return true;
	case WM_NOTIFYFORMAT: std::cout << "公用控件、自定义控件和其父窗通过本消息判断控件在WM_NOTIFY通知消息中是使用ANSI还是UNICODE,使用本消息能使某个控件与它的父控件间进行相互通信" << std::endl; return true;
	case WM_CONTEXTMENU: std::cout << "当用户在某窗口中点击右键就发送本消息给该窗口,设置右键菜单" << std::endl; return true;
	case WM_STYLECHANGING: std::cout << "当调用SetWindowLong函数将要改变一个或多个窗口的风格时,发送本消息给那个窗口" << std::endl; return true;
	case WM_STYLECHANGED: std::cout << "当调用SetWindowLong函数改变一个或多个窗口的风格后,发送本消息给那个窗口" << std::endl; return true;
	case WM_DISPLAYCHANGE: std::cout << "当显示器的分辨率改变后,发送本消息给所有窗口" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_GETICON: std::cout << "本消息发送给某个窗口,用于返回与某窗口有关联的大图标或小图标的句柄" << std::endl; return true;
#endif
	case WM_SETICON: std::cout << "应用程序发送本消息让一个新的大图标或小图标与某窗口相关联" << std::endl; return true;
	case WM_NCCREATE: std::cout << "当某窗口首次被创建时,本消息在WM_CREATE消息发送前发送" << std::endl; return true;
	case WM_NCDESTROY: std::cout << "本消息通知某窗口,非客户区正在销毁" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_NCCALCSIZE: std::cout << "当某窗口的客户区的大小和位置须被计算时发送本消息" << std::endl; return true;
	case WM_NCHITTEST: std::cout << "当用户在在非客户区移动鼠标、按住或释放鼠标时发送本消息(击中测试);若鼠标没有被捕获,则本消息在窗口得到光标之后发出,否则消息发送到捕获到鼠标的窗口" << std::endl; return true;
	case WM_NCPAINT: std::cout << "当窗口框架(非客户区)必须被被重绘时,应用程序发送本消息给该窗口" << std::endl; return true;
#endif
	case WM_NCACTIVATE: std::cout << "本消息发送给某窗口,在窗口的非客户区被激活时重绘窗口" << std::endl; return true;
	case WM_GETDLGCODE: std::cout << "发送本消息给某个与对话框程序关联的控件,系统控制方位键和TAB键使输入进入该控件,通过响应本消息应用程序可把它当成一个特殊的输入控件并能处理它" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_SYNCPAINT: std::cout << "当避免联系独立的GUI线程时,本消息用于同步刷新,本消息由系统确定是否发送" << std::endl; return true;
	case WM_NCMOUSEMOVE: std::cout << "当光标在某窗口的非客户区内移动时,发送本消息给该窗口" << std::endl; return true;
#endif
	case WM_NCLBUTTONDOWN: std::cout << "当光标在某窗口的非客户区内的同时按下鼠标左键,会发送本消息" << std::endl; return true;
	case WM_NCLBUTTONUP: std::cout << "当用户释放鼠标左键的同时光标在某窗口的非客户区内时,会发送本消息" << std::endl; return true;
	case WM_NCLBUTTONDBLCLK: std::cout << "当用户双击鼠标左键的同时光标在某窗口的非客户区内时,会发送本消息" << std::endl; return true;
	case WM_NCRBUTTONDOWN: std::cout << "当用户按下鼠标右键的同时光标在某窗口的非客户区内时,会发送本消息" << std::endl; return true;
	case WM_NCRBUTTONUP: std::cout << "当用户释放鼠标右键的同时光标在某窗口的非客户区内时,会发送本消息" << std::endl; return true;
	case WM_NCRBUTTONDBLCLK: std::cout << "当用户双击鼠标右键的同时光标在某窗口的非客户区内时,会发送本消息" << std::endl; return true;
	case WM_NCMBUTTONDOWN: std::cout << "当用户按下鼠标中键的同时光标在某窗口的非客户区内时,会发送本消息" << std::endl; return true;
	case WM_NCMBUTTONUP: std::cout << "当用户释放鼠标中键的同时光标在某窗口的非客户区内时,会发送本消息" << std::endl; return true;
	case WM_NCMBUTTONDBLCLK: std::cout << "当用户双击鼠标中键的同时光标在某窗口的非客户区内时,会发送本消息" << std::endl; return true;

	case SBM_SETPOS: std::cout << "设置滚动条上滑块的位置" << std::endl; return true;
	case SBM_GETPOS: std::cout << "获取滚动条上滑块的位置" << std::endl; return true;
	case SBM_SETRANGE: std::cout << "设置滚动条的最大与最小位置值" << std::endl; return true;
	case SBM_SETRANGEREDRAW: std::cout << "设置滚动条的最大与最小位置值,并重绘滚动条" << std::endl; return true;
	case SBM_GETRANGE: std::cout << "获取滚动条的最大与最小位置值" << std::endl; return true;
	case SBM_ENABLE_ARROWS: std::cout << "设置滚动条的最大与最小位置值,并重绘滚动条" << std::endl; return true;
	case SBM_SETSCROLLINFO: std::cout << "设置滚动条的参数,本消息通过SCROLLINFO结构指定多种参数,具体指定哪些参数由结构中的fMask成员确定" << std::endl; return true;
	case SBM_GETSCROLLINFO: std::cout << "获取滚动条的参数,发送本消息后,将在SCROLLINFO结构中返回控件的多种参数,当然须事先设定结构的fMask成员以确定具体要取得哪些参数" << std::endl; return true;

	case BM_GETCHECK: std::cout << "获取单选按钮或复选框的状态" << std::endl; return true;
	case BM_SETCHECK: std::cout << "设置单选按钮或复选框的状态" << std::endl; return true;
	case BM_GETSTATE: std::cout << "确定单选按钮或复选框的状态" << std::endl; return true;
	case BM_SETSTATE: std::cout << "改变按钮的高亮状态" << std::endl; return true;
	case BM_SETSTYLE: std::cout << "改变按钮的风格" << std::endl; return true;
	case BM_CLICK: std::cout << "模拟用户点击按钮" << std::endl; return true;
	case BM_GETIMAGE: std::cout << "获取与按钮相关联的图片句柄(图标或位图)" << std::endl; return true;
	case BM_SETIMAGE: std::cout << "把按钮与某张图片相关联(图标或位图),注:按钮须有BS_ICON风格" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_KEYFIRST: std::cout << "用于WinCE系统,本消息在使用GetMessage和PeekMessage函数时,用于过滤键盘消息" << std::endl; return true;
	case WM_KEYUP: std::cout << "当一个非系统按键被释放弹起时(<ALT>键没有被按下),会发送本消息给拥有键盘焦点的窗口" << std::endl; return true;
#endif
	case WM_CHAR: std::cout << "按下某按键,并已发出WM_KEYDOWN、WM_KEYUP消息,本消息包含被按下的按键的字符码" << std::endl; return true;
	case WM_DEADCHAR: std::cout << "\"死字符\"消息,当使用TranslateMessage函数翻译WM_KEYUP消息时,发送本消息给拥有键盘焦点的窗口,注:德语键盘上,有些按键只是给字符添加音标的,并不产生字符,故称\"死字符\"" << std::endl; return true;
	case WM_SYSKEYDOWN: std::cout << "当用户按住<ALT>键的同时又按下其它键时,发送本消息给拥有焦点的窗口" << std::endl; return true;
	case WM_SYSKEYUP: std::cout << "当用户释放一个按键的同时<ALT>键还按着时,发送本消息给拥有焦点的窗口" << std::endl; return true;
	case WM_SYSCHAR: std::cout << "当WM_SYSKEYDOWN消息被TranslateMessage函数翻译后,发送本消息给拥有焦点的窗口,注:<ALT>键被按下" << std::endl; return true;
	case WM_SYSDEADCHAR: std::cout << "\"死字符\"消息,当使用TranslateMessage函数翻译WM_SYSKEYDOWN消息时,发送本消息给拥有键盘焦点的窗口,注:德语键盘上,有些按键只是给字符添加音标的,并不产生字符,故称\"死字符\"" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_KEYLAST: std::cout << "用于WinCE系统,本消息在使用GetMessage和PeekMessage函数时,用于过滤键盘消息" << std::endl; return true;
#endif
	case WM_IME_STARTCOMPOSITION: std::cout << "当用户开始输入编码时,系统立即发送该消息到IME中,IME打开编码窗口,注:输入法相关" << std::endl; return true;
	case WM_IME_ENDCOMPOSITION: std::cout << "当编码结束时,IME发送本消息,用户程序可接受本消息,以便自己显示用户输入的编码,注:输入法相关" << std::endl; return true;
	case WM_IME_COMPOSITION: std::cout << "当用户改变了编码状态时,发送本消息,应用程序可通过调用ImmGetCompositionString函数获取新的编码状态" << std::endl; return true;
	case WM_INITDIALOG: std::cout << "在某对话框程序被显示前发送本消息给该对话框程序,通常用本消息对控件进行一些初始化工作和执行其它任务" << std::endl; return true;
	case WM_COMMAND: std::cout << "用户选择一条菜单命令项或某控件发送一条通知消息给其父窗,或某快捷键被翻译时,本消息被发送" << std::endl; return true;
	case WM_SYSCOMMAND: std::cout << "当用户选择一条系统菜单命令、用户最大化或最小化或还原或关闭时，窗口会收到本消息" << std::endl; return true;
	case WM_TIMER: std::cout << "发生了定时器事件" << std::endl; return true;
	case WM_HSCROLL: std::cout << "当窗口的标准水平滚动条产生一个滚动事件时,发送本消息给该窗口" << std::endl; return true;
	case WM_VSCROLL: std::cout << "当窗口的标准垂直滚动条产生一个滚动事件时,发送本消息给该窗口" << std::endl; return true;
	case WM_INITMENU: std::cout << "当一个菜单将被激活时发送本消息,它发生在用户点击了某菜单项或按下某菜单键。它允许程序在显示前更改菜单" << std::endl; return true;
	case WM_INITMENUPOPUP: std::cout << "当一个下拉菜单或子菜单将被激活时发送本消息,它允许程序在它显示前更改菜单,却不更改全部菜单" << std::endl; return true;
	case WM_MENUSELECT: std::cout << "当用户选择一条菜单项时,发送本消息给菜单的所有者(一般是窗口)" << std::endl; return true;
	case WM_MENUCHAR: std::cout << "当菜单已被激活且用户按下了某菜单字符键(菜单字符键用括号括着、带下划线,不同于快捷键),发送本消息给菜单的所有者" << std::endl; return true;
	case WM_ENTERIDLE: std::cout << "当一个模态对话框或菜单进入空闲状态时,发送本消息给它的所有者,一个模态对话框或菜单进入空闲状态就是在处理完一条或几条先前的消息后,没有消息在消息列队中等待" << std::endl; return true;
	case WM_MENURBUTTONUP: std::cout << "本消息允许程序为菜单项提供一个感知上下文的菜单(即快捷菜单),要为菜单项显示一下上下文菜单,请使用TPM_RECURSE标识调用TrackPopupMenuEx函数" << std::endl; return true;
	case WM_MENUDRAG: std::cout << "当用户拖动菜单项时,发送本消息给拖放菜单的拥有者,可让菜单支持拖拽,可使用OLE拖放传输协议启动拖放操作,注:菜单要具有MNS_DRAGDROP风格" << std::endl; return true;
	case WM_MENUGETOBJECT: std::cout << "当鼠标光标进入或离开菜单项时,本消息发送给支持拖放的菜单的拥有者,相关结构体:MENUGETOBJECTINFO,注:菜单要具有MNS_DRAGDROP风格" << std::endl; return true;
	case WM_UNINITMENUPOPUP: std::cout << "当一条下拉菜单或子菜单被销毁时,发送本消息" << std::endl; return true;
	case WM_MENUCOMMAND: std::cout << "当用户在一个菜单上作出选择时,会发送本消息,菜单要具有MNS_NOTIFYBYPOS风格(在MENUINFO结构体中设置)" << std::endl; return true;
	case WM_CTLCOLORMSGBOX: std::cout << "系统绘制消息框前发送本消息给消息框的所有者窗口,通过响应本消息,所有者窗口可通过使用给定的相关显示设备的句柄来设置消息框的文本和背景色" << std::endl; return true;
	case WM_CTLCOLOREDIT: std::cout << "当一个编辑框控件将要被绘制时,发送本消息给其父窗;通过响应本消息,所有者窗口可通过使用给定的相关显示设备的句柄来设置编辑框的文本和背景色" << std::endl; return true;
	case WM_CTLCOLORLISTBOX: std::cout << "当一个列表框控件将要被绘制前,发送本消息给其父窗;通过响应本消息,所有者窗口可通过使用给定的相关显示设备的句柄来设置列表框的文本和背景色" << std::endl; return true;
	case WM_CTLCOLORBTN: std::cout << "本消息设置按钮的背景色" << std::endl; return true;
	case WM_CTLCOLORDLG: std::cout << "设置对话框的背景色,通常是在WinnApp中使用SetDialogBkColor函数实现" << std::endl; return true;
	case WM_CTLCOLORSCROLLBAR: std::cout << "设置滚动条的背景色" << std::endl; return true;
	case WM_CTLCOLORSTATIC: std::cout << "设置一个静态控件的背景色" << std::endl; return true;

	case CB_GETEDITSEL: std::cout << "用于取得组合框所包含编辑框子控件中当前被选中的字符串的起止位置,对应函数:GetEditSel" << std::endl; return true;
	case CB_LIMITTEXT: std::cout << "用于限制组合框所包含编辑框子控件输入文本的长度" << std::endl; return true;
	case CB_SETEDITSEL: std::cout << "用于选中组合框所包含编辑框子控件中的部分字符串,对应函数:GetEditSel" << std::endl; return true;
	case CB_ADDSTRING: std::cout << "用于向组合框控件追加一个列表项(字符串)" << std::endl; return true;
	case CB_DELETESTRING: std::cout << "用于删除组合框中指定的列表项(字符串)" << std::endl; return true;
	case CB_DIR: std::cout << "用于向组合框控件所包含的列表框控件中添加一个文件名列表清单" << std::endl; return true;
	case CB_GETCOUNT: std::cout << "用于返回组合框控件中列表项的总项数" << std::endl; return true;
	case CB_GETCURSEL: std::cout << "用于返回当前被选择项的索引" << std::endl; return true;
	case CB_GETLBTEXT: std::cout << "获取组合框控件中指定列表项的字符串" << std::endl; return true;
	case CB_GETLBTEXTLEN: std::cout << "用于返回组合框控件中指定列表项的字符串的长度(不包括结束符0)" << std::endl; return true;
	case CB_INSERTSTRING: std::cout << "在组合框控件所包含的列表框子控件中的指定位置插入字符串" << std::endl; return true;
	case CB_RESETCONTENT: std::cout << "用于清除组合框控件所有列表项" << std::endl; return true;
	case CB_FINDSTRING: std::cout << "在组合框控件中根据给定的字符串查找匹配字符串(忽略大小写)" << std::endl; return true;
	case CB_SELECTSTRING: std::cout << "在组合框控件中设定与指定字符串相匹配的列表项为选中项" << std::endl; return true;
	case CB_SETCURSEL: std::cout << "在组合框控件中设置指定的列表项为当前选择项" << std::endl; return true;
	case CB_SHOWDROPDOWN: std::cout << "用于显示或关闭下拉式和下拉列表式组合框的列表框" << std::endl; return true;
	case CB_GETITEMDATA: std::cout << "组合框控件每个列表项都有一个32位的附加数据,本消息用于返回指定列表项的附加数据" << std::endl; return true;
	case CB_SETITEMDATA: std::cout << "用于更新组合框所包含的列表框所指定的列表项的32位附加数据" << std::endl; return true;
	case CB_GETDROPPEDCONTROLRECT: std::cout << "用于取组合框中列表框的屏幕位置" << std::endl; return true;
	case CB_SETITEMHEIGHT: std::cout << "指定组合框中列表项的显示高度(像素点)" << std::endl; return true;
	case CB_GETITEMHEIGHT: std::cout << "用于返回指定列表项的高度(像素点)" << std::endl; return true;
	case CB_SETEXTENDEDUI: std::cout << "指定下拉式或下拉列表式组合框是使用默认界面还是扩展界面" << std::endl; return true;
	case CB_GETEXTENDEDUI: std::cout << "用于返回组合框是否存在扩展界面" << std::endl; return true;
	case CB_GETDROPPEDSTATE: std::cout << "用于取组合框中列表框是否可见" << std::endl; return true;
	case CB_FINDSTRINGEXACT: std::cout << "在组合框中查找字符串,忽略大小写,与CB_FINDSTRING不同,本消息必须整个字符串相同" << std::endl; return true;
	case CB_SETLOCALE: std::cout << "设置组合框列表项当前用于排序的语言代码" << std::endl; return true;
	case CB_GETLOCALE: std::cout << "获取组合框列表项当前用于排序的语言代码" << std::endl; return true;
	case CB_GETTOPINDEX: std::cout << "获取组合框中列表框中第一个可见项的索引" << std::endl; return true;
	case CB_SETTOPINDEX: std::cout << "用于将指定的组合框中列表项设置为列表框的第一个可见项" << std::endl; return true;
	case CB_GETHORIZONTALEXTENT: std::cout << "用于返回组合框水平滚动的总宽度(像素点)(要具有水平滚动条)" << std::endl; return true;
	case CB_SETHORIZONTALEXTENT: std::cout << "用于设定组合框中的列表框的宽度" << std::endl; return true;
	case CB_GETDROPPEDWIDTH: std::cout << "取组合框中列表框的宽度(像素点)" << std::endl; return true;
	case CB_SETDROPPEDWIDTH: std::cout << "用于设定组合框中的列表框的最大允许宽度" << std::endl; return true;
	case CB_INITSTORAGE: std::cout << "只适用于Win95,当将要向组合框中列表框中加入大量表项时,本消息将预先分配一块内存,以免大量添加表项多次分配内存,从而加快程序运行速度" << std::endl; return true;
	case CB_MSGMAX: std::cout << "该消息还具有三个消息值:0x0162,0x0163,0x0165,消息含义不明,搜索了整个Visual Studio 6的目录也只有其定义,却未见其使用的相关代码" << std::endl; return true;

	case CBN_ERRSPACE: std::cout << "通知父窗(通过WM_COMMAND获知),组合框内存不足" << std::endl; return true;
	case CBN_KILLFOCUS: std::cout << "通知父窗(通过WM_COMMAND获知),组合框失去输入焦点" << std::endl; return true;
	case CBN_SELENDOK: std::cout << "通知父窗(通过WM_COMMAND获知),用户对组合框的选择是合法的" << std::endl; return true;

	case LB_ADDSTRING: std::cout << "向列表框中添加字符串" << std::endl; return true;
	case LB_INSERTSTRING: std::cout << "向列表框中插入一个条目数据或字符串。不像LB_ADDSTRING消息,该消息不会激活LBS_SORT样式来对条目进行排序" << std::endl; return true;
	case LB_DELETESTRING: std::cout << "删除列表框中的一个字符串" << std::endl; return true;
	case LB_SELITEMRANGEEX: std::cout << "在多选状态下的列表框中选择一个或多个连续的条目" << std::endl; return true;
	case LB_RESETCONTENT: std::cout << "清除列表框中所有列表项" << std::endl; return true;
	case LB_SETSEL: std::cout << "在多选状态下的列表框中选择一个字符串" << std::endl; return true;
	case LB_SETCURSEL: std::cout << "在列表框中选择一个字符串,并将其所在的条目滚动到视野内,并高亮新选中的字符串" << std::endl; return true;
	case LB_GETSEL: std::cout << "获得列表框中一个条目的选择状态" << std::endl; return true;
	case LB_GETCURSEL: std::cout << "获得列表框中当前被选条目的索引。仅在单选状态的列表框有效" << std::endl; return true;
	case LB_GETTEXT: std::cout << "从列表框中获得一个字符串" << std::endl; return true;
	case LB_GETTEXTLEN: std::cout << "获得一个列表框中字符串的长度" << std::endl; return true;
	case LB_GETCOUNT: std::cout << "获得列表框中条目的数量" << std::endl; return true;
	case LB_SELECTSTRING: std::cout << "仅适用于单选择列表框,设定与指定字符串相匹配的列表项为选中项,会滚动列表框以使选择项可见" << std::endl; return true;
	case LB_DIR: std::cout << "在列表框中列出文件名" << std::endl; return true;
	case LB_GETTOPINDEX: std::cout << "返回列表框中第一个可见项的索引" << std::endl; return true;
	case LB_FINDSTRING: std::cout << "在列表框中查找匹配字符串(忽略大小写)" << std::endl; return true;
	case LB_GETSELCOUNT: std::cout << "仅用于多重选择列表框,本消息用于返回选择项的数目" << std::endl; return true;
	case LB_GETSELITEMS: std::cout << "仅用于多重选择列表框,本消息用于获得选中项的数目及位置" << std::endl; return true;
	case LB_SETTABSTOPS: std::cout << "设置列表框的光标(输入焦点)站数及索引顺序表" << std::endl; return true;
	case LB_GETHORIZONTALEXTENT: std::cout << "返回列表框的可滚动的宽度(像素点)" << std::endl; return true;
	case LB_SETHORIZONTALEXTENT: std::cout << "本消息设置列表框的滚动宽度" << std::endl; return true;
	case LB_SETCOLUMNWIDTH: std::cout << "为列表框指定列数" << std::endl; return true;
	case LB_ADDFILE: std::cout << "为列表框增加文件名" << std::endl; return true;
	case LB_SETTOPINDEX: std::cout << "用于列表框将中指定的列表项设置为列表框的第一个可见项,会将列表框滚动到合适的位置" << std::endl; return true;
	case LB_GETITEMRECT: std::cout << "用于列表框中获得列表项的客户区的RECT" << std::endl; return true;
	case LB_GETITEMDATA: std::cout << "列表框中每个列表项都有个32位附加数据,本消息用于返回指定列表项的附加数据" << std::endl; return true;
	case LB_SETITEMDATA: std::cout << "用于更新列表框中指定列表项的32位附加数据" << std::endl; return true;
	case LB_SELITEMRANGE: std::cout << "仅用于多重选择列表框,用来使指定范围内的列表项选中或落选" << std::endl; return true;
	case LB_SETANCHORINDEX: std::cout << "用于列表框中设置鼠标最后选中的表项成指定表项" << std::endl; return true;
	case LB_GETANCHORINDEX: std::cout << "用于列表框中鼠标最后选中的项的索引" << std::endl; return true;
	case LB_SETCARETINDEX: std::cout << "用于列表框中设置键盘输入焦点到指定表项" << std::endl; return true;
	case LB_GETCARETINDEX: std::cout << "用于列表框中返回具有矩形焦点的项目索引" << std::endl; return true;
	case LB_SETITEMHEIGHT: std::cout << "用于列表框中指定列表项显示高度" << std::endl; return true;
	case LB_GETITEMHEIGHT: std::cout << "用于列表框中返回列表框中某一项的高度(像素点)" << std::endl; return true;
	case LB_FINDSTRINGEXACT: std::cout << "用于列表框中查找字符串(忽略大小写),与LB_FINDSTRING不同,本消息必须整个字符串相同" << std::endl; return true;
	case LB_SETLOCALE: std::cout << "用于列表框中设置列表项当前用于排序的语言代码,当用户使用LB_ADDSTRING向组合框中的列表框中添加记录,并使用LBS_SORT风格进行重新排序时,必须使用该语言代码" << std::endl; return true;
	case LB_GETLOCALE: std::cout << "用于列表框中获取列表项当前用于排序的语言代码,当用户使用LB_ADDSTRING向组合框中的列表框中添加记录,并使用LBS_SORT风格进行重新排序时,必须使用该语言代码" << std::endl; return true;
	case LB_SETCOUNT: std::cout << "用于列表框中设置表项数目" << std::endl; return true;
	case LB_INITSTORAGE: std::cout << "只适用于Win95版本,当将要向列表框中加入大量表项时,本消息将预先分配一块内存,以免在以后的操作中一次次分配内存" << std::endl; return true;
	case LB_ITEMFROMPOINT: std::cout << "用于列表框中获得与指定点最近的项目索引" << std::endl; return true;
	case LB_MSGMAX: std::cout << "该消息还具有三个消息值:0x01B0,0x01B1,0x01B3,,消息含义不明,搜索了整个Visual Studio 6的目录也只有其定义,却未见其使用的相关代码" << std::endl; return true;

	case LBN_ERRSPACE: std::cout << "通知父窗(通过WM_COMMAND获知),列表框内存不够" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_MOUSEMOVE: std::cout << "移动鼠标" << std::endl; return true;
#endif
	case WM_LBUTTONDOWN: std::cout << "按下鼠标左键" << std::endl; return true;
	case WM_LBUTTONUP: std::cout << "释放鼠标左键" << std::endl; return true;
	case WM_LBUTTONDBLCLK: std::cout << "双击鼠标左键" << std::endl; return true;
	case WM_RBUTTONDOWN: std::cout << "按下鼠标右键" << std::endl; return true;
	case WM_RBUTTONUP: std::cout << "释放鼠标右键" << std::endl; return true;
	case WM_RBUTTONDBLCLK: std::cout << "双击鼠标右键" << std::endl; return true;
	case WM_MBUTTONDOWN: std::cout << "按下鼠标中键" << std::endl; return true;
	case WM_MBUTTONUP: std::cout << "释放鼠标中键" << std::endl; return true;
	case WM_MBUTTONDBLCLK: std::cout << "双击鼠标中键" << std::endl; return true;
	case WM_MOUSEWHEEL: std::cout << "当鼠标轮子转动时,发送本消息给当前拥有焦点的控件" << std::endl; return true;
	case WM_MOUSELAST: std::cout << "WM_MBUTTONDBLCLK的别名,通常用于判断鼠标消息的范围,对应的还有WM_MOUSEFIRST,例如:if(message > =  WM_MOUSEFIRST)&&(message <= WM_MOUSELAST)" << std::endl; return true;
	case WM_PARENTNOTIFY: std::cout << "当MDI子窗口被创建或被销毁,或用户按了一下鼠标键而光标在子窗口上时,发送本消息给其父窗" << std::endl; return true;
	case WM_ENTERMENULOOP: std::cout << "发送本消息通知应用程序的主窗口已进入菜单循环模式" << std::endl; return true;
	case WM_EXITMENULOOP: std::cout << "发送本消息通知应用程序的主窗口已退出菜单循环模式" << std::endl; return true;
	case WM_NEXTMENU: std::cout << "当使用左箭头光标键或右箭头光标键在菜单条与系统菜单之间切换时,会发送本消息给应用程序,相关结构体:MDINEXTMENU" << std::endl; return true;
	case WM_SIZING: std::cout << "当用户正在调整窗口大小时,发送本消息给窗口;通过本消息应用程序可监视窗口大小和位置,也可修改它们" << std::endl; return true;
	case WM_CAPTURECHANGED: std::cout << "当它失去捕获的鼠标时,发送本消息给窗口" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_MOVING: std::cout << "当用户在移动窗口时发送本消息,通过本消息应用程序以监视窗口大小和位置,也可修改它们" << std::endl; return true;
#endif
	case WM_POWERBROADCAST: std::cout << "本消息发送给应用程序来通知它有关电源管理事件,比如待机休眠时会发送本消息" << std::endl; return true;
	case WM_DEVICECHANGE: std::cout << "当设备的硬件配置改变时,发送本消息给应用程序或设备驱动程序" << std::endl; return true;
	case WM_MDICREATE: std::cout << "发送本消息给多文档应用程序的客户窗口来创建一个MDI子窗口" << std::endl; return true;
	case WM_MDIDESTROY: std::cout << "发送本消息给多文档应用程序的客户窗口来关闭一个MDI子窗口" << std::endl; return true;
	case WM_MDIACTIVATE: std::cout << "发送本消息给多文档应用程序的客户窗口通知客户窗口激活另一个MDI子窗口,当客户窗口收到本消息后,它发出WM_MDIACTIVE消息给MDI子窗口(未激活)来激活它" << std::endl; return true;
	case WM_MDIRESTORE: std::cout << "发送本消息给MDI客户窗口,让子窗口从最大最小化恢复到原来的大小" << std::endl; return true;
	case WM_MDINEXT: std::cout << "发送本消息给MDI客户窗口,激活下一个或前一个窗口" << std::endl; return true;
	case WM_MDIMAXIMIZE: std::cout << "发送本消息给MDI客户窗口来最大化一个MDI子窗口" << std::endl; return true;
	case WM_MDITILE: std::cout << "发送本消息给MDI客户窗口,以平铺方式重新排列所有MDI子窗口" << std::endl; return true;
	case WM_MDICASCADE: std::cout << "发送本消息给MDI客户窗口,以层叠方式重新排列所有MDI子窗口" << std::endl; return true;
	case WM_MDIICONARRANGE: std::cout << "发送本消息给MDI客户窗口重新排列所有最小化的MDI子窗口" << std::endl; return true;
	case WM_MDIGETACTIVE: std::cout << "发送本消息给MDI客户窗口以找到激活的子窗口句柄" << std::endl; return true;
	case WM_MDISETMENU: std::cout << "发送本消息给MDI客户窗口,用MDI菜单代替子窗口的菜单" << std::endl; return true;
	case WM_ENTERSIZEMOVE: std::cout << "当某窗口进入移动或调整大小的模式循环时,本消息发送到该窗口" << std::endl; return true;
	case WM_EXITSIZEMOVE: std::cout << "确定用户改变窗口大小或改变窗口位置的事件是何时完成的" << std::endl; return true;
	case WM_DROPFILES: std::cout << "鼠标拖放时,放下事件产生时发送本消息,比如:文件拖放功能" << std::endl; return true;
	case WM_MDIREFRESHMENU: std::cout << "发送本消息给多文档应用程序的客户窗口,根据当前MDI子窗口更新MDI框架窗口的菜单" << std::endl; return true;



	case WM_IME_SETCONTEXT: std::cout << "应用程序的窗口激活时,系统将向应用程序发送WM_IME_SETCONTEXT消息,注:输入法相关" << std::endl; return true;
	case WM_IME_NOTIFY: std::cout << "可使用WM_IME_NOTIFY消息来通知关于IME窗口状态的常规改变,注:输入法相关" << std::endl; return true;
	case WM_IME_CONTROL: std::cout << "可使用WM_IME_CONTROL消息来改变字母组合窗口的位置,注:输入法相关" << std::endl; return true;
	case WM_IME_COMPOSITIONFULL: std::cout << "用户接口窗口不能增加编码窗口的尺寸时,IME用户接口窗口将发送WM_IME_COMPOSITIONFULL消息,可不处理,注:输入法相关" << std::endl; return true;
	case WM_IME_SELECT: std::cout << "系统发出WM_IME_SELECT以便选择一个新的IME输入法,注:输入法相关" << std::endl; return true;
	case WM_IME_CHAR: std::cout << "当打开输入法输入文字时,会发送WM_IME_CHAR消息" << std::endl; return true;
	case WM_IME_REQUEST: std::cout << "应用程序请求输入法时,触发发送本消息" << std::endl; return true;
	case WM_IME_KEYDOWN: std::cout << "在输入法录字窗口中按下按键时,触发发送本消息" << std::endl; return true;
	case WM_IME_KEYUP: std::cout << "在输入法录字窗口中释放按键时,触发发送本消息" << std::endl; return true;
	case WM_MOUSEHOVER: std::cout << "鼠标移过控件时,触发发送本消息" << std::endl; return true;
	case WM_MOUSELEAVE: std::cout << "鼠标离开控件时,触发发送本消息" << std::endl; return true;
	case WM_CUT: std::cout << "应用程序发送本消息给一个编辑框或组合框来删除当前选择的文本" << std::endl; return true;
	case WM_COPY: std::cout << "应用程序发送本消息给一个编辑框或组合框,以便用CF_TEXT格式复制当前选择的文本到剪贴板" << std::endl; return true;
	case WM_PASTE: std::cout << "应用程序发送本消息给编辑框或组合框,以便从剪贴板中得到数据" << std::endl; return true;
	case WM_CLEAR: std::cout << "应用程序发送本消息给编辑框或组合框,以清除当前选择的内容" << std::endl; return true;
	case WM_UNDO: std::cout << "应用程序发送本消息给编辑框或组合框,以撤消最后一次操作" << std::endl; return true;
	case WM_RENDERFORMAT: std::cout << "应用程序需要系统剪切板数据时,触发发送本消息" << std::endl; return true;
	case WM_RENDERALLFORMATS: std::cout << "应用程序退出时在程序退出时,系统会给当前程序发送该消息,要求提供所有格式的剪帖板数据,避免造成数据丢失" << std::endl; return true;
	case WM_DESTROYCLIPBOARD: std::cout << "当调用EmptyClipboard函数时,发送本消息给剪贴板的所有者" << std::endl; return true;
	case WM_DRAWCLIPBOARD: std::cout << "当剪贴板的内容变化时,发送本消息给剪贴板观察链的首个窗口;它允许用剪贴板观察窗口来显示剪贴板的新内容" << std::endl; return true;
	case WM_PAINTCLIPBOARD: std::cout << "当剪贴板包含CF_OWNERDIPLAY格式的数据,并且剪贴板观察窗口的客户区需要重画时,触发发送本消息" << std::endl; return true;
	case WM_VSCROLLCLIPBOARD: std::cout << "当剪贴板查看器的垂直滚动条被单击时,触发发送本消息" << std::endl; return true;
	case WM_SIZECLIPBOARD: std::cout << "当剪贴板包含CF_OWNERDIPLAY格式的数据,并且剪贴板观察窗口的客户区域的大小已改变时,本消息通过剪贴板观察窗口发送给剪贴板的所有者" << std::endl; return true;
	case WM_ASKCBFORMATNAME: std::cout << "通过剪贴板观察窗口发送本消息给剪贴板的所有者,以请求一个CF_OWNERDISPLAY格式的剪贴板的名字" << std::endl; return true;
	case WM_CHANGECBCHAIN: std::cout << "当一个窗口从剪贴板观察链中移去时,发送本消息给剪贴板观察链的首个窗口" << std::endl; return true;
	case WM_HSCROLLCLIPBOARD: std::cout << "本消息通过一个剪贴板观察窗口发送给剪贴板的所有者,它发生在当剪贴板包含CFOWNERDISPALY格式的数据,并且有个事件在剪贴板观察窗的水平滚动条上,所有者应滚动剪贴板图像并更新滚动条的值" << std::endl; return true;
	case WM_QUERYNEWPALETTE: std::cout << "本消息发送给将要收到焦点的窗口,本消息能使窗口在收到焦点时同时有机会实现逻辑调色板" << std::endl; return true;
	case WM_PALETTEISCHANGING: std::cout << "当一个应用程序正要实现它的逻辑调色板时,发本消息通知所有的应用程序" << std::endl; return true;
	case WM_PALETTECHANGED: std::cout << "本消息在一个拥有焦点的窗口实现它的逻辑调色板后,发送本消息给所有顶级并重叠的窗口,以此来改变系统调色板" << std::endl; return true;
	case WM_HOTKEY: std::cout << "当用户按下由RegisterHotKey函数注册的热键时,发送本消息" << std::endl; return true;
	case WM_PRINT: std::cout << "发送本消息给一个窗口请求在指定的设备上下文中绘制自身,可用于窗口截图,但对子控件截图时得到的是与子控件等大的黑块" << std::endl; return true;
	case WM_PRINTCLIENT: std::cout << "送本消息给一个窗口请求在指定的设备上下文中绘制其客户区(最通常是在一个打印机设备上下文中)" << std::endl; return true;
	case WM_HANDHELDFIRST: std::cout << "消息含义未知,搜索了整个Visual Studio 6的目录也只有其定义,却未见其使用的相关代码" << std::endl; return true;
	case WM_HANDHELDLAST: std::cout << "消息含义未知,搜索了整个Visual Studio 6的目录也只有其定义,却未见其使用的相关代码" << std::endl; return true;
	case WM_AFXFIRST: std::cout << "指定首个AFX消息(MFC)" << std::endl; return true;
	case WM_AFXLAST: std::cout << "指定末个afx消息" << std::endl; return true;
	case WM_PENWINFIRST: std::cout << "指定首个Pen Window消息,参见:PENWIN.H/WINUSER.H" << std::endl; return true;
	case WM_PENWINLAST: std::cout << "指定末个Pen Window消息,参见:PENWIN.H/WINUSER.H" << std::endl; return true;

	case WM_APP: std::cout << "用于帮助应用程序自定义私有消息,通常形式为:WM_APP + X" << std::endl; return true;
	case WM_USER: std::cout << "用于帮助应用程序自定义私有消息,通常形式为:WM_USER + X" << std::endl; return true;

	case TTM_ACTIVATE: std::cout << "动态停用和启用工具提示控件,TTM即为ToolTip Message" << std::endl; return true;
	case TTM_SETDELAYTIME: std::cout << "使用本消息可指定工具提示控件显示提示文本时的时间延迟(持续时间)(毫秒)" << std::endl; return true;
	case TTM_ADDTOOLA: std::cout << "为某窗口注册添加工具提示控件,相关结构体:TOOLINFO" << std::endl; return true;
	case TTM_ADDTOOLW: std::cout << "为某窗口注册添加工具提示控件,相关结构体:TOOLINFO" << std::endl; return true;
	case TTM_DELTOOLA: std::cout << "为某窗口去除工具提示控件,相关结构体:TOOLINFO" << std::endl; return true;
	case TTM_DELTOOLW: std::cout << "为某窗口去除工具提示控件,相关结构体:TOOLINFO" << std::endl; return true;
	case TTM_NEWTOOLRECTA: std::cout << "为某窗口设置工具提示控件的矩形大小(rect)" << std::endl; return true;
	case TTM_NEWTOOLRECTW: std::cout << "为某窗口设置工具提示控件的矩形大小(rect)" << std::endl; return true;
	case TTM_RELAYEVENT: std::cout << "用于把鼠标消息传递给一个工具提示控件,让其进行处理" << std::endl; return true;
	case TTM_GETTOOLINFOA: std::cout << "从工具提示控件中获取TOOLINFO结构体" << std::endl; return true;
	case TTM_GETTOOLINFOW: std::cout << "从工具提示控件中获取TOOLINFO结构体" << std::endl; return true;
	case TTM_SETTOOLINFOA: std::cout << "用于向工具提示控件中设置TOOLINFO结构体" << std::endl; return true;
	case TTM_SETTOOLINFOW: std::cout << "用于向工具提示控件中设置TOOLINFO结构体" << std::endl; return true;
	case TTM_HITTESTA: std::cout << "获取工具提示控件是否受到点击的信息,即测试鼠标坐标点是否在工具提示控件绑定的矩形内点击" << std::endl; return true;
	case TTM_HITTESTW: std::cout << "获取工具提示控件是否受到点击的信息,即测试鼠标坐标点是否在工具提示控件绑定的矩形内点击" << std::endl; return true;
	case TTM_GETTEXTA: std::cout << "从工具提示控件中获取文本数据" << std::endl; return true;
	case TTM_GETTEXTW: std::cout << "从工具提示控件中获取文本数据" << std::endl; return true;
	case TTM_UPDATETIPTEXTA: std::cout << "用于对工具提示控件进行文本设置" << std::endl; return true;
	case TTM_UPDATETIPTEXTW: std::cout << "用于对工具提示控件进行文本设置" << std::endl; return true;
	case TTM_GETTOOLCOUNT: std::cout << "获取被工具提示控件所维护的工具的数量" << std::endl; return true;
	case TTM_ENUMTOOLSA: std::cout << "获取当前是哪个工具提示控件在显示文本" << std::endl; return true;
	case TTM_ENUMTOOLSW: std::cout << "获取当前是哪个工具提示控件在显示文本" << std::endl; return true;
	case TTM_GETCURRENTTOOLA: std::cout << "获取工具提示控件中当前工具的信息" << std::endl; return true;
	case TTM_GETCURRENTTOOLW: std::cout << "获取工具提示控件中当前工具的信息" << std::endl; return true;
	case TTM_WINDOWFROMPOINT: std::cout << "用于子类化一个窗口过程,使之能让工具提示控件为鼠标下的窗口显示工具提示文本" << std::endl; return true;
	case TTM_TRACKACTIVATE: std::cout << "获取工具提示控件中当前工具的信息" << std::endl; return true;
	case TTM_TRACKPOSITION: std::cout << "设置跟踪工具提示控件的坐标" << std::endl; return true;
	case TTM_SETTIPBKCOLOR: std::cout << "设置工具提示控件的窗口背景色" << std::endl; return true;
	case TTM_SETTIPTEXTCOLOR: std::cout << "设置工具提示控件的文本前景色" << std::endl; return true;
	case TTM_GETDELAYTIME: std::cout << "使用本消息可获以工具提示控件显示提示文本时的时间延迟(持续时间)(毫秒)" << std::endl; return true;
	case TTM_GETTIPBKCOLOR: std::cout << "获取工具提示控件的窗口背景色" << std::endl; return true;
	case TTM_GETTIPTEXTCOLOR: std::cout << "获取工具提示控件的文本前景色" << std::endl; return true;
	case TTM_SETMAXTIPWIDTH: std::cout << "设置工具提示控件窗口的最大宽度(像素点)" << std::endl; return true;
	case TTM_GETMAXTIPWIDTH: std::cout << "获取工具提示控件窗口的最大宽度(像素点)" << std::endl; return true;
	case TTM_SETMARGIN: std::cout << "设置工具提示控件窗口的四周边空(像素点)" << std::endl; return true;
	case TTM_GETMARGIN: std::cout << "获取工具提示控件窗口的四周边空(像素点)" << std::endl; return true;
	case TTM_POP: std::cout << "从一个视图中去除已显示的工具提示控件" << std::endl; return true;
	case TTM_UPDATE: std::cout << "强制工具提示控件重绘其窗口" << std::endl; return true;

	case LVM_FIRST: std::cout << "指定Listview控件的首个消息,其它相关消息用LVM_FIRST + X的形式定义,比如:LVM_GETBKCOLOR为LVM_FIRST + 0" << std::endl; return true;
	case LVM_SETBKCOLOR: std::cout << "设置Listview控件的背景色,宏:ListView_SetBkColor" << std::endl; return true;
	case LVM_GETIMAGELIST: std::cout << "获取Listview控件的图片列表句柄,宏:ListView_GetImageList" << std::endl; return true;
	case LVM_SETIMAGELIST: std::cout << "设置Listview控件的图片列表,宏:ListView_SetImageList" << std::endl; return true;
	case LVM_GETITEMCOUNT: std::cout << "获取Listview控件的项目总数,宏:ListView_GetItemCount" << std::endl; return true;
	case LVM_GETITEMA: std::cout << "获取Listview控件的项目,宏:ListView_GetItem" << std::endl; return true;
	case LVM_GETITEMW: std::cout << "获取Listview控件的项目,宏:ListView_GetItem" << std::endl; return true;
	case LVM_SETITEMA: std::cout << "设置Listview控件的项目,宏:ListView_SetItem" << std::endl; return true;
	case LVM_SETITEMW: std::cout << "设置Listview控件的项目,宏:ListView_SetItem " << std::endl; return true;
	case LVM_INSERTITEMA: std::cout << "向Listview控件插入项目,宏:ListView_InsertItem" << std::endl; return true;
	case LVM_INSERTITEMW: std::cout << "向Listview控件插入项目,宏:ListView_InsertItem" << std::endl; return true;
	case LVM_DELETEITEM: std::cout << "用于删除Listview控件中的一行项目,宏:ListView_DeleteItem" << std::endl; return true;
	case LVM_DELETEALLITEMS: std::cout << "清空Listview控件所有项目,宏:ListView_DeleteAllItems" << std::endl; return true;
	case LVM_GETCALLBACKMASK: std::cout << "获取Listview控件的回调掩码,宏:ListView_GetCallbackMask" << std::endl; return true;
	case LVM_SETCALLBACKMASK: std::cout << "设置Listview控件的回调掩码,宏:ListView_SetCallbackMask" << std::endl; return true;
	case LVM_GETNEXTITEM: std::cout << "获取Listview控件中的下一个项目,宏:ListView_GetNextItem" << std::endl; return true;
	case LVM_FINDITEMA: std::cout << "在Listview控件中寻找项目,宏:ListView_FindItem" << std::endl; return true;
	case LVM_FINDITEMW: std::cout << "在Listview控件中寻找项目,宏:ListView_FindItem" << std::endl; return true;
	case LVM_GETITEMRECT: std::cout << "在Listview控件中获取指定项目的矩形范围,宏:ListView_GetItemRect" << std::endl; return true;
	case LVM_SETITEMPOSITION: std::cout << "设置Listview控件的项目位置,宏:ListView_SetItemPosition" << std::endl; return true;
	case LVM_GETITEMPOSITION: std::cout << "获取Listview控件的项目位置,宏:ListView_GetItemPosition" << std::endl; return true;
	case LVM_GETSTRINGWIDTHA: std::cout << "在Listview控件中获取显示一个文本所需列宽,宏:ListView_GetStringWidth" << std::endl; return true;
	case LVM_GETSTRINGWIDTHW: std::cout << "在Listview控件中获取显示一个文本所需列,宏:ListView_GetStringWidth" << std::endl; return true;
	case LVM_HITTEST: std::cout << "在Listview控件中进行点击测试,判断某个鼠标坐标点是否落在Listview控件某个项目中,宏:ListView_HitTest" << std::endl; return true;
	case LVM_ENSUREVISIBLE: std::cout << "Listview控件的某个项目保证能显示出来(可视),并决定一个项目的全部或部份是否可视,宏:ListView_EnsureVisible" << std::endl; return true;
	case LVM_SCROLL: std::cout << "在Listview控件中移动滚动条,宏:ListView_Scroll" << std::endl; return true;
	case LVM_REDRAWITEMS: std::cout << "在Listview控件中重绘项目,宏:ListView_RedrawItems" << std::endl; return true;
	case LVM_ARRANGE: std::cout << "在Listview控件中图像显示时,排列项目,宏:ListView_Arrange" << std::endl; return true;
	case LVM_EDITLABELA: std::cout << "编辑Listview控件中指定项目的文本(即label标签),宏:ListView_EditLabel" << std::endl; return true;
	case LVM_EDITLABELW: std::cout << "编辑Listview控件中指定项目的文本(即label标签),宏:ListView_EditLabel" << std::endl; return true;
	case LVM_GETEDITCONTROL: std::cout << "在Listview控件中,当列表控件正在进行编辑时,返回该编辑框的句柄,宏:ListView_GetEditControl" << std::endl; return true;
	case LVM_GETCOLUMNA: std::cout << "获取Listview控件中某列的属性,宏:ListView_GetColumn" << std::endl; return true;
	case LVM_GETCOLUMNW: std::cout << "获取Listview控件中某列的属性,宏:ListView_GetColumn" << std::endl; return true;
	case LVM_SETCOLUMNA: std::cout << "设置Listview控件中某列的属性,宏:ListView_InsertColumn" << std::endl; return true;
	case LVM_SETCOLUMNW: std::cout << "设置Listview控件中某列的属性,宏:ListView_InsertColumn" << std::endl; return true;
	case LVM_INSERTCOLUMNA: std::cout << "向Listview控件插入一个列,宏:ListView_InsertColumn" << std::endl; return true;
	case LVM_INSERTCOLUMNW: std::cout << "向Listview控件插入一个列,宏:ListView_InsertColumn" << std::endl; return true;
	case LVM_DELETECOLUMN: std::cout << "删除Listview控件中的一个列,宏:ListView_DeleteColumn" << std::endl; return true;
	case LVM_GETCOLUMNWIDTH: std::cout << "在Listview控件中获取指定列的宽度,宏:ListView_GetColumnWidth" << std::endl; return true;
	case LVM_SETCOLUMNWIDTH: std::cout << "在Listview控件中设置指定列的宽度,宏:ListView_SetColumnWidth" << std::endl; return true;
	case LVM_GETHEADER: std::cout << "在Listview控件中获取表头控件的句柄,宏:ListView_GetHeader" << std::endl; return true;
	case LVM_CREATEDRAGIMAGE: std::cout << "在Listview控件中为指定的项目创建一个拖曳图像列表,宏:ListView_CreateDragImage" << std::endl; return true;
	case LVM_GETVIEWRECT: std::cout << "当前所有项目所占用的矩形范围,宏:ListView_GetViewRect" << std::endl; return true;
	case LVM_GETTEXTCOLOR: std::cout << "在Listview控件中获取文本颜色,宏:ListView_GetTextColor" << std::endl; return true;
	case LVM_SETTEXTCOLOR: std::cout << "在Listview控件中设置文本颜色,宏:ListView_SetTextColor" << std::endl; return true;
	case LVM_GETTEXTBKCOLOR: std::cout << "在Listview控件中获取文本背景色,宏:ListView_SetTextBkC" << std::endl; return true;
	case LVM_SETTEXTBKCOLOR: std::cout << "在Listview控件中设置文本背景色,宏:ListView_SetTextBkColor" << std::endl; return true;
	case LVM_GETTOPINDEX: std::cout << "在Listview控件中获取可视范围中首个项目的索引,宏:ListView_GetTopInd" << std::endl; return true;
	case LVM_GETCOUNTPERPAGE: std::cout << "在Listview控件中获取当前可视项目数量,宏:ListView_GetCountPerPage" << std::endl; return true;
	case LVM_GETORIGIN: std::cout << "在Listview控件中获取当前的原点,宏:ListView_GetOrigin" << std::endl; return true;
	case LVM_UPDATE: std::cout << "用于刷新Listview控件,若该控件具有LVS_AUTOARRANGE风格,则会引起排列操作,宏:ListView_Update" << std::endl; return true;
	case LVM_SETITEMSTATE: std::cout << "在Listview控件中设置项目状态,宏:ListView_SetItemState" << std::endl; return true;
	case LVM_GETITEMSTATE: std::cout << "在Listview控件中获取项目状态,宏:ListView_GetItemS" << std::endl; return true;
	case LVM_GETITEMTEXTA: std::cout << "在Listview控件中获取项目文本,宏:ListView_GetItemText" << std::endl; return true;
	case LVM_GETITEMTEXTW: std::cout << "在Listview控件中获取项目文本,宏:ListView_GetItemText" << std::endl; return true;
	case LVM_SETITEMTEXTA: std::cout << "在Listview控件中设置项目文本,宏:ListView_SetItemText" << std::endl; return true;
	case LVM_SETITEMTEXTW: std::cout << "在Listview控件中设置项目文本,宏:ListView_SetItemText" << std::endl; return true;
	case LVM_SETITEMCOUNT: std::cout << "在Listview控件(虚拟列表视图控件,LVS_OWNERDATA风格)中设置当前列表中项目的总数,宏:ListView_SetItemCount/ListView_SetItemCountEx" << std::endl; return true;
	case LVM_SORTITEMS: std::cout << "在Listview控件中进行项目分类排序,宏:ListView_SortItems" << std::endl; return true;
	case LVM_SETITEMPOSITION32: std::cout << "设置Listview控件的项目位置,LVM_SETITEMPOSITION消息不同的是,本消息使用32位的坐标,宏:ListView_SetItemPosition32" << std::endl; return true;
	case LVM_GETSELECTEDCOUNT: std::cout << "获取Listview控件中被选择项目总数,宏:ListView_GetSelectedCount" << std::endl; return true;
	case LVM_GETITEMSPACING: std::cout << "获取Listview控件的项目间距,宏:ListView_GetItemSpacing" << std::endl; return true;
	case LVM_GETISEARCHSTRINGA: std::cout << "在Listview控件中获取增量搜索模式的字符串,宏:ListView_GetISearchString" << std::endl; return true;
	case LVM_GETISEARCHSTRINGW: std::cout << "在Listview控件中获取增量搜索模式的字符串,宏:ListView_GetISearchString" << std::endl; return true;
	case LVM_SETICONSPACING: std::cout << "在Listview控件中设置图标间距,宏:ListView_SetIconSpacing" << std::endl; return true;
	case LVM_SETEXTENDEDLISTVIEWSTYLE: std::cout << "设置Listview控件的扩展风格,宏:ListView_SetExtendedListViewStyle/ListView_SetExtendedListViewStyleEx" << std::endl; return true;
	case LVM_GETEXTENDEDLISTVIEWSTYLE: std::cout << "获取Listview控件的扩展风格,宏:ListView_GetExtendedListViewStyle" << std::endl; return true;
	case LVM_GETSUBITEMRECT: std::cout << "获取Listview控件子项目的矩形范围,宏:ListView_GetSubItemRect" << std::endl; return true;
	case LVM_SUBITEMHITTEST: std::cout << "对Listview控件进行点击测试,以确定哪个项目或子项目处于给定的位置,宏:ListView_SubItemHitTest" << std::endl; return true;
	case LVM_SETCOLUMNORDERARRAY: std::cout << "在Listview控件中设置列显示的排序(从左至右),宏:ListView_SetColumnOrderArray" << std::endl; return true;
	case LVM_GETCOLUMNORDERARRAY: std::cout << "在Listview控件中返回当前列中的左右对齐方式,宏:ListView_GetColumnOrderArray" << std::endl; return true;
	case LVM_SETHOTITEM: std::cout << "在Listview控件中设置热点项目(热项),宏:ListView_SetHotItem" << std::endl; return true;
	case LVM_GETHOTITEM: std::cout << "在Listview控件中获取热点项目(热项),宏:ListView_GetHotItem" << std::endl; return true;
	case LVM_SETHOTCURSOR: std::cout << "在Listview控件中设置热点光标,宏:ListView_SetHotCursor" << std::endl; return true;
	case LVM_GETHOTCURSOR: std::cout << "在Listview控件中获取热点光标,宏:ListView_GetHotCursor" << std::endl; return true;
	case LVM_APPROXIMATEVIEWRECT: std::cout << "在Listview控件中计算显示一个项目所需的宽度和高度,宏:ListView_ApproximateViewRect" << std::endl; return true;
	case LVM_SETWORKAREAS: std::cout << "在Listview控件中设置工作区,宏:ListView_SetWorkAreas" << std::endl; return true;
	case LVM_GETSELECTIONMARK: std::cout << "在Listview控件中获取当前选择项目,宏:ListView_GetSelectionMark" << std::endl; return true;
	case LVM_SETSELECTIONMARK: std::cout << "在Listview控件中设置当前选择项目,宏:ListView_SetSelectionMark" << std::endl; return true;
	case LVM_SETBKIMAGEA: std::cout << "在Listview控件中设置设置背景图片,宏:ListView_SetBkImage" << std::endl; return true;
	case LVM_GETBKIMAGEA: std::cout << "获取Listview控件的背景图片,宏:ListView_GetBkImage" << std::endl; return true;
	case LVM_GETWORKAREAS: std::cout << "获取Listview控件的工作区矩形范围,宏:ListView_GetWorkAreas" << std::endl; return true;
	case LVM_SETHOVERTIME: std::cout << "在Listview控件中设置鼠标在选择某项前停留在该项的延迟毫秒数,宏:ListView_SetHoverTime" << std::endl; return true;
	case LVM_GETHOVERTIME: std::cout << "获取Listview控件的鼠标在选择某项前停留在该项的延迟毫秒数,宏:ListView_GetHoverTime" << std::endl; return true;
	case LVM_GETNUMBEROFWORKAREAS: std::cout << "获取Listview控件的当前工作区中的项目数量,宏:ListView_GetNumberOfWorkAreas" << std::endl; return true;
	case LVM_SETTOOLTIPS: std::cout << "设置Listview控件的工具提示控件,宏:ListView_SetToolTips" << std::endl; return true;
	case LVM_GETTOOLTIPS: std::cout << "获取Listview控件的工具提示控件,宏:ListView_GetToolTips" << std::endl; return true;
	case LVM_SETBKIMAGEW: std::cout << "以宽字符方式设置Listview控件的背景图,宏:ListView_SetBkImage" << std::endl; return true;
	case LVM_GETBKIMAGEW: std::cout << "以宽字符方式获取Listview控件的背景图,宏:ListView_GetBkImage" << std::endl; return true;

	case LVN_ITEMCHANGING: std::cout << "通知Listview控件的父窗:有个项目正被改变" << std::endl; return true;
	case LVN_ITEMCHANGED: std::cout << "通知Listview控件的父窗:有个项目已经被改变" << std::endl; return true;
	case LVN_INSERTITEM: std::cout << "通知Listview控件的父窗:有个新项目被插入了" << std::endl; return true;
	case LVN_DELETEITEM: std::cout << "通知Listview控件的父窗:有个项目要被删除" << std::endl; return true;
	case LVN_DELETEALLITEMS: std::cout << "通知Listview控件的父窗:所有项目要被删除" << std::endl; return true;
	case LVN_BEGINLABELEDITA: std::cout << "通知Listview控件的父窗:一个项目的文本标签编辑的起点" << std::endl; return true;
	case LVN_BEGINLABELEDITW: std::cout << "通知Listview控件的父窗:一个项目的文本标签编辑的起点" << std::endl; return true;
	case LVN_ENDLABELEDITA: std::cout << "通知Listview控件的父窗:一个项目的文本标签编辑的终点" << std::endl; return true;
	case LVN_ENDLABELEDITW: std::cout << "通知Listview控件的父窗:一个项目的文本标签编辑的终点" << std::endl; return true;
	case LVN_COLUMNCLICK: std::cout << "通知Listview控件的父窗:一个列被点击了" << std::endl; return true;
	case LVN_BEGINDRAG: std::cout << "通知Listview控件的父窗:鼠标左键的拖放操作开始了" << std::endl; return true;
	case LVN_BEGINRDRAG: std::cout << "通知Listview控件的父窗:鼠标右键的拖放操作开始了" << std::endl; return true;
	case LVN_ODCACHEHINT: std::cout << "通知消息,由虚列表控件在其可视区域的内容被改变时发送" << std::endl; return true;
	case LVN_ODFINDITEMA: std::cout << "通知消息,由虚列表控件在需要其所有者查找特定的回调项目时发送" << std::endl; return true;
	case LVN_ODFINDITEMW: std::cout << "通知消息,由虚列表控件在需要其所有者查找特定的回调项目时发送" << std::endl; return true;
	case LVN_ITEMACTIVATE: std::cout << "通知消息,由虚列表控件在用户激活了某个项目时发送" << std::endl; return true;
	case LVN_ODSTATECHANGED: std::cout << "通知消息,由虚列表控件在一个项目的状态或排列已被改变时发送" << std::endl; return true;
	case LVN_HOTTRACK: std::cout << "通知消息,由虚列表控件在用户的鼠标掠过某个项目时发送" << std::endl; return true;
	case LVN_GETDISPINFOA: std::cout << "由虚列表控件发送,请求父窗提供显示项目或对某个项目排序所需的信息" << std::endl; return true;
	case LVN_GETDISPINFOW: std::cout << "由虚列表控件发送,请求父窗提供显示项目或对某个项目排序所需的信息" << std::endl; return true;
	case LVN_SETDISPINFOA: std::cout << "通知Listview控件的父窗:它得更新某个项目的信息" << std::endl; return true;
	case LVN_SETDISPINFOW: std::cout << "通知Listview控件的父窗:它得更新某个项目的信息" << std::endl; return true;
	case LVN_MARQUEEBEGIN: std::cout << "通知Listview控件的父窗:某个边框选择已开始" << std::endl; return true;
	case LVN_GETINFOTIPA: std::cout << "通知消息,由大图标的具有LVS_EX_INFOTIP扩展风格的Listview控件在它要请求附加的文本信息(显示于工具提示控件中)时发送" << std::endl; return true;
	case LVN_GETINFOTIPW: std::cout << "通知消息,由大图标的具有LVS_EX_INFOTIP扩展风格的Listview控件在它要请求附加的文本信息(显示于工具提示控件中)时发送" << std::endl; return true;

	case TVM_CREATEDRAGIMAGE: std::cout << "创建树控件的RAG图象,宏:TreeView_CreateDragImage" << std::endl; return true;
	case TVM_DELETEITEM: std::cout << "删除树控件的项目,宏:TreeView_DeleteAllItems" << std::endl; return true;
	case TVM_EDITLABELA: std::cout << "编辑树控件的标签,宏:TVN_BEGINLABELEDIT" << std::endl; return true;
	case TVM_EDITLABELW: std::cout << "编辑树控件的标签,宏:TVN_BEGINLABELEDIT" << std::endl; return true;
	case TVM_ENDEDITLABELNOW: std::cout << "结束编辑树控件的新标签,宏:TreeView_EndEditLabelNow" << std::endl; return true;
	case TVM_ENSUREVISIBLE: std::cout << "保证树控件的某个项目可视,宏:TreeView_EnsureVisible" << std::endl; return true;
	case TVM_EXPAND: std::cout << "扩展或收缩树控件某节点下的所有子节点,宏:TreeView_Expand" << std::endl; return true;
	case TVM_GETBKCOLOR: std::cout << "获取树控件的背景色,宏:TreeView_GetBkColor" << std::endl; return true;
	case TVM_GETCOUNT: std::cout << "获取树控件的项目数量,宏:TreeView_GetCount" << std::endl; return true;
	case TVM_GETEDITCONTROL: std::cout << "获取树控件编辑框控件句柄(用于编辑某项文本),宏:TreeView_GetEditControl" << std::endl; return true;
	case TVM_GETIMAGELIST: std::cout << "获取树控件的列表图片,返回图象句柄,宏:TreeView_GetImageList" << std::endl; return true;
	case TVM_GETINDENT: std::cout << "获取树控件的子项相对于父项缩进的量(像素),宏:TreeView_GetIndent" << std::endl; return true;
	case TVM_GETINSERTMARKCOLOR: std::cout << "返回一个包含当前插入编辑颜色的COLORREF,宏:TreeView_GetInsertMarkColor" << std::endl; return true;
	case TVM_GETISEARCHSTRING: std::cout << "获取树控件的搜索文本,宏:TreeView_GetISearchString" << std::endl; return true;
	case TVM_GETITEMA: std::cout << "获取树控件指定项的属性,相关结构:TVITEM,宏:TreeView_GetItem" << std::endl; return true;
	case TVM_GETITEMW: std::cout << "获取树控件指定项的属性,相关结构:TVITEM,宏:TreeView_GetItem" << std::endl; return true;
	case TVM_GETITEMHEIGHT: std::cout << "取树控件项目的行距,宏:TreeView_GetItemHeight" << std::endl; return true;
	case TVM_GETITEMRECT: std::cout << "获取树控件项目的边界矩形,并确定该项是否可视,宏:TreeView_GetItemRect" << std::endl; return true;
	case TVM_GETNEXTITEM: std::cout << "获取树控件下一项目的句柄,宏:TreeView_GetNextItem/TreeView_GetChild/TreeView_GetDropHilight/TreeView_GetFirstVisible/TreeView_GetLastVisible/TreeView_GetNextSibling/TreeView_GetNextVisible/TreeView_GetParent/TreeView_GetPrevSibling/TreeView_GetPrevVisible/TreeView_GetRoot/TreeView_GetSelection " << std::endl; return true;
	case TVM_GETSCROLLTIME: std::cout << "获取树控件的最大滚动时间(毫秒),宏:TreeView_GetScrollTime" << std::endl; return true;
	case TVM_GETTEXTCOLOR: std::cout << "获取树控件的文本颜色,宏:TreeView_GetTextColor" << std::endl; return true;
	case TVM_GETTOOLTIPS: std::cout << "获取树控件所使用的工具提示控件的句柄,宏:TreeView_GetToolTips" << std::endl; return true;
	case TVM_GETUNICODEFORMAT: std::cout << "获取树控件的UNICODE格式标志,宏:TreeView_GetUnicodeFormat" << std::endl; return true;
	case TVM_GETVISIBLECOUNT: std::cout << "获取树控件中的可视项目数量,宏:TreeView_GetVisibleCount" << std::endl; return true;
	case TVM_HITTEST: std::cout << "树控件点击测试,宏:TreeView_HitTest" << std::endl; return true;
	case TVM_INSERTITEMA: std::cout << "在树控件中插入项目,宏:TreeView_InsertItem" << std::endl; return true;
	case TVM_INSERTITEMW: std::cout << "在树控件中插入项目,宏:TreeView_InsertItem" << std::endl; return true;
	case TVM_SELECTITEM: std::cout << "选取树控件的项目,宏:TreeView_Select/TreeView_SelectItem/TreeView_SelectDropTarget" << std::endl; return true;
	case TVM_SETBKCOLOR: std::cout << "设置树控件的背景色,宏:TreeView_SetBkColor" << std::endl; return true;
	case TVM_SETIMAGELIST: std::cout << "设置树控件的图象列表,宏:TreeView_SetImageList" << std::endl; return true;
	case TVM_SETINDENT: std::cout << "设置树控件缩进的宽度,并重画控件以反映新的宽度,宏:TreeView_SetIndent" << std::endl; return true;
	case TVM_SETINSERTMARK: std::cout << "设置树控件的插入标志,宏:TreeView_SetInsertMark" << std::endl; return true;
	case TVM_SETINSERTMARKCOLOR: std::cout << "设置树控件的插入标志色(返回包含先前的插入标记色的COLORREF值),宏:TreeView_SetInsertMarkColor" << std::endl; return true;
	case TVM_SETITEMA: std::cout << "设置树控件的项目,相关结构:TVITEM,宏:TreeView_SetItem" << std::endl; return true;
	case TVM_SETITEMW: std::cout << "设置树控件的项目,相关结构:TVITEM,宏:TreeView_SetItem" << std::endl; return true;
	case TVM_SETITEMHEIGHT: std::cout << "设置树控件的项目高度(像素)(返回项目先前的高度),宏:TreeView_SetItemHeight" << std::endl; return true;
	case TVM_SETSCROLLTIME: std::cout << "设置树控件的最大滚动时间(毫秒),宏:TreeView_SetScrollTime" << std::endl; return true;
	case TVM_SETTEXTCOLOR: std::cout << "设置树控件的文本颜色,宏:TreeView_SetTextColor" << std::endl; return true;
	case TVM_SETTOOLTIPS: std::cout << "设置树控件的工具提示控件(句柄),宏:TreeView_SetToolTips" << std::endl; return true;
	case TVM_SETUNICODEFORMAT: std::cout << "设置树控件的UNICODE格式标志,宏:TreeView_SetUnicodeFormat" << std::endl; return true;
	case TVM_SORTCHILDREN: std::cout << "对树控件中指定父项的子项进行排序,宏:TreeView_SortChildren" << std::endl; return true;
	case TVM_SORTCHILDRENCB: std::cout << "通过一个用来比较各项的回调函数对树控件进行排序,宏:TreeView_SortChildrenCB" << std::endl; return true;

	case TVN_BEGINDRAG: std::cout << "树控件中鼠标左键拖放开始" << std::endl; return true;
	case TVN_BEGINLABELEDIT: std::cout << "开始编辑项目的标签" << std::endl; return true;
	case TVN_BEGINRDRAG: std::cout << "树控件中鼠标右键拖放开始" << std::endl; return true;
	case TVN_DELETEITEM: std::cout << "删除树控件中的项目" << std::endl; return true;
	case TVN_ENDLABELEDIT: std::cout << "允许用户编辑项目的标签" << std::endl; return true;
	case TVN_GETDISPINFO: std::cout << "在需要得到树控件的某结点信息时发送(如得到结点的显示字符)" << std::endl; return true;
	case TVN_GETINFOTIP: std::cout << "获取工具提示控件信息" << std::endl; return true;
	case TVN_ITEMEXPANDED: std::cout << "树控件某结点已被展开或收缩,所用结构:NMTREEVIEW" << std::endl; return true;
	case TVN_ITEMEXPANDING: std::cout << "树控件某结点将被展开或收缩,所用结构:NMTREEVIEW" << std::endl; return true;
	case TVN_KEYDOWN: std::cout << "用户在树控件中按下了某键盘按键,并且树控件获得了输入焦点" << std::endl; return true;
	case TVN_SELCHANGED: std::cout << "用户已改变树控件项目的选择" << std::endl; return true;
	case TVN_SELCHANGING: std::cout << "用户对树控件项目的选择将改变" << std::endl; return true;
	case TVN_SETDISPINFO: std::cout << "通知更新树控件项目的信息" << std::endl; return true;
	case TVN_SINGLEEXPAND: std::cout << "用户使用鼠标单击打开或关闭树控件项目时发送的通知消息" << std::endl; return true;

	case HDM_DELETEITEM: std::cout << "删除标头控件的列项目,宏:Header_DeleteItem" << std::endl; return true;
	case HDM_GETIMAGELIST: std::cout << "获取标头控件的列图标列表,宏:Header_GetImageList" << std::endl; return true;
	case HDM_GETITEMA: std::cout << "获取标头控件的列项目,宏:Header_GetItemRect" << std::endl; return true;
	case HDM_GETITEMW: std::cout << "获取标头控件的列项目,宏:Header_GetItemRect" << std::endl; return true;
	case HDM_GETITEMCOUNT: std::cout << "获取标头控件的列项目数量,宏:Header_GetItemCount " << std::endl; return true;
	case HDM_GETITEMRECT: std::cout << "获取标头控件的项目矩形范围,宏:Header_GetItemRect" << std::endl; return true;
	case HDM_GETORDERARRAY: std::cout << "获取标头控件列的左右对齐方式(提供对标头项目排序的支持),宏:Header_GetOrderArray" << std::endl; return true;
	case HDM_HITTEST: std::cout << "标头控件点击测试,找到鼠标点击的坐标" << std::endl; return true;
	case HDM_INSERTITEM: std::cout << "为标头控件插入列项目,宏:Header_InsertItem" << std::endl; return true;
	case HDM_LAYOUT: std::cout << "获取在一个指定矩形内的标头控件的大小和位置,宏:Header_Layout" << std::endl; return true;
	case HDM_ORDERTOINDEX: std::cout << "获取标头控件列的左右对齐方式(项目索引,基于标头控件中项目的顺序,索引自左到右由0开始),宏:Header_OrderToIndex" << std::endl; return true;
	case HDM_SETHOTDIVIDER: std::cout << "设置高亮度分隔符,提供对标头项目的拖放,宏:Header_SetHotDivider" << std::endl; return true;
	case HDM_SETIMAGELIST: std::cout << "为标头控件设置图像列表,宏:Header_SetImageList" << std::endl; return true;
	case HDM_SETITEMA: std::cout << "设置标头控件中项目的属性,宏:Header_SetItem" << std::endl; return true;
	case HDM_SETITEMW: std::cout << "设置标头控件中项目的属性,宏:Header_SetItem" << std::endl; return true;
	case HDM_SETORDERARRAY: std::cout << "设置标头控件列的左右对齐方式,宏:Header_SetOrderArray" << std::endl; return true;

	case STM_GETICON: std::cout << "获取显示在静态控件中的图标的句柄,须具SS_ICON风格,宏:Static_GetIcon" << std::endl; return true;
	case STM_GETIMAGE: std::cout << "获取显示在静态控件中的图像的句柄,支持图标(SS_ICON)、光标(SS_ICON)、位图(SS_BITMAP)、图元(SS_ENHMETAFILE)" << std::endl; return true;
	case STM_SETICON: std::cout << "设置将要在静态控件中显示的图标(句柄),须具SS_ICON风格,宏:Static_SetIcon" << std::endl; return true;
	case STM_SETIMAGE: std::cout << "设置将要在静态控件中显示的图像(句柄),支持图标(SS_ICON)、光标(SS_ICON)、位图(SS_BITMAP)、图元(SS_ENHMETAFILE),自定义宏(位图):#define Static_SetBitmap(hwndCtl, hBitmap)  ((HBITMAP)(UINT_PTR)SendMessage((hwndCtl), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(HBITMAP)hBitmap))" << std::endl; return true;
	case TB_ADDBUTTONS: std::cout << "向一个工具栏中添加一个或多个按钮,相关结构:TBBUTTON" << std::endl; return true;
	case TB_ADDSTRING: std::cout << "添加一个新字符串到工具栏的内部字符串列表中" << std::endl; return true;
	case TB_AUTOSIZE: std::cout << "调整工具栏的尺寸。当设置按钮或位图尺寸、添加字符串时,发送本消息" << std::endl; return true;
	case TB_BUTTONSTRUCTSIZE: std::cout << "指定工具栏TBBUTTON结构的大小" << std::endl; return true;
	case TB_CHANGEBITMAP: std::cout << "改变工具栏的按钮图像(图像列表中的索引)" << std::endl; return true;
	case TB_CHECKBUTTON: std::cout << "核选或清除工具栏的指定按钮。当一个按钮被核选时,它看起来就像被按下一样" << std::endl; return true;
	case TB_GETANCHORHIGHLIGHT: std::cout << "获取工具栏的固定的加亮设置" << std::endl; return true;
	case TB_GETBITMAP: std::cout << "获取与工具栏按钮相关联的位图的索引" << std::endl; return true;
	case TB_GETBITMAPFLAGS: std::cout << "从工具栏获取位图标志(DWORD值)" << std::endl; return true;
	case TB_GETBUTTONINFO: std::cout << "获取工具栏中按钮的扩展信息,相关结构:TBBUTTONINFO" << std::endl; return true;
	case TB_GETBUTTONTEXT: std::cout << "获取工具栏按钮的文本" << std::endl; return true;
	case TB_GETCOLORSCHEME: std::cout << "获取工具栏的色彩方案信息" << std::endl; return true;
	case TB_GETEXTENDEDSTYLE: std::cout << "获取工具栏的扩展风格" << std::endl; return true;
	case TB_GETHOTITEM: std::cout << "获取工具栏的热点项的索引(从零开始)" << std::endl; return true;
	case TB_GETIMAGELIST: std::cout << "获取工具栏中用于显示按钮常规状态图像的列表" << std::endl; return true;
	case TB_GETINSERTMARK: std::cout << "获取工具栏的插入标记,相关结构:TBINSERTMARK" << std::endl; return true;
	case TB_GETINSERTMARKCOLOR: std::cout << "获取工具栏中包含当前插入标记的颜色(COLORREF值)" << std::endl; return true;
	case TB_GETMAXSIZE: std::cout << "获取工具栏中所有可见按钮与分隔条的总尺寸" << std::endl; return true;
	case TB_GETOBJECT: std::cout << "获取工具栏的IDropTarget接口,IDropTarget用于实现以拖放方式获取资源等" << std::endl; return true;
	case TB_GETPADDING: std::cout << "获取工具栏的填充距(像素)(padding:补白、填充距)" << std::endl; return true;
	case TB_GETROWS: std::cout << "获取工具栏中当前显示的按钮行数,注:除非工具栏是用TBSTYLE_WRAPABLE风格创建的,否则行数总为1" << std::endl; return true;
	case TB_GETTEXTROWS: std::cout << "获取用于显示在工具栏按钮上的文本行的最大数字" << std::endl; return true;
	case TB_GETTOOLTIPS: std::cout << "获取与工具栏相关联的工具提示控件(若有)的句柄,通常工具栏会自己创建并维护其工具提示控件,一般不必使用本消息" << std::endl; return true;
	case TB_HITTEST: std::cout << "点击测试,获取工具栏指定点的位置的整数值,若该值为零或正值,则表示该点所在按钮的索引(非分隔条),为负则该点不在某按钮上" << std::endl; return true;
	case TB_INSERTBUTTON: std::cout << "在工具栏中插入一个按钮" << std::endl; return true;
	case TB_INSERTMARKHITTEST: std::cout << "为工具栏上的某点(该点坐标是相对于工具栏客户区的)获取插入标记的信息,相关结构:TBINSERTMARK" << std::endl; return true;

	case TB_MAPACCELERATOR: std::cout << "为一个工具栏按钮映射快捷键字符,该字符与按钮文本中加下划线的字符是一样的" << std::endl; return true;
	case TB_MOVEBUTTON: std::cout << "将一个工具栏按钮从一个索引移动到另一个索引" << std::endl; return true;
	case TB_REPLACEBITMAP: std::cout << "以新位图替换工具栏中现有的位图,相关结构:TBREPLACEBITMAP" << std::endl; return true;
	case TB_SAVERESTORE: std::cout << "保存或恢复工具栏的状态,相关结构:TBSAVEPARAMS" << std::endl; return true;
	case TB_SETANCHORHIGHLIGHT: std::cout << "对工具栏进行加亮设置" << std::endl; return true;
	case TB_SETBITMAPSIZE: std::cout << "设置添加到工具栏的位图尺寸" << std::endl; return true;
	case TB_SETBUTTONINFO: std::cout << "设置工具栏指定按钮的信息" << std::endl; return true;
	case TB_SETBUTTONSIZE: std::cout << "设置工具栏的按钮尺寸(尺寸必须与所载入的位图尺寸一样)" << std::endl; return true;
	case TB_SETCMDID: std::cout << "设置工具栏指定按钮的命令标识符(按钮被按下时,标识符将被发送给属主窗口)" << std::endl; return true;
	case TB_SETCOLORSCHEME: std::cout << "设置工具栏的色彩方案信息" << std::endl; return true;
	case TB_SETDRAWTEXTFLAGS: std::cout << "设置工具栏的文本绘制标志" << std::endl; return true;
	case TB_SETEXTENDEDSTYLE: std::cout << "设置工具栏的扩展风格" << std::endl; return true;
	case TB_SETHOTITEM: std::cout << "设置工具栏热点项索引(若无TBSTYLE_FLAT风格,则本消息将会被忽略)" << std::endl; return true;
	case TB_SETIMAGELIST: std::cout << "设置工具栏的默认图像列表" << std::endl; return true;
	case TB_SETINDENT: std::cout << "设置工具栏中首个按钮的缩进" << std::endl; return true;
	case TB_SETINSERTMARK: std::cout << "设置工具栏的当前插入标记" << std::endl; return true;
	case TB_SETINSERTMARKCOLOR: std::cout << "设置工具栏插入标记的颜色" << std::endl; return true;
	case TB_SETMAXTEXTROWS: std::cout << "设置显示在工具栏按钮中的文本的最大行数" << std::endl; return true;
	case TB_SETPADDING: std::cout << "设置工具栏的填充距(像素)(padding:补白、填充距)" << std::endl; return true;
	case TB_SETPARENT: std::cout << "为工具栏指定一个父窗(用于发送通知消息)" << std::endl; return true;
	case TB_SETROWS: std::cout << "设置工具栏按钮的行数" << std::endl; return true;
	case TB_SETTOOLTIPS: std::cout << "将一个工具提示控件与工具栏进行关联" << std::endl; return true;

	case TBN_BEGINADJUST: std::cout << "通知工具栏的父窗:用户已开始自定义一个工具栏,相关结构:NMHDR" << std::endl; return true;
	case TBN_BEGINDRAG: std::cout << "通知工具栏的父窗:用户已开始拖动工具栏的某个按钮,相关结构:NMTOOLBAR" << std::endl; return true;
	case TBN_CUSTHELP: std::cout << "通知工具栏的父窗:用户已经选择了自定义工具栏对话框上的帮助按钮,相关结构:NMHDR" << std::endl; return true;
	case TBN_ENDADJUST: std::cout << "通知工具栏的父窗:用户已停止自定义一个工具栏,相关结构:NMHDR" << std::endl; return true;
	case TBN_ENDDRAG: std::cout << "通知工具栏的父窗:用户已停止拖动工具栏的某个按钮,相关结构:NMTOOLBAR" << std::endl; return true;
	case TBN_GETBUTTONINFO: std::cout << "获取工具栏的自定义信息(通知消息),相关结构:NMTOOLBAR" << std::endl; return true;
	case TBN_QUERYDELETE: std::cout << "通知工具栏的父窗:用户自定义工具栏时一个按钮是否可被删除" << std::endl; return true;
	case TBN_QUERYINSERT: std::cout << "通知工具栏的父窗:用户自定义工具栏时一个按钮是否可被插入到指定按钮的左侧" << std::endl; return true;
	case TBN_RESET: std::cout << "通知工具栏的父窗:用户在自定义工具栏对话框中已经对工具栏进行重置了" << std::endl; return true;
	case TBN_TOOLBARCHANGE: std::cout << "通知工具栏的父窗:用户已经定制完一个工具栏" << std::endl; return true;

	case IPM_CLEARADDRESS: std::cout << "清空IP地址控件中的内容" << std::endl; return true;
	case IPM_GETADDRESS: std::cout << "从IP地址控件的4个域中获取地址值,相关宏:FIRST_IPADDRESS/SECOND_IPADDRESS/THIRD_IPADDRESS/FOURTH_IPADDRESS" << std::endl; return true;
	case IPM_ISBLANK: std::cout << "确定IP地址控件的全部域是否为空" << std::endl; return true;
	case IPM_SETADDRESS: std::cout << "为IP地址控件的4个域设置地址,宏:MAKEIPADDRESS" << std::endl; return true;
	case IPM_SETFOCUS: std::cout << "为IP地址控件指定域设置键盘焦点,同时该域中的文本会被选中" << std::endl; return true;
	case IPM_SETRANGE: std::cout << "为IP地址控件指定域设置有效范围,宏:MAKEIPRANGE" << std::endl; return true;

	case IPN_FIELDCHANGED: std::cout << "用户改变了IP地址控件某域或从一个域移动其它域时发送本通知消息,相关结构NMIPADDRESS" << std::endl; return true;

	case PBM_SETBKCOLOR: std::cout << "设置进度条控件的背景色" << std::endl; return true;


	case TCM_ADJUSTRECT: std::cout << "根据给定的窗口矩形计算标签控件的显示区域,或根据一个给定的显示区域计算相应的窗口矩形" << std::endl; return true;
	case TCM_DELETEALLITEMS: std::cout << "删除标签控件中所有选项卡" << std::endl; return true;
	case TCM_DELETEITEM: std::cout << "从标签控件中删除指定选项卡" << std::endl; return true;
	case TCM_DESELECTALL: std::cout << "重新设置标签控件中的项,清除任何被按下的选项卡" << std::endl; return true;
	case TCM_GETCURFOCUS: std::cout << "获取标签控件中拥有当前焦点的选项卡索引" << std::endl; return true;
	case TCM_GETCURSEL: std::cout << "获取标签控件中当前选择的选项卡" << std::endl; return true;
	case TCM_GETEXTENDEDSTYLE: std::cout << "获取标签控件的扩展风格" << std::endl; return true;
	case TCM_GETIMAGELIST: std::cout << "获取与标签控件相关联的图像列表" << std::endl; return true;
	case TCM_GETITEM: std::cout << "获取标签控件中某选项卡的信息" << std::endl; return true;
	case TCM_GETITEMCOUNT: std::cout << "获取标签控件中的项数" << std::endl; return true;
	case TCM_GETITEMRECT: std::cout << "获取标签控件中指定选项卡的边界矩形" << std::endl; return true;
	case TCM_GETROWCOUNT: std::cout << "获取标签控件中当前行数,注:TCS_MULTILINT风格的标签控件可有多行选项卡" << std::endl; return true;
	case TCM_GETTOOLTIPS: std::cout << "获取与标签控件相关联的工具提示控件的句柄" << std::endl; return true;
	case TCM_HIGHLIGHTITEM: std::cout << "使标签控件的某选项卡处于高亮状态" << std::endl; return true;
	case TCM_HITTEST: std::cout << "确定是哪个选项卡(若有)位于指定的屏幕位置" << std::endl; return true;
	case TCM_INSERTITEM: std::cout << "插入一个新选项卡到标签控件" << std::endl; return true;
	case TCM_REMOVEIMAGE: std::cout << "从标签控件的图像列表中删除指定图像" << std::endl; return true;
	case TCM_SETCURFOCUS: std::cout << "设置焦点到标签控件中指定的选项卡" << std::endl; return true;
	case TCM_SETCURSEL: std::cout << "设置标签控件中当前选择的选项卡" << std::endl; return true;
	case TCM_SETEXTENDEDSTYLE: std::cout << "设置标签控件的扩展风格" << std::endl; return true;
	case TCM_SETIMAGELIST: std::cout << "为标签控件指定一个已创建的图像列表" << std::endl; return true;
	case TCM_SETITEM: std::cout << "设置标签控件中选项卡的某些或所有属性" << std::endl; return true;
	case TCM_SETITEMEXTRA: std::cout << "标签控件为空时,改变标签控件中各选项卡的额外字节数(默认4字节),注:额外字节可把程序数据与各选项卡关联起来" << std::endl; return true;
	case TCM_SETITEMSIZE: std::cout << "设置标签控件中选项卡的宽度和高度" << std::endl; return true;
	case TCM_SETMINTABWIDTH: std::cout << "设置标签控件中选项卡的最小宽度" << std::endl; return true;
	case TCM_SETPADDING: std::cout << "设置标签控件中的每个选项卡的图标和标签周围的空间大小(填充距)" << std::endl; return true;
	case TCM_SETTOOLTIPS: std::cout << "设置与标签控件相关联的工具提示控件(用句柄进行设置)" << std::endl; return true;

	case TCN_GETOBJECT: std::cout << "将对象拖动到标签控件的选项卡上时,标签控件生成TCN_GETOBJECT通知消息以请求放置目标对象,注:须调用AfxOleInit初始化OLE库" << std::endl; return true;
	case TCN_KEYDOWN: std::cout << "通知标签控件的父窗:用户已按下某键" << std::endl; return true;
	case TCN_SELCHANGE: std::cout << "通知标签控件的父窗:选项卡的当前选择已被改变" << std::endl; return true;
	case TCN_SELCHANGING: std::cout << "通知标签控件的父窗:选项卡的当前选择将要改变" << std::endl; return true;

	case EM_GETSEL: std::cout << "获取编辑控件当前被选中部分(若有)的开始和结束位置;返回双字,低位字为起始位置,高位字为首个未被选中的字符位置" << std::endl; return true;
	case EM_SETSEL: std::cout << "在编辑控件中选定文本" << std::endl; return true;
	case EM_GETRECT: std::cout << "获取编辑控件的带格式的文本边界矩形(与其窗口大小无关)" << std::endl; return true;
	case EM_SETRECT: std::cout << "设置编辑控件的带格式的文本边界矩形(与其窗口大小无关)" << std::endl; return true;
	case EM_SETRECTNP: std::cout << "设置多行编辑控件的带格式的文本边界矩形,而不必重绘" << std::endl; return true;
	case EM_SCROLL: std::cout << "使多行编辑控件滚动一行(SB_LINEDOWN/SB_LINEUP)或一页(SB_PAGEDOWN/SB_PAGEUP)" << std::endl; return true;
	case EM_LINESCROLL: std::cout << "以行为单位使多行编辑控件左右或上下滚动" << std::endl; return true;
	case EM_SCROLLCARET: std::cout << "滚动编辑控件中的caret插入光标,使之可视" << std::endl; return true;
	case EM_GETMODIFY: std::cout << "测试编辑控件的内容是否被改变(它有个内部标记来表明其内容是否被改变)" << std::endl; return true;
	case EM_SETMODIFY: std::cout << "设置或清除编辑控件的改变标志;改变标记表明文本是否被改变(用户改变文本时,会自动设置此标志)" << std::endl; return true;
	case EM_GETLINECOUNT: std::cout << "获取多行编辑控件中的总行数" << std::endl; return true;
	case EM_LINEINDEX: std::cout << "获得多行编辑控件中某行的字符索引" << std::endl; return true;
	case EM_SETHANDLE: std::cout << "设置可被多行编辑控件使用的局部内存的句柄,编辑控件可用此缓冲区存储当前显示的文本,而不必自己分配" << std::endl; return true;
	case EM_GETHANDLE: std::cout << "获取多行编辑控件中当前分配的内存句柄,此句柄是个局部内存句柄" << std::endl; return true;
	case EM_GETTHUMB: std::cout << "取得多行文本编辑控件的滚动框的当前位置(象素)" << std::endl; return true;
	case EM_LINELENGTH: std::cout << "获得编辑控件中的行的长度" << std::endl; return true;
	case EM_REPLACESEL: std::cout << "用指定文本覆盖编辑控件中当前被选中的文本" << std::endl; return true;
	case EM_GETLINE: std::cout << "在编辑控件中获得一行文本" << std::endl; return true;
	case EM_LIMITTEXT: std::cout << "用户在编辑控件中输入文本时的文本长度限制" << std::endl; return true;
	case EM_CANUNDO: std::cout << "确定对编辑控件的操作能否撤销" << std::endl; return true;
	case EM_UNDO: std::cout << "撤销上一次对编辑控件的操作" << std::endl; return true;
	case EM_FMTLINES: std::cout << "设置多行编辑控件中是否包含软回车符" << std::endl; return true;
	case EM_LINEFROMCHAR: std::cout << "获取包含指定字符索引的行的行号(字符索引指从开始到指定字符的字符数)" << std::endl; return true;
	case EM_SETTABSTOPS: std::cout << "在多行编辑控件中设置跳格键的跳幅(控件中文本的任何制表键间会产生一段空白)" << std::endl; return true;
	case EM_SETPASSWORDCHAR: std::cout << "设置或清除编辑控件中密码的替换显示字符" << std::endl; return true;
	case EM_EMPTYUNDOBUFFER: std::cout << "清除控件的撤消缓冲区,使其不能撤消上一次编辑操作" << std::endl; return true;
	case EM_GETFIRSTVISIBLELINE: std::cout << "确定编辑控件中可视的最顶端行的行号" << std::endl; return true;
	case EM_SETREADONLY: std::cout << "设置编辑控件的只读状态" << std::endl; return true;
	case EM_SETWORDBREAKPROC: std::cout << "设置编辑控件的新的断字处理回调函数" << std::endl; return true;
	case EM_GETWORDBREAKPROC: std::cout << "获取编辑控件的新的断字处理回调函数" << std::endl; return true;
	case EM_GETPASSWORDCHAR: std::cout << "获取编辑控件中密码的替换显示字符" << std::endl; return true;
	case EM_SETMARGINS: std::cout << "设置编辑控件的左右边空" << std::endl; return true;
	case EM_GETMARGINS: std::cout << "获取编辑控件的左右边空" << std::endl; return true;
	case EM_GETLIMITTEXT: std::cout << "获取编辑控件中文本的输入长度限制" << std::endl; return true;
	case EM_POSFROMCHAR: std::cout << "获得指定字符索引的左上角的坐标" << std::endl; return true;
	case EM_CHARFROMPOS: std::cout << "获得编辑控件中最靠近指定位置的字符的行和字符索引" << std::endl; return true;
	case EM_SETIMESTATUS: std::cout << "修改编辑控件的IME输入法的属性" << std::endl; return true;
	case EM_GETIMESTATUS: std::cout << "获取编辑控件的IME输入法的属性" << std::endl; return true;

	case EN_ERRSPACE: std::cout << "编辑控件不能为特定请求分配足够的空间的通知消息" << std::endl; return true;
	case EN_MAXTEXT: std::cout << "通知父窗,编辑控件当前输入文本已超过指定字符数(并作截尾处理)" << std::endl; return true;
	case EN_HSCROLL: std::cout << "用户单击了编辑控件的水平滚动条,父窗在屏幕更新之前被通知" << std::endl; return true;
	case EN_VSCROLL: std::cout << "用户单击了编辑控件的垂直滚动条,父窗在屏幕更新之前被通知" << std::endl; return true;


	case MCN_GETDAYSTATE: std::cout << "获取月历控件的日期显示风格(粗体/圈定等),要有MCS_DAYSTATE风格,相关结构:NMDAYSTATE" << std::endl; return true;
	case MCN_SELCHANGE: std::cout << "当前选择项已被改变,相关结构:NMSELCHANGE" << std::endl; return true;
	case MCN_SELECT: std::cout << "在月历控件中选择一个日期,相关结构:NMSELCHANGE" << std::endl; return true;
	case NM_RELEASEDCAPTURE: std::cout << "释放月历控件中鼠标的捕获消息,相关结构:NMHDR" << std::endl; return true;


	case DTN_CLOSEUP: std::cout << "DTP控件中的月历控件将要关闭,相关结构:NMHDR" << std::endl; return true;
	case DTN_DATETIMECHANGE: std::cout << "DTP控件中的内容已发生改变,相关结构:NMDATETIMECHANGE" << std::endl; return true;
	case DTN_DROPDOWN: std::cout << "DTP控件中的月历控件将要显示,相关结构:NMHDR" << std::endl; return true;
	case DTN_FORMAT: std::cout << "DTP控件中的文本需要显示在回调域中,相关结构:NMDATETIMEFORMAT" << std::endl; return true;
	case DTN_FORMATQUERY: std::cout << "DTP控件中的文本显示所需最大尺寸,相关结构:NMDATETIMEFORMATQUERY" << std::endl; return true;
	case DTN_USERSTRING: std::cout << "用户已完成DTP控件的编辑,相关结构:NMDATETIMESTRING" << std::endl; return true;
	case DTN_WMKEYDOWN: std::cout << "用户单击了DTP控件的回调域,相关结构:NMDATETIMEWMKEYDOWN" << std::endl; return true;


	case SBN_SIMPLEMODECHANGE: std::cout << "由于SB_SIMPLE消息导致状态栏简单模式发生改变时发送的通知消息" << std::endl; return true;


	case RBN_AUTOSIZE: std::cout << "当Rebar控件自己自动调整大小时由(用RBS_AUTOSIZE风格创建的)Rebar控件发送" << std::endl; return true;
	case RBN_BEGINDRAG: std::cout << "当用户开始拖动带区时由Rebar控件发送" << std::endl; return true;
	case RBN_CHILDSIZE: std::cout << "当调整带区的子窗口大小时由Rebar控件发送" << std::endl; return true;
	case RBN_DELETEDBAND: std::cout << "在带区已被删除后由Rebar控件发送" << std::endl; return true;
	case RBN_DELETINGBAND: std::cout << "当带区即将被删除时由Rebar控件发送" << std::endl; return true;
	case RBN_ENDDRAG: std::cout << "当用户停止拖动带区时由Rebar控件发送" << std::endl; return true;
	case RBN_GETOBJECT: std::cout << "当对象被拖动到此控件中的带区上由(用RBS_REGISTERDROP风格创建的)Rebar控件发送" << std::endl; return true;
	case RBN_HEIGHTCHANGE: std::cout << "当其高度已被更改时由Rebar控件发送" << std::endl; return true;
	case RBN_LAYOUTCHANGED: std::cout << "当用户更改此控件的带区布局时由Rebar控件发送" << std::endl; return true;

	case PSM_APPLY: std::cout << "应用属性表的新属性,宏:PropSheet_Apply" << std::endl; return true;
	case PSM_CANCELTOCLOSE: std::cout << "使属性表改变确定按钮上的文本为关闭(标志着应用的改变不可被取消),宏:PropSheet_CancelToClose" << std::endl; return true;
	case PSM_GETCURRENTPAGEHWND: std::cout << "获取属性表当前页的窗口句柄,宏:PropSheet_GetCurrentPageHwnd" << std::endl; return true;
	case PSM_GETTABCONTROL: std::cout << "获取属性表中标签控件的句柄,宏:PropSheet_GetTabControl" << std::endl; return true;
	case PSM_ISDIALOGMESSAGE: std::cout << "发送一个消息至属性表对话框,并指明该对话框是否已处理了该消息,宏:PropSheet_IsDialogMessage" << std::endl; return true;
	case PSM_PRESSBUTTON: std::cout << "在属性表中模拟选择某指定按钮,宏:PropSheet_PressButton" << std::endl; return true;
	case PSM_QUERYSIBLINGS: std::cout << "查询属性表的兄弟页,相互传递参数,宏:PropSheet_QuerySiblings" << std::endl; return true;
	case PSM_REBOOTSYSTEM: std::cout << "指明需重启系统以使改变生效(若某页改变了系统配置),宏:PropSheet_RebootSystem" << std::endl; return true;
	case PSM_SETCURSELID: std::cout << "根据页ID选择(激活)属性表中某页,宏:PropSheet_SetCurSelByID" << std::endl; return true;
	case PSM_SETFINISHTEXT: std::cout << "设置属性表中Finish按钮的文本,宏:PropSheet_SetFinishText" << std::endl; return true;
	case PSM_SETTITLE: std::cout << "指定属性表的标题,宏:PropSheet_SetTitle" << std::endl; return true;
	case PSM_SETWIZBUTTONS: std::cout << "使向导属性表中的Back,Next或Finish按钮有效或无效,宏:PropSheet_SetWizButtons" << std::endl; return true;
	case PSM_UNCHANGED: std::cout << "禁止应用按钮,重新初始化属性表,宏:PropSheet_UnChanged" << std::endl; return true;

	case PSN_APPLY: std::cout << "属性页的应用按钮被按下,相关结构:PSHNOTIFY" << std::endl; return true;
	case PSN_GETOBJECT: std::cout << "通知允许属性页进行OLE拖放对象,相关结构:NMOBJECTNOTIFY" << std::endl; return true;
	case PSN_HELP: std::cout << "用户点击了属性页的帮助按扭,相关结构:PSHNOTIFY" << std::endl; return true;
	case PSN_KILLACTIVE: std::cout << "属性页失去焦点(其它属性页获得了焦点或用户点击了确定按钮),相关结构:PSHNOTIFY" << std::endl; return true;
	case PSN_QUERYCANCEL: std::cout << "用户点击了属性页的取消按扭,相关结构:PSHNOTIFY" << std::endl; return true;
	case PSN_RESET: std::cout << "用户点击了取消按扭,属性页将被销毁,相关结构:PSHNOTIFY" << std::endl; return true;
	case PSN_SETACTIVE: std::cout << "某属性页被激活获得焦点,相关结构:PSHNOTIFY" << std::endl; return true;
	case PSN_WIZBACK: std::cout << "用户点击了向导属性表的前一页按钮,相关结构:PSHNOTIFY" << std::endl; return true;
	case PSN_WIZFINISH: std::cout << "用户点击了向导属性表的结束按钮,相关结构:PSHNOTIFY" << std::endl; return true;
	case PSN_WIZNEXT: std::cout << "用户点击了向导属性表的下一页按钮,相关结构:PSHNOTIFY" << std::endl; return true;


	case CBEN_BEGINEDIT: std::cout << "用户激活下拉列表或点击CComboBoxEx控件的编辑框,相关结构:NMHDR" << std::endl; return true;
	case CBEN_DELETEITEM: std::cout << "CComboBoxEx控件的某条目已被删除,相关结构:NMCOMBOBOXEX" << std::endl; return true;
	case CBEN_DRAGBEGIN: std::cout << "用户开始拖动CComboBoxEx控件的编辑框中的条目图像,相关结构:NMCBEDRAGBEGIN" << std::endl; return true;
	case CBEN_ENDEDIT: std::cout << "用户已结束对CComboBoxEx控件的编辑框的操作或选择了下拉列表,相关结构:NMCBEENDEDIT" << std::endl; return true;
	case CBEN_GETDISPINFO: std::cout << "获取CComboBoxEx控件的回调条目的显示信息,相关结构:NMCOMBOBOXEX" << std::endl; return true;
	case CBEN_INSERTITEM: std::cout << "一个新条目已插入到CComboBoxEx控件,相关结构:NMCOMBOBOXEX" << std::endl; return true;

	case NM_CHAR: std::cout << "字符键被处理后由控件发送,相关结构:NMCHAR" << std::endl; return true;
	case NM_DBLCLK: std::cout << "通知父窗:用户在控件上双击了鼠标左键,相关结构:NMHDR" << std::endl; return true;
	case NM_HOVER: std::cout << "用户的鼠标掠过控件,相关结构:NMHDR" << std::endl; return true;
	case NM_KEYDOWN: std::cout << "控件获得键盘焦点并按下某键,相关结构:NMKEY" << std::endl; return true;
	case NM_KILLFOCUS: std::cout << "通知父窗:用户失去输入焦点,相关结构:NMHDR" << std::endl; return true;
	case NM_NCHITTEST: std::cout << "控件收到WM_NCHITTEST消息,相关结构:NMMOUSE" << std::endl; return true;
	case NM_RCLICK: std::cout << "通知父窗:用户在控件上点击了鼠标右键,相关结构:NMHDR" << std::endl; return true;
	case NM_RDBLCLK: std::cout << "通知父窗:用户在控件上双击了鼠标右键,相关结构:NMHDR" << std::endl; return true;
	case NM_RETURN: std::cout << "通知父窗:控件获得键盘焦点并按下回车键,相关结构:NMHDR" << std::endl; return true;
	case NM_SETCURSOR: std::cout << "通知父窗:控件响应WM_SETCURSOR要设置光标,相关结构:NMMOUSE" << std::endl; return true;
	case NM_SETFOCUS: std::cout << "通知父窗:控件收到输入焦点,相关结构:NMHDR" << std::endl; return true;
	case NM_TOOLTIPSCREATED: std::cout << "通知父窗:控件已创建一个工具提示控件,相关结构:NMTOOLTIPSCREATED" << std::endl; return true;

	case UDM_SETRANGE32: std::cout << "设置旋转按钮的上下限范围(32位DWORD值)" << std::endl; return true;

	case UDN_DELTAPOS: std::cout << "微调按钮控件的位置将要改变(向其父窗口发送),相关结构:NMUPDOWN" << std::endl; return true;

	case PGM_FORWARDMOUSE: std::cout << "启用或禁用Pager分页控件的鼠标消息转发,若启用则Pager控件转发WM_MOUSEMOVE消息给包含的窗口,宏:Pager_ForwardMouse" << std::endl; return true;
	case PGM_GETBKCOLOR: std::cout << "获取Pager分页控件的当前背景色,宏:Pager_GetBkColor" << std::endl; return true;
	case PGM_GETBORDER: std::cout << "获取Pager分页控件的当前边界尺寸,宏:Pager_GetBorder" << std::endl; return true;
	case PGM_GETBUTTONSIZE: std::cout << "获取Pager分页控件的当前按钮大小,宏:Pager_GetButtonSize" << std::endl; return true;
	case PGM_GETBUTTONSTATE: std::cout << "获取Pager分页控件中指定按钮的状态,宏:Pager_GetButtonState" << std::endl; return true;
	case PGM_GETDROPTARGET: std::cout << "获取Pager分页控件的IDropTarget接口指针,宏:Pager_GetDropTarget" << std::endl; return true;
	case PGM_GETPOS: std::cout << "获取Pager分页控件的滚动条的当前位置,宏:Pager_GetPos" << std::endl; return true;
	case PGM_RECALCSIZE: std::cout << "强制重新计算Pager分页控件包含的窗口的大小,宏:Pager_RecalcSize" << std::endl; return true;
	case PGM_SETBKCOLOR: std::cout << "设置Pager分页控件的当前背景色,宏:Pager_SetBkColor" << std::endl; return true;
	case PGM_SETBORDER: std::cout << "设置Pager分页控件的当前边界尺寸,宏:Pager_SetBorder" << std::endl; return true;
	case PGM_SETBUTTONSIZE: std::cout << "设置Pager分页控件的当前按钮大小,宏:Pager_SetButtonSize" << std::endl; return true;
	case PGM_SETCHILD: std::cout << "设置Pager分页控件中包含的窗口,宏:Pager_SetChild" << std::endl; return true;
	case PGM_SETPOS: std::cout << "获取Pager分页控件的滚动条的当前位置,宏:Pager_SetPos" << std::endl; return true;

	case PGN_CALCSIZE: std::cout << "Pager控件发送的通知消息,用以获得所包含窗口的滚动尺寸,相关结构:NMPGCALCSIZE" << std::endl; return true;
	case PGN_SCROLL: std::cout << "在所包含窗口被滚动之前,Pager控件发送的通知消息,相关结构:NMPGSCROLL" << std::endl; return true;
	default: return false;
	}
}


/* 图形算法父类 */
GraphicsAlgorithm::GraphicsAlgorithm() {
}

GraphicsAlgorithm::~GraphicsAlgorithm() {
}

void GraphicsAlgorithm::SetAlgorithmName(WCHAR* AghmName) {
	AlgorithmName = AghmName;
}

WCHAR* GraphicsAlgorithm::GetAlgorithmName() {
	return AlgorithmName;
}

/* 圆形统计图算法类 */
CircularChart::CircularChart() {
	SetAlgorithmName(L"CircularChart");
}

CircularChart::~CircularChart() {
}

WCHAR* CircularChart::GetGAName(){
	return GetAlgorithmName();
}
