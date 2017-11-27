/* �ļ�����LeftWidnows.cpp */



#include "LeftWindows.h"

#include <shlobj.h>
#include <iostream>



// ����
LeftWindows::LeftWindows(HINSTANCE hInstance, UINT ClsStyle, WCHAR* ClsName,
	LRESULT CALLBACK ExWinProc(HWND, UINT, WPARAM, LPARAM)) {
	/* ��ʼ�������� */
	if (!LeftWindows::staticInit)
		Init();
	pWndClsEx = LeftWindows::GetDefWndClsEx();
	pWndClsEx->hInstance = hInstance;
	pWndClsEx->style |= ClsStyle;
	pWndClsEx->lpszClassName = ClsName ? ClsName : LeftWindows::DefClsName;
	pWndClsEx->lpfnWndProc = ExWinProc ? ExWinProc : LeftWindows::ExWinProc;
	RegClass = RegisterClassEx(pWndClsEx);

	/* ��ʼ������������ */
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

// ����
LeftWindows::~LeftWindows() {
	delete[] pWndClsEx;
}

// ��ʼ����̬����
WCHAR LeftWindows::DefClsName[] = L"default";
bool LeftWindows::staticInit = false;
/* ����֧�����ڲ�delete��ͬ���� */
HANDLE LeftWindows::DeleteSureLock = CreateEvent(NULL, FALSE, TRUE, NULL);

// LeftWindows�Ļ�����ʼ������
void LeftWindows::Init() {
	setlocale(LC_ALL, "");
	LeftWindows::staticInit = true;
}

/*��ExWinProc����ʾ�Ϸ��¼�*/
void LeftWindows::DefHandDropFiles(HDROP hDrop) {
	UINT FileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);
	std::cout << "�յ����Ϸ��ļ�����: " << FileCount << std::endl;
	WCHAR buf[1991];
	for (UINT i = 0; i < FileCount; i++) {
		if (DragQueryFile(hDrop, i, (LPTSTR)buf, 1990 * sizeof(WCHAR)) > 1990) {
			std::cout << "�ļ�������" << std::endl;
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
	//�����豸������(HDC)
	hBufDC = CreateCompatibleDC(hDC);
	//����HBITMAP
	hBitmap = CreateCompatibleBitmap(hDC, BitWidth, BitHeight);
	hBitTemp = (HBITMAP)SelectObject(hBufDC, hBitmap);
	//�õ�λͼ������
	StretchBlt(hBufDC, 0, 0, BitWidth, BitHeight,
		hDC, 0, 0, BitWidth, BitHeight, SRCCOPY);
	//�õ����յ�λͼ��Ϣ
	hBitmap = (HBITMAP)SelectObject(hBufDC, hBitTemp);
	//�ͷ��ڴ�
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
	case WM_DROPFILES:						// �ļ��Ϸ���Ϣ
		std::cout << "�յ��ļ��Ϸ���Ϣ" << std::endl;
		LeftWindows::DefHandDropFiles((HDROP)wParam);
		DragFinish((HDROP)wParam);
		break;
	case WM_DESTROY:						// ���ٴ�����Ϣ
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
	case WM_NULL: std::cout << "����Ϣ,�ɼ������Ƿ�����Ӧ��" << std::endl; return true;
	case WM_CREATE: std::cout << "�½�һ������" << std::endl; return true;
	case WM_DESTROY: std::cout << "����һ������" << std::endl; return true;

#ifndef SIMPLEMSG
	case WM_MOVE: std::cout << "�ƶ�һ������" << std::endl; return true;
	case WM_SIZE: std::cout << "�ı�һ�����ڵĴ�С" << std::endl; return true;
#endif
	case WM_ACTIVATE: std::cout << "һ�����ڱ������ʧȥ����״̬" << std::endl; return true;
	case WM_SETFOCUS: std::cout << "������ת��һ������" << std::endl; return true;
	case WM_KILLFOCUS: std::cout << "ʹһ������ʧȥ����" << std::endl; return true;

	case WM_ENABLE: std::cout << "ʹһ�����ڴ��ڿ���״̬" << std::endl; return true;
	case WM_SETREDRAW: std::cout << "���ô����Ƿ����ػ�" << std::endl; return true;
	case WM_SETTEXT: std::cout << "����һ�����ڵ��ı�" << std::endl; return true;
	case WM_GETTEXT: std::cout << "���ƴ��ڵ��ı���������" << std::endl; return true;
	case WM_GETTEXTLENGTH: std::cout << "�õ����ڵ��ı�����(����������)" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_PAINT: std::cout << "�����ػ�" << std::endl; return true;
#endif
	case WM_CLOSE: std::cout << "�û��رմ���ʱ�ᷢ�ͱ���Ϣ,�����Żᷢ��WM_DESTROY��Ϣ" << std::endl; return true;
	case WM_QUERYENDSESSION: std::cout << "�ػ���ע��ʱϵͳ�ᰴ���ȼ��������̷���WM_QUERYENDSESSION,����Ӧ�ó���Ҫ�ػ���ע����" << std::endl; return true;
	case WM_QUIT: std::cout << "�ر���Ϣѭ���������������" << std::endl; return true;
	case WM_QUERYOPEN: std::cout << "��С���Ĵ��ڼ������ָ���ǰ�Ĵ�Сλ��" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_ERASEBKGND: std::cout << "��һ�����ڵı������뱻����ʱ����Ϣ�ᱻ����(��:���ڴ�С�ı�ʱ)" << std::endl; return true;
#endif
	case WM_SYSCOLORCHANGE: std::cout << "��ϵͳ��ɫ�ı�ʱ,���ͱ���Ϣ�����ж�������" << std::endl; return true;
	case WM_ENDSESSION: std::cout << "�ػ���ע��ʱϵͳ�ᷢ��WM_QUERYENDSESSION��Ϣ,Ȼ�󽫱���Ϣ���͸�Ӧ�ó���,֪ͨ����Ự����" << std::endl; return true;

	case WM_SHOWWINDOW: std::cout << "���ͱ���Ϣ��һ������,�Ա����ػ���ʾ�ô���" << std::endl; return true;

	case WM_WININICHANGE: std::cout << "��д\"win.ini\"ʱ�ᷢ�ͱ���Ϣ�����ж��㴰��,֪ͨ�������̸��ļ��ѱ�����" << std::endl; return true;
	case WM_DEVMODECHANGE: std::cout << "�ı��豸ģʽ����(\"win.ini\")ʱ,������Ϣ��Ӧ�ó�������³�ʼ�����ǵ��豸ģʽ����" << std::endl; return true;
	case WM_ACTIVATEAPP: std::cout << "���ڽ��̼���״̬�Ķ�,��������Ĵ������ڲ�ͬ��Ӧ�ó��� " << std::endl; return true;
	case WM_FONTCHANGE: std::cout << "��ϵͳ��������Դ��仯ʱ���ͱ���Ϣ�����ж�������" << std::endl; return true;
	case WM_TIMECHANGE: std::cout << "��ϵͳ��ʱ��仯ʱ���ͱ���Ϣ�����ж�������" << std::endl; return true;
	case WM_CANCELMODE: std::cout << "���ͱ���Ϣ��ȡ��ĳ�����ڽ��е�ģ̬(����)(����ʾ����),����:����һ��ģ̬����ʱ,�������յ�����Ϣ;����Ϣ�޲���" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_SETCURSOR: std::cout << "���������ĳ�������ƶ������û������ʱ,�ͻᷢ�ͱ���Ϣ��ĳ������" << std::endl; return true;
#endif
	case WM_MOUSEACTIVATE: std::cout << "���������ĳ��δ�������,���û�����������ĳ����ʱ,�ᷢ�ͱ���Ϣ����ǰ����" << std::endl; return true;
	case WM_CHILDACTIVATE: std::cout << "������ڱ������򵱴��ڱ�����ƶ�����С�ı�ʱ,�ᷢ�ͱ���Ϣ��MDI�Ӵ���" << std::endl; return true;
	case WM_QUEUESYNC: std::cout << "����Ϣ�ɻ��ڼ������ѵ��������,ͨ��WH_JOURNALPALYBACK��HOOK���������û�������Ϣ" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_GETMINMAXINFO: std::cout << "�����ڽ�Ҫ�ı��С��λ��ʱ,��ϵͳ���ͱ���Ϣ������,�û��϶�һ�������ô�С�Ĵ���ʱ��ᷢ������Ϣ" << std::endl; return true;
#endif

	case WM_PAINTICON: std::cout << "��һ����С���Ĵ���ͼ�꽫���ػ�ʱ���ͱ���Ϣ" << std::endl; return true;
	case WM_ICONERASEBKGND: std::cout << "����Ϣ���͸�ĳ����С���Ĵ���,�������ڻ�ͼ��ǰ���ı������뱻�ػ�" << std::endl; return true;
	case WM_NEXTDLGCTL: std::cout << "���ͱ���Ϣ��һ���Ի�����򴰿ڹ���,�Ա��ڸ��ؼ������ü��̽���λ��" << std::endl; return true;

	case WM_SPOOLERSTATUS: std::cout << "ÿ����ӡ�����ж����ӻ����һ����ҵʱ�ͻᷢ������Ϣ" << std::endl; return true;
	case WM_DRAWITEM: std::cout << "��ť����Ͽ��б�򡢲˵�����۸ı�ʱ�ᷢ�ͱ���Ϣ����Щ�ؼ���������" << std::endl; return true;
	case WM_MEASUREITEM: std::cout << "��ť����Ͽ��б���б�ؼ����˵������ʱ�ᷢ�ͱ���Ϣ����Щ�ؼ���������" << std::endl; return true;
	case WM_DELETEITEM: std::cout << "���б�����Ͽ����ٻ�ͨ��LB_DELETESTRING��LB_RESETCONTENT��CB_DELETESTRING��CB_RESETCONTENT��Ϣɾ��ĳЩ��ʱ,�ᷢ�ͱ���Ϣ����Щ�ؼ���������" << std::endl; return true;
	case WM_VKEYTOITEM: std::cout << "LBS_WANTKEYBOARDINPUT�����б��ᷢ������Ϣ����������,�Ա���ӦWM_KEYDOWN��Ϣ" << std::endl; return true;
	case WM_CHARTOITEM: std::cout << "LBS_WANTKEYBOARDINPUT�����б��ᷢ�ͱ���Ϣ����������,�Ա���ӦWM_CHAR��Ϣ" << std::endl; return true;
	case WM_SETFONT: std::cout << "ָ���ؼ���������" << std::endl; return true;
	case WM_GETFONT: std::cout << "�õ���ǰ�ؼ��������ı����õ�����" << std::endl; return true;
	case WM_SETHOTKEY: std::cout << "Ϊĳ���ڹ���һ���ȼ�" << std::endl; return true;
	case WM_GETHOTKEY: std::cout << "ȷ��ĳ�ȼ���ĳ�����Ƿ������" << std::endl; return true;



	case WM_QUERYDRAGICON: std::cout << "����Ϣ���͸���С���Ĵ���(iconic),���ô��ڽ����ϷŶ��䴰������û�ж���ͼ��,Ӧ�ó����ܷ���һ��ͼ�����ľ�������û��Ϸ�ͼ��ʱϵͳ����ʾ���ͼ�����" << std::endl; return true;

	case WM_COMPAREITEM: std::cout << "�ɷ��ͱ���Ϣ��ȷ����Ͽ�(CBS_SORT)���б��(LBS_SORT)������������λ��" << std::endl; return true;



	case WM_GETOBJECT: std::cout << "\"oleacc.dll\"(COM���)(Microsoft Active Accessibility:����м���ʹ�õ��Ե�һ�ּ���)���ͱ���Ϣ�����������Ա��ȡ���������Ĺ����������Ϣ" << std::endl; return true;



	case WM_COMPACTING: std::cout << "��ʾ�ڴ��Ѿ�������" << std::endl; return true;


	case WM_COMMNOTIFY: std::cout << "Win3.1��,�������¼�����ʱ,ͨѶ�豸������������Ϣ����Ϣ��ϵͳ,ָʾ����������е�״̬" << std::endl; return true;

#ifndef SIMPLEMSG
	case WM_WINDOWPOSCHANGING: std::cout << "����Ϣ�ᷢ�͸���Щ��С��λ��(Z_Order)�����ı�Ĵ���,�Ե���SetWindowPos�������������ڹ�����" << std::endl; return true;
	case WM_WINDOWPOSCHANGED: std::cout << "����Ϣ�ᷢ�͸���Щ��С��λ��(Z_Order)�ѱ��ı�Ĵ���,�Ե���SetWindowPos�������������ڹ�����" << std::endl; return true;
#endif
	case WM_POWER: std::cout << "��ϵͳ��Ҫ������ͣ״̬ʱ���ͱ���Ϣ(������16λ��windows)" << std::endl; return true;

	case WM_COPYDATA: std::cout << "��һ��Ӧ�ó��򴫵����ݸ���һ��Ӧ�ó���ʱ���ͱ���Ϣ" << std::endl; return true;
	case WM_CANCELJOURNAL: std::cout << "���û�ȡ��������־����״̬ʱ,���ͱ���Ϣ���Ǹ�Ӧ�ó��򡣸���Ϣʹ�ÿմ��ھ������" << std::endl; return true;


	case WM_NOTIFY: std::cout << "��ĳ�ؼ���ĳ�¼��ѷ�����ÿؼ���õ�һЩ��Ϣʱ,���ͱ���Ϣ���丸��" << std::endl; return true;

	case WM_INPUTLANGCHANGEREQUEST: std::cout << "���û�ͨ���������������ϵ�����ָʾ����ĳ��ݼ����ѡ��ı����뷨ʱϵͳ���򽹵㴰�ڷ��ͱ���Ϣ" << std::endl; return true;
	case WM_INPUTLANGCHANGE: std::cout << "�л����뷨��,ϵͳ�ᷢ�ͱ���Ϣ����Ӱ��Ķ��㴰��" << std::endl; return true;
	case WM_TCARD: std::cout << "�����ѳ�ʼ��windows��������ʱ�ᷢ�ͱ���Ϣ��Ӧ�ó���" << std::endl; return true;
	case WM_HELP: std::cout << "����<F1>��,��ĳ�˵��Ǽ����,�ͷ��ͱ���Ϣ���˴��ڹ����Ĳ˵�;����ͷ��͸��н���Ĵ���;����ǰ��û�н���,�Ͱѱ���Ϣ���͸���ǰ����Ĵ���" << std::endl; return true;
	case WM_USERCHANGED: std::cout << "���û��ѵ�����˳����ͱ���Ϣ�����д���;���û�������˳�ʱϵͳ�����û��ľ���������Ϣ,���û���������ʱϵͳ���Ϸ��ͱ���Ϣ" << std::endl; return true;
	case WM_NOTIFYFORMAT: std::cout << "���ÿؼ����Զ���ؼ����丸��ͨ������Ϣ�жϿؼ���WM_NOTIFY֪ͨ��Ϣ����ʹ��ANSI����UNICODE,ʹ�ñ���Ϣ��ʹĳ���ؼ������ĸ��ؼ�������໥ͨ��" << std::endl; return true;
	case WM_CONTEXTMENU: std::cout << "���û���ĳ�����е���Ҽ��ͷ��ͱ���Ϣ���ô���,�����Ҽ��˵�" << std::endl; return true;
	case WM_STYLECHANGING: std::cout << "������SetWindowLong������Ҫ�ı�һ���������ڵķ��ʱ,���ͱ���Ϣ���Ǹ�����" << std::endl; return true;
	case WM_STYLECHANGED: std::cout << "������SetWindowLong�����ı�һ���������ڵķ���,���ͱ���Ϣ���Ǹ�����" << std::endl; return true;
	case WM_DISPLAYCHANGE: std::cout << "����ʾ���ķֱ��ʸı��,���ͱ���Ϣ�����д���" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_GETICON: std::cout << "����Ϣ���͸�ĳ������,���ڷ�����ĳ�����й����Ĵ�ͼ���Сͼ��ľ��" << std::endl; return true;
#endif
	case WM_SETICON: std::cout << "Ӧ�ó����ͱ���Ϣ��һ���µĴ�ͼ���Сͼ����ĳ���������" << std::endl; return true;
	case WM_NCCREATE: std::cout << "��ĳ�����״α�����ʱ,����Ϣ��WM_CREATE��Ϣ����ǰ����" << std::endl; return true;
	case WM_NCDESTROY: std::cout << "����Ϣ֪ͨĳ����,�ǿͻ�����������" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_NCCALCSIZE: std::cout << "��ĳ���ڵĿͻ����Ĵ�С��λ���뱻����ʱ���ͱ���Ϣ" << std::endl; return true;
	case WM_NCHITTEST: std::cout << "���û����ڷǿͻ����ƶ���ꡢ��ס���ͷ����ʱ���ͱ���Ϣ(���в���);�����û�б�����,����Ϣ�ڴ��ڵõ����֮�󷢳�,������Ϣ���͵��������Ĵ���" << std::endl; return true;
	case WM_NCPAINT: std::cout << "�����ڿ��(�ǿͻ���)���뱻���ػ�ʱ,Ӧ�ó����ͱ���Ϣ���ô���" << std::endl; return true;
#endif
	case WM_NCACTIVATE: std::cout << "����Ϣ���͸�ĳ����,�ڴ��ڵķǿͻ���������ʱ�ػ洰��" << std::endl; return true;
	case WM_GETDLGCODE: std::cout << "���ͱ���Ϣ��ĳ����Ի����������Ŀؼ�,ϵͳ���Ʒ�λ����TAB��ʹ�������ÿؼ�,ͨ����Ӧ����ϢӦ�ó���ɰ�������һ�����������ؼ����ܴ�����" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_SYNCPAINT: std::cout << "��������ϵ������GUI�߳�ʱ,����Ϣ����ͬ��ˢ��,����Ϣ��ϵͳȷ���Ƿ���" << std::endl; return true;
	case WM_NCMOUSEMOVE: std::cout << "�������ĳ���ڵķǿͻ������ƶ�ʱ,���ͱ���Ϣ���ô���" << std::endl; return true;
#endif
	case WM_NCLBUTTONDOWN: std::cout << "�������ĳ���ڵķǿͻ����ڵ�ͬʱ����������,�ᷢ�ͱ���Ϣ" << std::endl; return true;
	case WM_NCLBUTTONUP: std::cout << "���û��ͷ���������ͬʱ�����ĳ���ڵķǿͻ�����ʱ,�ᷢ�ͱ���Ϣ" << std::endl; return true;
	case WM_NCLBUTTONDBLCLK: std::cout << "���û�˫����������ͬʱ�����ĳ���ڵķǿͻ�����ʱ,�ᷢ�ͱ���Ϣ" << std::endl; return true;
	case WM_NCRBUTTONDOWN: std::cout << "���û���������Ҽ���ͬʱ�����ĳ���ڵķǿͻ�����ʱ,�ᷢ�ͱ���Ϣ" << std::endl; return true;
	case WM_NCRBUTTONUP: std::cout << "���û��ͷ�����Ҽ���ͬʱ�����ĳ���ڵķǿͻ�����ʱ,�ᷢ�ͱ���Ϣ" << std::endl; return true;
	case WM_NCRBUTTONDBLCLK: std::cout << "���û�˫������Ҽ���ͬʱ�����ĳ���ڵķǿͻ�����ʱ,�ᷢ�ͱ���Ϣ" << std::endl; return true;
	case WM_NCMBUTTONDOWN: std::cout << "���û���������м���ͬʱ�����ĳ���ڵķǿͻ�����ʱ,�ᷢ�ͱ���Ϣ" << std::endl; return true;
	case WM_NCMBUTTONUP: std::cout << "���û��ͷ�����м���ͬʱ�����ĳ���ڵķǿͻ�����ʱ,�ᷢ�ͱ���Ϣ" << std::endl; return true;
	case WM_NCMBUTTONDBLCLK: std::cout << "���û�˫������м���ͬʱ�����ĳ���ڵķǿͻ�����ʱ,�ᷢ�ͱ���Ϣ" << std::endl; return true;

	case SBM_SETPOS: std::cout << "���ù������ϻ����λ��" << std::endl; return true;
	case SBM_GETPOS: std::cout << "��ȡ�������ϻ����λ��" << std::endl; return true;
	case SBM_SETRANGE: std::cout << "���ù��������������Сλ��ֵ" << std::endl; return true;
	case SBM_SETRANGEREDRAW: std::cout << "���ù��������������Сλ��ֵ,���ػ������" << std::endl; return true;
	case SBM_GETRANGE: std::cout << "��ȡ���������������Сλ��ֵ" << std::endl; return true;
	case SBM_ENABLE_ARROWS: std::cout << "���ù��������������Сλ��ֵ,���ػ������" << std::endl; return true;
	case SBM_SETSCROLLINFO: std::cout << "���ù������Ĳ���,����Ϣͨ��SCROLLINFO�ṹָ�����ֲ���,����ָ����Щ�����ɽṹ�е�fMask��Աȷ��" << std::endl; return true;
	case SBM_GETSCROLLINFO: std::cout << "��ȡ�������Ĳ���,���ͱ���Ϣ��,����SCROLLINFO�ṹ�з��ؿؼ��Ķ��ֲ���,��Ȼ�������趨�ṹ��fMask��Ա��ȷ������Ҫȡ����Щ����" << std::endl; return true;

	case BM_GETCHECK: std::cout << "��ȡ��ѡ��ť��ѡ���״̬" << std::endl; return true;
	case BM_SETCHECK: std::cout << "���õ�ѡ��ť��ѡ���״̬" << std::endl; return true;
	case BM_GETSTATE: std::cout << "ȷ����ѡ��ť��ѡ���״̬" << std::endl; return true;
	case BM_SETSTATE: std::cout << "�ı䰴ť�ĸ���״̬" << std::endl; return true;
	case BM_SETSTYLE: std::cout << "�ı䰴ť�ķ��" << std::endl; return true;
	case BM_CLICK: std::cout << "ģ���û������ť" << std::endl; return true;
	case BM_GETIMAGE: std::cout << "��ȡ�밴ť�������ͼƬ���(ͼ���λͼ)" << std::endl; return true;
	case BM_SETIMAGE: std::cout << "�Ѱ�ť��ĳ��ͼƬ�����(ͼ���λͼ),ע:��ť����BS_ICON���" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_KEYFIRST: std::cout << "����WinCEϵͳ,����Ϣ��ʹ��GetMessage��PeekMessage����ʱ,���ڹ��˼�����Ϣ" << std::endl; return true;
	case WM_KEYUP: std::cout << "��һ����ϵͳ�������ͷŵ���ʱ(<ALT>��û�б�����),�ᷢ�ͱ���Ϣ��ӵ�м��̽���Ĵ���" << std::endl; return true;
#endif
	case WM_CHAR: std::cout << "����ĳ����,���ѷ���WM_KEYDOWN��WM_KEYUP��Ϣ,����Ϣ���������µİ������ַ���" << std::endl; return true;
	case WM_DEADCHAR: std::cout << "\"���ַ�\"��Ϣ,��ʹ��TranslateMessage��������WM_KEYUP��Ϣʱ,���ͱ���Ϣ��ӵ�м��̽���Ĵ���,ע:���������,��Щ����ֻ�Ǹ��ַ���������,���������ַ�,�ʳ�\"���ַ�\"" << std::endl; return true;
	case WM_SYSKEYDOWN: std::cout << "���û���ס<ALT>����ͬʱ�ְ���������ʱ,���ͱ���Ϣ��ӵ�н���Ĵ���" << std::endl; return true;
	case WM_SYSKEYUP: std::cout << "���û��ͷ�һ��������ͬʱ<ALT>��������ʱ,���ͱ���Ϣ��ӵ�н���Ĵ���" << std::endl; return true;
	case WM_SYSCHAR: std::cout << "��WM_SYSKEYDOWN��Ϣ��TranslateMessage���������,���ͱ���Ϣ��ӵ�н���Ĵ���,ע:<ALT>��������" << std::endl; return true;
	case WM_SYSDEADCHAR: std::cout << "\"���ַ�\"��Ϣ,��ʹ��TranslateMessage��������WM_SYSKEYDOWN��Ϣʱ,���ͱ���Ϣ��ӵ�м��̽���Ĵ���,ע:���������,��Щ����ֻ�Ǹ��ַ���������,���������ַ�,�ʳ�\"���ַ�\"" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_KEYLAST: std::cout << "����WinCEϵͳ,����Ϣ��ʹ��GetMessage��PeekMessage����ʱ,���ڹ��˼�����Ϣ" << std::endl; return true;
#endif
	case WM_IME_STARTCOMPOSITION: std::cout << "���û���ʼ�������ʱ,ϵͳ�������͸���Ϣ��IME��,IME�򿪱��봰��,ע:���뷨���" << std::endl; return true;
	case WM_IME_ENDCOMPOSITION: std::cout << "���������ʱ,IME���ͱ���Ϣ,�û�����ɽ��ܱ���Ϣ,�Ա��Լ���ʾ�û�����ı���,ע:���뷨���" << std::endl; return true;
	case WM_IME_COMPOSITION: std::cout << "���û��ı��˱���״̬ʱ,���ͱ���Ϣ,Ӧ�ó����ͨ������ImmGetCompositionString������ȡ�µı���״̬" << std::endl; return true;
	case WM_INITDIALOG: std::cout << "��ĳ�Ի��������ʾǰ���ͱ���Ϣ���öԻ������,ͨ���ñ���Ϣ�Կؼ�����һЩ��ʼ��������ִ����������" << std::endl; return true;
	case WM_COMMAND: std::cout << "�û�ѡ��һ���˵��������ĳ�ؼ�����һ��֪ͨ��Ϣ���丸��,��ĳ��ݼ�������ʱ,����Ϣ������" << std::endl; return true;
	case WM_SYSCOMMAND: std::cout << "���û�ѡ��һ��ϵͳ�˵�����û���󻯻���С����ԭ��ر�ʱ�����ڻ��յ�����Ϣ" << std::endl; return true;
	case WM_TIMER: std::cout << "�����˶�ʱ���¼�" << std::endl; return true;
	case WM_HSCROLL: std::cout << "�����ڵı�׼ˮƽ����������һ�������¼�ʱ,���ͱ���Ϣ���ô���" << std::endl; return true;
	case WM_VSCROLL: std::cout << "�����ڵı�׼��ֱ����������һ�������¼�ʱ,���ͱ���Ϣ���ô���" << std::endl; return true;
	case WM_INITMENU: std::cout << "��һ���˵���������ʱ���ͱ���Ϣ,���������û������ĳ�˵������ĳ�˵������������������ʾǰ���Ĳ˵�" << std::endl; return true;
	case WM_INITMENUPOPUP: std::cout << "��һ�������˵����Ӳ˵���������ʱ���ͱ���Ϣ,���������������ʾǰ���Ĳ˵�,ȴ������ȫ���˵�" << std::endl; return true;
	case WM_MENUSELECT: std::cout << "���û�ѡ��һ���˵���ʱ,���ͱ���Ϣ���˵���������(һ���Ǵ���)" << std::endl; return true;
	case WM_MENUCHAR: std::cout << "���˵��ѱ��������û�������ĳ�˵��ַ���(�˵��ַ������������š����»���,��ͬ�ڿ�ݼ�),���ͱ���Ϣ���˵���������" << std::endl; return true;
	case WM_ENTERIDLE: std::cout << "��һ��ģ̬�Ի����˵��������״̬ʱ,���ͱ���Ϣ������������,һ��ģ̬�Ի����˵��������״̬�����ڴ�����һ��������ǰ����Ϣ��,û����Ϣ����Ϣ�ж��еȴ�" << std::endl; return true;
	case WM_MENURBUTTONUP: std::cout << "����Ϣ�������Ϊ�˵����ṩһ����֪�����ĵĲ˵�(����ݲ˵�),ҪΪ�˵�����ʾһ�������Ĳ˵�,��ʹ��TPM_RECURSE��ʶ����TrackPopupMenuEx����" << std::endl; return true;
	case WM_MENUDRAG: std::cout << "���û��϶��˵���ʱ,���ͱ���Ϣ���ϷŲ˵���ӵ����,���ò˵�֧����ק,��ʹ��OLE�ϷŴ���Э�������ϷŲ���,ע:�˵�Ҫ����MNS_DRAGDROP���" << std::endl; return true;
	case WM_MENUGETOBJECT: std::cout << "������������뿪�˵���ʱ,����Ϣ���͸�֧���ϷŵĲ˵���ӵ����,��ؽṹ��:MENUGETOBJECTINFO,ע:�˵�Ҫ����MNS_DRAGDROP���" << std::endl; return true;
	case WM_UNINITMENUPOPUP: std::cout << "��һ�������˵����Ӳ˵�������ʱ,���ͱ���Ϣ" << std::endl; return true;
	case WM_MENUCOMMAND: std::cout << "���û���һ���˵�������ѡ��ʱ,�ᷢ�ͱ���Ϣ,�˵�Ҫ����MNS_NOTIFYBYPOS���(��MENUINFO�ṹ��������)" << std::endl; return true;
	case WM_CTLCOLORMSGBOX: std::cout << "ϵͳ������Ϣ��ǰ���ͱ���Ϣ����Ϣ��������ߴ���,ͨ����Ӧ����Ϣ,�����ߴ��ڿ�ͨ��ʹ�ø����������ʾ�豸�ľ����������Ϣ����ı��ͱ���ɫ" << std::endl; return true;
	case WM_CTLCOLOREDIT: std::cout << "��һ���༭��ؼ���Ҫ������ʱ,���ͱ���Ϣ���丸��;ͨ����Ӧ����Ϣ,�����ߴ��ڿ�ͨ��ʹ�ø����������ʾ�豸�ľ�������ñ༭����ı��ͱ���ɫ" << std::endl; return true;
	case WM_CTLCOLORLISTBOX: std::cout << "��һ���б��ؼ���Ҫ������ǰ,���ͱ���Ϣ���丸��;ͨ����Ӧ����Ϣ,�����ߴ��ڿ�ͨ��ʹ�ø����������ʾ�豸�ľ���������б����ı��ͱ���ɫ" << std::endl; return true;
	case WM_CTLCOLORBTN: std::cout << "����Ϣ���ð�ť�ı���ɫ" << std::endl; return true;
	case WM_CTLCOLORDLG: std::cout << "���öԻ���ı���ɫ,ͨ������WinnApp��ʹ��SetDialogBkColor����ʵ��" << std::endl; return true;
	case WM_CTLCOLORSCROLLBAR: std::cout << "���ù������ı���ɫ" << std::endl; return true;
	case WM_CTLCOLORSTATIC: std::cout << "����һ����̬�ؼ��ı���ɫ" << std::endl; return true;

	case CB_GETEDITSEL: std::cout << "����ȡ����Ͽ��������༭���ӿؼ��е�ǰ��ѡ�е��ַ�������ֹλ��,��Ӧ����:GetEditSel" << std::endl; return true;
	case CB_LIMITTEXT: std::cout << "����������Ͽ��������༭���ӿؼ������ı��ĳ���" << std::endl; return true;
	case CB_SETEDITSEL: std::cout << "����ѡ����Ͽ��������༭���ӿؼ��еĲ����ַ���,��Ӧ����:GetEditSel" << std::endl; return true;
	case CB_ADDSTRING: std::cout << "��������Ͽ�ؼ�׷��һ���б���(�ַ���)" << std::endl; return true;
	case CB_DELETESTRING: std::cout << "����ɾ����Ͽ���ָ�����б���(�ַ���)" << std::endl; return true;
	case CB_DIR: std::cout << "��������Ͽ�ؼ����������б��ؼ������һ���ļ����б��嵥" << std::endl; return true;
	case CB_GETCOUNT: std::cout << "���ڷ�����Ͽ�ؼ����б����������" << std::endl; return true;
	case CB_GETCURSEL: std::cout << "���ڷ��ص�ǰ��ѡ���������" << std::endl; return true;
	case CB_GETLBTEXT: std::cout << "��ȡ��Ͽ�ؼ���ָ���б�����ַ���" << std::endl; return true;
	case CB_GETLBTEXTLEN: std::cout << "���ڷ�����Ͽ�ؼ���ָ���б�����ַ����ĳ���(������������0)" << std::endl; return true;
	case CB_INSERTSTRING: std::cout << "����Ͽ�ؼ����������б���ӿؼ��е�ָ��λ�ò����ַ���" << std::endl; return true;
	case CB_RESETCONTENT: std::cout << "���������Ͽ�ؼ������б���" << std::endl; return true;
	case CB_FINDSTRING: std::cout << "����Ͽ�ؼ��и��ݸ������ַ�������ƥ���ַ���(���Դ�Сд)" << std::endl; return true;
	case CB_SELECTSTRING: std::cout << "����Ͽ�ؼ����趨��ָ���ַ�����ƥ����б���Ϊѡ����" << std::endl; return true;
	case CB_SETCURSEL: std::cout << "����Ͽ�ؼ�������ָ�����б���Ϊ��ǰѡ����" << std::endl; return true;
	case CB_SHOWDROPDOWN: std::cout << "������ʾ��ر�����ʽ�������б�ʽ��Ͽ���б��" << std::endl; return true;
	case CB_GETITEMDATA: std::cout << "��Ͽ�ؼ�ÿ���б����һ��32λ�ĸ�������,����Ϣ���ڷ���ָ���б���ĸ�������" << std::endl; return true;
	case CB_SETITEMDATA: std::cout << "���ڸ�����Ͽ����������б����ָ�����б����32λ��������" << std::endl; return true;
	case CB_GETDROPPEDCONTROLRECT: std::cout << "����ȡ��Ͽ����б�����Ļλ��" << std::endl; return true;
	case CB_SETITEMHEIGHT: std::cout << "ָ����Ͽ����б������ʾ�߶�(���ص�)" << std::endl; return true;
	case CB_GETITEMHEIGHT: std::cout << "���ڷ���ָ���б���ĸ߶�(���ص�)" << std::endl; return true;
	case CB_SETEXTENDEDUI: std::cout << "ָ������ʽ�������б�ʽ��Ͽ���ʹ��Ĭ�Ͻ��滹����չ����" << std::endl; return true;
	case CB_GETEXTENDEDUI: std::cout << "���ڷ�����Ͽ��Ƿ������չ����" << std::endl; return true;
	case CB_GETDROPPEDSTATE: std::cout << "����ȡ��Ͽ����б���Ƿ�ɼ�" << std::endl; return true;
	case CB_FINDSTRINGEXACT: std::cout << "����Ͽ��в����ַ���,���Դ�Сд,��CB_FINDSTRING��ͬ,����Ϣ���������ַ�����ͬ" << std::endl; return true;
	case CB_SETLOCALE: std::cout << "������Ͽ��б��ǰ������������Դ���" << std::endl; return true;
	case CB_GETLOCALE: std::cout << "��ȡ��Ͽ��б��ǰ������������Դ���" << std::endl; return true;
	case CB_GETTOPINDEX: std::cout << "��ȡ��Ͽ����б���е�һ���ɼ��������" << std::endl; return true;
	case CB_SETTOPINDEX: std::cout << "���ڽ�ָ������Ͽ����б�������Ϊ�б��ĵ�һ���ɼ���" << std::endl; return true;
	case CB_GETHORIZONTALEXTENT: std::cout << "���ڷ�����Ͽ�ˮƽ�������ܿ��(���ص�)(Ҫ����ˮƽ������)" << std::endl; return true;
	case CB_SETHORIZONTALEXTENT: std::cout << "�����趨��Ͽ��е��б��Ŀ��" << std::endl; return true;
	case CB_GETDROPPEDWIDTH: std::cout << "ȡ��Ͽ����б��Ŀ��(���ص�)" << std::endl; return true;
	case CB_SETDROPPEDWIDTH: std::cout << "�����趨��Ͽ��е��б������������" << std::endl; return true;
	case CB_INITSTORAGE: std::cout << "ֻ������Win95,����Ҫ����Ͽ����б���м����������ʱ,����Ϣ��Ԥ�ȷ���һ���ڴ�,���������ӱ����η����ڴ�,�Ӷ��ӿ���������ٶ�" << std::endl; return true;
	case CB_MSGMAX: std::cout << "����Ϣ������������Ϣֵ:0x0162,0x0163,0x0165,��Ϣ���岻��,����������Visual Studio 6��Ŀ¼Ҳֻ���䶨��,ȴδ����ʹ�õ���ش���" << std::endl; return true;

	case CBN_ERRSPACE: std::cout << "֪ͨ����(ͨ��WM_COMMAND��֪),��Ͽ��ڴ治��" << std::endl; return true;
	case CBN_KILLFOCUS: std::cout << "֪ͨ����(ͨ��WM_COMMAND��֪),��Ͽ�ʧȥ���뽹��" << std::endl; return true;
	case CBN_SELENDOK: std::cout << "֪ͨ����(ͨ��WM_COMMAND��֪),�û�����Ͽ��ѡ���ǺϷ���" << std::endl; return true;

	case LB_ADDSTRING: std::cout << "���б��������ַ���" << std::endl; return true;
	case LB_INSERTSTRING: std::cout << "���б���в���һ����Ŀ���ݻ��ַ���������LB_ADDSTRING��Ϣ,����Ϣ���ἤ��LBS_SORT��ʽ������Ŀ��������" << std::endl; return true;
	case LB_DELETESTRING: std::cout << "ɾ���б���е�һ���ַ���" << std::endl; return true;
	case LB_SELITEMRANGEEX: std::cout << "�ڶ�ѡ״̬�µ��б����ѡ��һ��������������Ŀ" << std::endl; return true;
	case LB_RESETCONTENT: std::cout << "����б���������б���" << std::endl; return true;
	case LB_SETSEL: std::cout << "�ڶ�ѡ״̬�µ��б����ѡ��һ���ַ���" << std::endl; return true;
	case LB_SETCURSEL: std::cout << "���б����ѡ��һ���ַ���,���������ڵ���Ŀ��������Ұ��,��������ѡ�е��ַ���" << std::endl; return true;
	case LB_GETSEL: std::cout << "����б����һ����Ŀ��ѡ��״̬" << std::endl; return true;
	case LB_GETCURSEL: std::cout << "����б���е�ǰ��ѡ��Ŀ�����������ڵ�ѡ״̬���б����Ч" << std::endl; return true;
	case LB_GETTEXT: std::cout << "���б���л��һ���ַ���" << std::endl; return true;
	case LB_GETTEXTLEN: std::cout << "���һ���б�����ַ����ĳ���" << std::endl; return true;
	case LB_GETCOUNT: std::cout << "����б������Ŀ������" << std::endl; return true;
	case LB_SELECTSTRING: std::cout << "�������ڵ�ѡ���б��,�趨��ָ���ַ�����ƥ����б���Ϊѡ����,������б����ʹѡ����ɼ�" << std::endl; return true;
	case LB_DIR: std::cout << "���б�����г��ļ���" << std::endl; return true;
	case LB_GETTOPINDEX: std::cout << "�����б���е�һ���ɼ��������" << std::endl; return true;
	case LB_FINDSTRING: std::cout << "���б���в���ƥ���ַ���(���Դ�Сд)" << std::endl; return true;
	case LB_GETSELCOUNT: std::cout << "�����ڶ���ѡ���б��,����Ϣ���ڷ���ѡ�������Ŀ" << std::endl; return true;
	case LB_GETSELITEMS: std::cout << "�����ڶ���ѡ���б��,����Ϣ���ڻ��ѡ�������Ŀ��λ��" << std::endl; return true;
	case LB_SETTABSTOPS: std::cout << "�����б��Ĺ��(���뽹��)վ��������˳���" << std::endl; return true;
	case LB_GETHORIZONTALEXTENT: std::cout << "�����б��Ŀɹ����Ŀ��(���ص�)" << std::endl; return true;
	case LB_SETHORIZONTALEXTENT: std::cout << "����Ϣ�����б��Ĺ������" << std::endl; return true;
	case LB_SETCOLUMNWIDTH: std::cout << "Ϊ�б��ָ������" << std::endl; return true;
	case LB_ADDFILE: std::cout << "Ϊ�б�������ļ���" << std::endl; return true;
	case LB_SETTOPINDEX: std::cout << "�����б����ָ�����б�������Ϊ�б��ĵ�һ���ɼ���,�Ὣ�б����������ʵ�λ��" << std::endl; return true;
	case LB_GETITEMRECT: std::cout << "�����б���л���б���Ŀͻ�����RECT" << std::endl; return true;
	case LB_GETITEMDATA: std::cout << "�б����ÿ���б���и�32λ��������,����Ϣ���ڷ���ָ���б���ĸ�������" << std::endl; return true;
	case LB_SETITEMDATA: std::cout << "���ڸ����б����ָ���б����32λ��������" << std::endl; return true;
	case LB_SELITEMRANGE: std::cout << "�����ڶ���ѡ���б��,����ʹָ����Χ�ڵ��б���ѡ�л���ѡ" << std::endl; return true;
	case LB_SETANCHORINDEX: std::cout << "�����б��������������ѡ�еı����ָ������" << std::endl; return true;
	case LB_GETANCHORINDEX: std::cout << "�����б����������ѡ�е��������" << std::endl; return true;
	case LB_SETCARETINDEX: std::cout << "�����б�������ü������뽹�㵽ָ������" << std::endl; return true;
	case LB_GETCARETINDEX: std::cout << "�����б���з��ؾ��о��ν������Ŀ����" << std::endl; return true;
	case LB_SETITEMHEIGHT: std::cout << "�����б����ָ���б�����ʾ�߶�" << std::endl; return true;
	case LB_GETITEMHEIGHT: std::cout << "�����б���з����б����ĳһ��ĸ߶�(���ص�)" << std::endl; return true;
	case LB_FINDSTRINGEXACT: std::cout << "�����б���в����ַ���(���Դ�Сд),��LB_FINDSTRING��ͬ,����Ϣ���������ַ�����ͬ" << std::endl; return true;
	case LB_SETLOCALE: std::cout << "�����б���������б��ǰ������������Դ���,���û�ʹ��LB_ADDSTRING����Ͽ��е��б������Ӽ�¼,��ʹ��LBS_SORT��������������ʱ,����ʹ�ø����Դ���" << std::endl; return true;
	case LB_GETLOCALE: std::cout << "�����б���л�ȡ�б��ǰ������������Դ���,���û�ʹ��LB_ADDSTRING����Ͽ��е��б������Ӽ�¼,��ʹ��LBS_SORT��������������ʱ,����ʹ�ø����Դ���" << std::endl; return true;
	case LB_SETCOUNT: std::cout << "�����б�������ñ�����Ŀ" << std::endl; return true;
	case LB_INITSTORAGE: std::cout << "ֻ������Win95�汾,����Ҫ���б���м����������ʱ,����Ϣ��Ԥ�ȷ���һ���ڴ�,�������Ժ�Ĳ�����һ�δη����ڴ�" << std::endl; return true;
	case LB_ITEMFROMPOINT: std::cout << "�����б���л����ָ�����������Ŀ����" << std::endl; return true;
	case LB_MSGMAX: std::cout << "����Ϣ������������Ϣֵ:0x01B0,0x01B1,0x01B3,,��Ϣ���岻��,����������Visual Studio 6��Ŀ¼Ҳֻ���䶨��,ȴδ����ʹ�õ���ش���" << std::endl; return true;

	case LBN_ERRSPACE: std::cout << "֪ͨ����(ͨ��WM_COMMAND��֪),�б���ڴ治��" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_MOUSEMOVE: std::cout << "�ƶ����" << std::endl; return true;
#endif
	case WM_LBUTTONDOWN: std::cout << "����������" << std::endl; return true;
	case WM_LBUTTONUP: std::cout << "�ͷ�������" << std::endl; return true;
	case WM_LBUTTONDBLCLK: std::cout << "˫��������" << std::endl; return true;
	case WM_RBUTTONDOWN: std::cout << "��������Ҽ�" << std::endl; return true;
	case WM_RBUTTONUP: std::cout << "�ͷ�����Ҽ�" << std::endl; return true;
	case WM_RBUTTONDBLCLK: std::cout << "˫������Ҽ�" << std::endl; return true;
	case WM_MBUTTONDOWN: std::cout << "��������м�" << std::endl; return true;
	case WM_MBUTTONUP: std::cout << "�ͷ�����м�" << std::endl; return true;
	case WM_MBUTTONDBLCLK: std::cout << "˫������м�" << std::endl; return true;
	case WM_MOUSEWHEEL: std::cout << "���������ת��ʱ,���ͱ���Ϣ����ǰӵ�н���Ŀؼ�" << std::endl; return true;
	case WM_MOUSELAST: std::cout << "WM_MBUTTONDBLCLK�ı���,ͨ�������ж������Ϣ�ķ�Χ,��Ӧ�Ļ���WM_MOUSEFIRST,����:if(message > =  WM_MOUSEFIRST)&&(message <= WM_MOUSELAST)" << std::endl; return true;
	case WM_PARENTNOTIFY: std::cout << "��MDI�Ӵ��ڱ�����������,���û�����һ��������������Ӵ�����ʱ,���ͱ���Ϣ���丸��" << std::endl; return true;
	case WM_ENTERMENULOOP: std::cout << "���ͱ���Ϣ֪ͨӦ�ó�����������ѽ���˵�ѭ��ģʽ" << std::endl; return true;
	case WM_EXITMENULOOP: std::cout << "���ͱ���Ϣ֪ͨӦ�ó�������������˳��˵�ѭ��ģʽ" << std::endl; return true;
	case WM_NEXTMENU: std::cout << "��ʹ�����ͷ�������Ҽ�ͷ�����ڲ˵�����ϵͳ�˵�֮���л�ʱ,�ᷢ�ͱ���Ϣ��Ӧ�ó���,��ؽṹ��:MDINEXTMENU" << std::endl; return true;
	case WM_SIZING: std::cout << "���û����ڵ������ڴ�Сʱ,���ͱ���Ϣ������;ͨ������ϢӦ�ó���ɼ��Ӵ��ڴ�С��λ��,Ҳ���޸�����" << std::endl; return true;
	case WM_CAPTURECHANGED: std::cout << "����ʧȥ��������ʱ,���ͱ���Ϣ������" << std::endl; return true;
#ifndef SIMPLEMSG
	case WM_MOVING: std::cout << "���û����ƶ�����ʱ���ͱ���Ϣ,ͨ������ϢӦ�ó����Լ��Ӵ��ڴ�С��λ��,Ҳ���޸�����" << std::endl; return true;
#endif
	case WM_POWERBROADCAST: std::cout << "����Ϣ���͸�Ӧ�ó�����֪ͨ���йص�Դ�����¼�,�����������ʱ�ᷢ�ͱ���Ϣ" << std::endl; return true;
	case WM_DEVICECHANGE: std::cout << "���豸��Ӳ�����øı�ʱ,���ͱ���Ϣ��Ӧ�ó�����豸��������" << std::endl; return true;
	case WM_MDICREATE: std::cout << "���ͱ���Ϣ�����ĵ�Ӧ�ó���Ŀͻ�����������һ��MDI�Ӵ���" << std::endl; return true;
	case WM_MDIDESTROY: std::cout << "���ͱ���Ϣ�����ĵ�Ӧ�ó���Ŀͻ��������ر�һ��MDI�Ӵ���" << std::endl; return true;
	case WM_MDIACTIVATE: std::cout << "���ͱ���Ϣ�����ĵ�Ӧ�ó���Ŀͻ�����֪ͨ�ͻ����ڼ�����һ��MDI�Ӵ���,���ͻ������յ�����Ϣ��,������WM_MDIACTIVE��Ϣ��MDI�Ӵ���(δ����)��������" << std::endl; return true;
	case WM_MDIRESTORE: std::cout << "���ͱ���Ϣ��MDI�ͻ�����,���Ӵ��ڴ������С���ָ���ԭ���Ĵ�С" << std::endl; return true;
	case WM_MDINEXT: std::cout << "���ͱ���Ϣ��MDI�ͻ�����,������һ����ǰһ������" << std::endl; return true;
	case WM_MDIMAXIMIZE: std::cout << "���ͱ���Ϣ��MDI�ͻ����������һ��MDI�Ӵ���" << std::endl; return true;
	case WM_MDITILE: std::cout << "���ͱ���Ϣ��MDI�ͻ�����,��ƽ�̷�ʽ������������MDI�Ӵ���" << std::endl; return true;
	case WM_MDICASCADE: std::cout << "���ͱ���Ϣ��MDI�ͻ�����,�Բ����ʽ������������MDI�Ӵ���" << std::endl; return true;
	case WM_MDIICONARRANGE: std::cout << "���ͱ���Ϣ��MDI�ͻ�������������������С����MDI�Ӵ���" << std::endl; return true;
	case WM_MDIGETACTIVE: std::cout << "���ͱ���Ϣ��MDI�ͻ��������ҵ�������Ӵ��ھ��" << std::endl; return true;
	case WM_MDISETMENU: std::cout << "���ͱ���Ϣ��MDI�ͻ�����,��MDI�˵������Ӵ��ڵĲ˵�" << std::endl; return true;
	case WM_ENTERSIZEMOVE: std::cout << "��ĳ���ڽ����ƶ��������С��ģʽѭ��ʱ,����Ϣ���͵��ô���" << std::endl; return true;
	case WM_EXITSIZEMOVE: std::cout << "ȷ���û��ı䴰�ڴ�С��ı䴰��λ�õ��¼��Ǻ�ʱ��ɵ�" << std::endl; return true;
	case WM_DROPFILES: std::cout << "����Ϸ�ʱ,�����¼�����ʱ���ͱ���Ϣ,����:�ļ��ϷŹ���" << std::endl; return true;
	case WM_MDIREFRESHMENU: std::cout << "���ͱ���Ϣ�����ĵ�Ӧ�ó���Ŀͻ�����,���ݵ�ǰMDI�Ӵ��ڸ���MDI��ܴ��ڵĲ˵�" << std::endl; return true;



	case WM_IME_SETCONTEXT: std::cout << "Ӧ�ó���Ĵ��ڼ���ʱ,ϵͳ����Ӧ�ó�����WM_IME_SETCONTEXT��Ϣ,ע:���뷨���" << std::endl; return true;
	case WM_IME_NOTIFY: std::cout << "��ʹ��WM_IME_NOTIFY��Ϣ��֪ͨ����IME����״̬�ĳ���ı�,ע:���뷨���" << std::endl; return true;
	case WM_IME_CONTROL: std::cout << "��ʹ��WM_IME_CONTROL��Ϣ���ı���ĸ��ϴ��ڵ�λ��,ע:���뷨���" << std::endl; return true;
	case WM_IME_COMPOSITIONFULL: std::cout << "�û��ӿڴ��ڲ������ӱ��봰�ڵĳߴ�ʱ,IME�û��ӿڴ��ڽ�����WM_IME_COMPOSITIONFULL��Ϣ,�ɲ�����,ע:���뷨���" << std::endl; return true;
	case WM_IME_SELECT: std::cout << "ϵͳ����WM_IME_SELECT�Ա�ѡ��һ���µ�IME���뷨,ע:���뷨���" << std::endl; return true;
	case WM_IME_CHAR: std::cout << "�������뷨��������ʱ,�ᷢ��WM_IME_CHAR��Ϣ" << std::endl; return true;
	case WM_IME_REQUEST: std::cout << "Ӧ�ó����������뷨ʱ,�������ͱ���Ϣ" << std::endl; return true;
	case WM_IME_KEYDOWN: std::cout << "�����뷨¼�ִ����а��°���ʱ,�������ͱ���Ϣ" << std::endl; return true;
	case WM_IME_KEYUP: std::cout << "�����뷨¼�ִ������ͷŰ���ʱ,�������ͱ���Ϣ" << std::endl; return true;
	case WM_MOUSEHOVER: std::cout << "����ƹ��ؼ�ʱ,�������ͱ���Ϣ" << std::endl; return true;
	case WM_MOUSELEAVE: std::cout << "����뿪�ؼ�ʱ,�������ͱ���Ϣ" << std::endl; return true;
	case WM_CUT: std::cout << "Ӧ�ó����ͱ���Ϣ��һ���༭�����Ͽ���ɾ����ǰѡ����ı�" << std::endl; return true;
	case WM_COPY: std::cout << "Ӧ�ó����ͱ���Ϣ��һ���༭�����Ͽ�,�Ա���CF_TEXT��ʽ���Ƶ�ǰѡ����ı���������" << std::endl; return true;
	case WM_PASTE: std::cout << "Ӧ�ó����ͱ���Ϣ���༭�����Ͽ�,�Ա�Ӽ������еõ�����" << std::endl; return true;
	case WM_CLEAR: std::cout << "Ӧ�ó����ͱ���Ϣ���༭�����Ͽ�,�������ǰѡ�������" << std::endl; return true;
	case WM_UNDO: std::cout << "Ӧ�ó����ͱ���Ϣ���༭�����Ͽ�,�Գ������һ�β���" << std::endl; return true;
	case WM_RENDERFORMAT: std::cout << "Ӧ�ó�����Ҫϵͳ���а�����ʱ,�������ͱ���Ϣ" << std::endl; return true;
	case WM_RENDERALLFORMATS: std::cout << "Ӧ�ó����˳�ʱ�ڳ����˳�ʱ,ϵͳ�����ǰ�����͸���Ϣ,Ҫ���ṩ���и�ʽ�ļ���������,����������ݶ�ʧ" << std::endl; return true;
	case WM_DESTROYCLIPBOARD: std::cout << "������EmptyClipboard����ʱ,���ͱ���Ϣ���������������" << std::endl; return true;
	case WM_DRAWCLIPBOARD: std::cout << "������������ݱ仯ʱ,���ͱ���Ϣ��������۲������׸�����;�������ü�����۲촰������ʾ�������������" << std::endl; return true;
	case WM_PAINTCLIPBOARD: std::cout << "�����������CF_OWNERDIPLAY��ʽ������,���Ҽ�����۲촰�ڵĿͻ�����Ҫ�ػ�ʱ,�������ͱ���Ϣ" << std::endl; return true;
	case WM_VSCROLLCLIPBOARD: std::cout << "��������鿴���Ĵ�ֱ������������ʱ,�������ͱ���Ϣ" << std::endl; return true;
	case WM_SIZECLIPBOARD: std::cout << "�����������CF_OWNERDIPLAY��ʽ������,���Ҽ�����۲촰�ڵĿͻ�����Ĵ�С�Ѹı�ʱ,����Ϣͨ��������۲촰�ڷ��͸��������������" << std::endl; return true;
	case WM_ASKCBFORMATNAME: std::cout << "ͨ��������۲촰�ڷ��ͱ���Ϣ���������������,������һ��CF_OWNERDISPLAY��ʽ�ļ����������" << std::endl; return true;
	case WM_CHANGECBCHAIN: std::cout << "��һ�����ڴӼ�����۲�������ȥʱ,���ͱ���Ϣ��������۲������׸�����" << std::endl; return true;
	case WM_HSCROLLCLIPBOARD: std::cout << "����Ϣͨ��һ��������۲촰�ڷ��͸��������������,�������ڵ����������CFOWNERDISPALY��ʽ������,�����и��¼��ڼ�����۲촰��ˮƽ��������,������Ӧ����������ͼ�񲢸��¹�������ֵ" << std::endl; return true;
	case WM_QUERYNEWPALETTE: std::cout << "����Ϣ���͸���Ҫ�յ�����Ĵ���,����Ϣ��ʹ�������յ�����ʱͬʱ�л���ʵ���߼���ɫ��" << std::endl; return true;
	case WM_PALETTEISCHANGING: std::cout << "��һ��Ӧ�ó�����Ҫʵ�������߼���ɫ��ʱ,������Ϣ֪ͨ���е�Ӧ�ó���" << std::endl; return true;
	case WM_PALETTECHANGED: std::cout << "����Ϣ��һ��ӵ�н���Ĵ���ʵ�������߼���ɫ���,���ͱ���Ϣ�����ж������ص��Ĵ���,�Դ����ı�ϵͳ��ɫ��" << std::endl; return true;
	case WM_HOTKEY: std::cout << "���û�������RegisterHotKey����ע����ȼ�ʱ,���ͱ���Ϣ" << std::endl; return true;
	case WM_PRINT: std::cout << "���ͱ���Ϣ��һ������������ָ�����豸�������л�������,�����ڴ��ڽ�ͼ,�����ӿؼ���ͼʱ�õ��������ӿؼ��ȴ�ĺڿ�" << std::endl; return true;
	case WM_PRINTCLIENT: std::cout << "�ͱ���Ϣ��һ������������ָ�����豸�������л�����ͻ���(��ͨ������һ����ӡ���豸��������)" << std::endl; return true;
	case WM_HANDHELDFIRST: std::cout << "��Ϣ����δ֪,����������Visual Studio 6��Ŀ¼Ҳֻ���䶨��,ȴδ����ʹ�õ���ش���" << std::endl; return true;
	case WM_HANDHELDLAST: std::cout << "��Ϣ����δ֪,����������Visual Studio 6��Ŀ¼Ҳֻ���䶨��,ȴδ����ʹ�õ���ش���" << std::endl; return true;
	case WM_AFXFIRST: std::cout << "ָ���׸�AFX��Ϣ(MFC)" << std::endl; return true;
	case WM_AFXLAST: std::cout << "ָ��ĩ��afx��Ϣ" << std::endl; return true;
	case WM_PENWINFIRST: std::cout << "ָ���׸�Pen Window��Ϣ,�μ�:PENWIN.H/WINUSER.H" << std::endl; return true;
	case WM_PENWINLAST: std::cout << "ָ��ĩ��Pen Window��Ϣ,�μ�:PENWIN.H/WINUSER.H" << std::endl; return true;

	case WM_APP: std::cout << "���ڰ���Ӧ�ó����Զ���˽����Ϣ,ͨ����ʽΪ:WM_APP + X" << std::endl; return true;
	case WM_USER: std::cout << "���ڰ���Ӧ�ó����Զ���˽����Ϣ,ͨ����ʽΪ:WM_USER + X" << std::endl; return true;

	case TTM_ACTIVATE: std::cout << "��̬ͣ�ú����ù�����ʾ�ؼ�,TTM��ΪToolTip Message" << std::endl; return true;
	case TTM_SETDELAYTIME: std::cout << "ʹ�ñ���Ϣ��ָ��������ʾ�ؼ���ʾ��ʾ�ı�ʱ��ʱ���ӳ�(����ʱ��)(����)" << std::endl; return true;
	case TTM_ADDTOOLA: std::cout << "Ϊĳ����ע����ӹ�����ʾ�ؼ�,��ؽṹ��:TOOLINFO" << std::endl; return true;
	case TTM_ADDTOOLW: std::cout << "Ϊĳ����ע����ӹ�����ʾ�ؼ�,��ؽṹ��:TOOLINFO" << std::endl; return true;
	case TTM_DELTOOLA: std::cout << "Ϊĳ����ȥ��������ʾ�ؼ�,��ؽṹ��:TOOLINFO" << std::endl; return true;
	case TTM_DELTOOLW: std::cout << "Ϊĳ����ȥ��������ʾ�ؼ�,��ؽṹ��:TOOLINFO" << std::endl; return true;
	case TTM_NEWTOOLRECTA: std::cout << "Ϊĳ�������ù�����ʾ�ؼ��ľ��δ�С(rect)" << std::endl; return true;
	case TTM_NEWTOOLRECTW: std::cout << "Ϊĳ�������ù�����ʾ�ؼ��ľ��δ�С(rect)" << std::endl; return true;
	case TTM_RELAYEVENT: std::cout << "���ڰ������Ϣ���ݸ�һ��������ʾ�ؼ�,������д���" << std::endl; return true;
	case TTM_GETTOOLINFOA: std::cout << "�ӹ�����ʾ�ؼ��л�ȡTOOLINFO�ṹ��" << std::endl; return true;
	case TTM_GETTOOLINFOW: std::cout << "�ӹ�����ʾ�ؼ��л�ȡTOOLINFO�ṹ��" << std::endl; return true;
	case TTM_SETTOOLINFOA: std::cout << "�����򹤾���ʾ�ؼ�������TOOLINFO�ṹ��" << std::endl; return true;
	case TTM_SETTOOLINFOW: std::cout << "�����򹤾���ʾ�ؼ�������TOOLINFO�ṹ��" << std::endl; return true;
	case TTM_HITTESTA: std::cout << "��ȡ������ʾ�ؼ��Ƿ��ܵ��������Ϣ,���������������Ƿ��ڹ�����ʾ�ؼ��󶨵ľ����ڵ��" << std::endl; return true;
	case TTM_HITTESTW: std::cout << "��ȡ������ʾ�ؼ��Ƿ��ܵ��������Ϣ,���������������Ƿ��ڹ�����ʾ�ؼ��󶨵ľ����ڵ��" << std::endl; return true;
	case TTM_GETTEXTA: std::cout << "�ӹ�����ʾ�ؼ��л�ȡ�ı�����" << std::endl; return true;
	case TTM_GETTEXTW: std::cout << "�ӹ�����ʾ�ؼ��л�ȡ�ı�����" << std::endl; return true;
	case TTM_UPDATETIPTEXTA: std::cout << "���ڶԹ�����ʾ�ؼ������ı�����" << std::endl; return true;
	case TTM_UPDATETIPTEXTW: std::cout << "���ڶԹ�����ʾ�ؼ������ı�����" << std::endl; return true;
	case TTM_GETTOOLCOUNT: std::cout << "��ȡ��������ʾ�ؼ���ά���Ĺ��ߵ�����" << std::endl; return true;
	case TTM_ENUMTOOLSA: std::cout << "��ȡ��ǰ���ĸ�������ʾ�ؼ�����ʾ�ı�" << std::endl; return true;
	case TTM_ENUMTOOLSW: std::cout << "��ȡ��ǰ���ĸ�������ʾ�ؼ�����ʾ�ı�" << std::endl; return true;
	case TTM_GETCURRENTTOOLA: std::cout << "��ȡ������ʾ�ؼ��е�ǰ���ߵ���Ϣ" << std::endl; return true;
	case TTM_GETCURRENTTOOLW: std::cout << "��ȡ������ʾ�ؼ��е�ǰ���ߵ���Ϣ" << std::endl; return true;
	case TTM_WINDOWFROMPOINT: std::cout << "�������໯һ�����ڹ���,ʹ֮���ù�����ʾ�ؼ�Ϊ����µĴ�����ʾ������ʾ�ı�" << std::endl; return true;
	case TTM_TRACKACTIVATE: std::cout << "��ȡ������ʾ�ؼ��е�ǰ���ߵ���Ϣ" << std::endl; return true;
	case TTM_TRACKPOSITION: std::cout << "���ø��ٹ�����ʾ�ؼ�������" << std::endl; return true;
	case TTM_SETTIPBKCOLOR: std::cout << "���ù�����ʾ�ؼ��Ĵ��ڱ���ɫ" << std::endl; return true;
	case TTM_SETTIPTEXTCOLOR: std::cout << "���ù�����ʾ�ؼ����ı�ǰ��ɫ" << std::endl; return true;
	case TTM_GETDELAYTIME: std::cout << "ʹ�ñ���Ϣ�ɻ��Թ�����ʾ�ؼ���ʾ��ʾ�ı�ʱ��ʱ���ӳ�(����ʱ��)(����)" << std::endl; return true;
	case TTM_GETTIPBKCOLOR: std::cout << "��ȡ������ʾ�ؼ��Ĵ��ڱ���ɫ" << std::endl; return true;
	case TTM_GETTIPTEXTCOLOR: std::cout << "��ȡ������ʾ�ؼ����ı�ǰ��ɫ" << std::endl; return true;
	case TTM_SETMAXTIPWIDTH: std::cout << "���ù�����ʾ�ؼ����ڵ������(���ص�)" << std::endl; return true;
	case TTM_GETMAXTIPWIDTH: std::cout << "��ȡ������ʾ�ؼ����ڵ������(���ص�)" << std::endl; return true;
	case TTM_SETMARGIN: std::cout << "���ù�����ʾ�ؼ����ڵ����ܱ߿�(���ص�)" << std::endl; return true;
	case TTM_GETMARGIN: std::cout << "��ȡ������ʾ�ؼ����ڵ����ܱ߿�(���ص�)" << std::endl; return true;
	case TTM_POP: std::cout << "��һ����ͼ��ȥ������ʾ�Ĺ�����ʾ�ؼ�" << std::endl; return true;
	case TTM_UPDATE: std::cout << "ǿ�ƹ�����ʾ�ؼ��ػ��䴰��" << std::endl; return true;

	case LVM_FIRST: std::cout << "ָ��Listview�ؼ����׸���Ϣ,���������Ϣ��LVM_FIRST + X����ʽ����,����:LVM_GETBKCOLORΪLVM_FIRST + 0" << std::endl; return true;
	case LVM_SETBKCOLOR: std::cout << "����Listview�ؼ��ı���ɫ,��:ListView_SetBkColor" << std::endl; return true;
	case LVM_GETIMAGELIST: std::cout << "��ȡListview�ؼ���ͼƬ�б���,��:ListView_GetImageList" << std::endl; return true;
	case LVM_SETIMAGELIST: std::cout << "����Listview�ؼ���ͼƬ�б�,��:ListView_SetImageList" << std::endl; return true;
	case LVM_GETITEMCOUNT: std::cout << "��ȡListview�ؼ�����Ŀ����,��:ListView_GetItemCount" << std::endl; return true;
	case LVM_GETITEMA: std::cout << "��ȡListview�ؼ�����Ŀ,��:ListView_GetItem" << std::endl; return true;
	case LVM_GETITEMW: std::cout << "��ȡListview�ؼ�����Ŀ,��:ListView_GetItem" << std::endl; return true;
	case LVM_SETITEMA: std::cout << "����Listview�ؼ�����Ŀ,��:ListView_SetItem" << std::endl; return true;
	case LVM_SETITEMW: std::cout << "����Listview�ؼ�����Ŀ,��:ListView_SetItem " << std::endl; return true;
	case LVM_INSERTITEMA: std::cout << "��Listview�ؼ�������Ŀ,��:ListView_InsertItem" << std::endl; return true;
	case LVM_INSERTITEMW: std::cout << "��Listview�ؼ�������Ŀ,��:ListView_InsertItem" << std::endl; return true;
	case LVM_DELETEITEM: std::cout << "����ɾ��Listview�ؼ��е�һ����Ŀ,��:ListView_DeleteItem" << std::endl; return true;
	case LVM_DELETEALLITEMS: std::cout << "���Listview�ؼ�������Ŀ,��:ListView_DeleteAllItems" << std::endl; return true;
	case LVM_GETCALLBACKMASK: std::cout << "��ȡListview�ؼ��Ļص�����,��:ListView_GetCallbackMask" << std::endl; return true;
	case LVM_SETCALLBACKMASK: std::cout << "����Listview�ؼ��Ļص�����,��:ListView_SetCallbackMask" << std::endl; return true;
	case LVM_GETNEXTITEM: std::cout << "��ȡListview�ؼ��е���һ����Ŀ,��:ListView_GetNextItem" << std::endl; return true;
	case LVM_FINDITEMA: std::cout << "��Listview�ؼ���Ѱ����Ŀ,��:ListView_FindItem" << std::endl; return true;
	case LVM_FINDITEMW: std::cout << "��Listview�ؼ���Ѱ����Ŀ,��:ListView_FindItem" << std::endl; return true;
	case LVM_GETITEMRECT: std::cout << "��Listview�ؼ��л�ȡָ����Ŀ�ľ��η�Χ,��:ListView_GetItemRect" << std::endl; return true;
	case LVM_SETITEMPOSITION: std::cout << "����Listview�ؼ�����Ŀλ��,��:ListView_SetItemPosition" << std::endl; return true;
	case LVM_GETITEMPOSITION: std::cout << "��ȡListview�ؼ�����Ŀλ��,��:ListView_GetItemPosition" << std::endl; return true;
	case LVM_GETSTRINGWIDTHA: std::cout << "��Listview�ؼ��л�ȡ��ʾһ���ı������п�,��:ListView_GetStringWidth" << std::endl; return true;
	case LVM_GETSTRINGWIDTHW: std::cout << "��Listview�ؼ��л�ȡ��ʾһ���ı�������,��:ListView_GetStringWidth" << std::endl; return true;
	case LVM_HITTEST: std::cout << "��Listview�ؼ��н��е������,�ж�ĳ�����������Ƿ�����Listview�ؼ�ĳ����Ŀ��,��:ListView_HitTest" << std::endl; return true;
	case LVM_ENSUREVISIBLE: std::cout << "Listview�ؼ���ĳ����Ŀ��֤����ʾ����(����),������һ����Ŀ��ȫ���򲿷��Ƿ����,��:ListView_EnsureVisible" << std::endl; return true;
	case LVM_SCROLL: std::cout << "��Listview�ؼ����ƶ�������,��:ListView_Scroll" << std::endl; return true;
	case LVM_REDRAWITEMS: std::cout << "��Listview�ؼ����ػ���Ŀ,��:ListView_RedrawItems" << std::endl; return true;
	case LVM_ARRANGE: std::cout << "��Listview�ؼ���ͼ����ʾʱ,������Ŀ,��:ListView_Arrange" << std::endl; return true;
	case LVM_EDITLABELA: std::cout << "�༭Listview�ؼ���ָ����Ŀ���ı�(��label��ǩ),��:ListView_EditLabel" << std::endl; return true;
	case LVM_EDITLABELW: std::cout << "�༭Listview�ؼ���ָ����Ŀ���ı�(��label��ǩ),��:ListView_EditLabel" << std::endl; return true;
	case LVM_GETEDITCONTROL: std::cout << "��Listview�ؼ���,���б�ؼ����ڽ��б༭ʱ,���ظñ༭��ľ��,��:ListView_GetEditControl" << std::endl; return true;
	case LVM_GETCOLUMNA: std::cout << "��ȡListview�ؼ���ĳ�е�����,��:ListView_GetColumn" << std::endl; return true;
	case LVM_GETCOLUMNW: std::cout << "��ȡListview�ؼ���ĳ�е�����,��:ListView_GetColumn" << std::endl; return true;
	case LVM_SETCOLUMNA: std::cout << "����Listview�ؼ���ĳ�е�����,��:ListView_InsertColumn" << std::endl; return true;
	case LVM_SETCOLUMNW: std::cout << "����Listview�ؼ���ĳ�е�����,��:ListView_InsertColumn" << std::endl; return true;
	case LVM_INSERTCOLUMNA: std::cout << "��Listview�ؼ�����һ����,��:ListView_InsertColumn" << std::endl; return true;
	case LVM_INSERTCOLUMNW: std::cout << "��Listview�ؼ�����һ����,��:ListView_InsertColumn" << std::endl; return true;
	case LVM_DELETECOLUMN: std::cout << "ɾ��Listview�ؼ��е�һ����,��:ListView_DeleteColumn" << std::endl; return true;
	case LVM_GETCOLUMNWIDTH: std::cout << "��Listview�ؼ��л�ȡָ���еĿ��,��:ListView_GetColumnWidth" << std::endl; return true;
	case LVM_SETCOLUMNWIDTH: std::cout << "��Listview�ؼ�������ָ���еĿ��,��:ListView_SetColumnWidth" << std::endl; return true;
	case LVM_GETHEADER: std::cout << "��Listview�ؼ��л�ȡ��ͷ�ؼ��ľ��,��:ListView_GetHeader" << std::endl; return true;
	case LVM_CREATEDRAGIMAGE: std::cout << "��Listview�ؼ���Ϊָ������Ŀ����һ����ҷͼ���б�,��:ListView_CreateDragImage" << std::endl; return true;
	case LVM_GETVIEWRECT: std::cout << "��ǰ������Ŀ��ռ�õľ��η�Χ,��:ListView_GetViewRect" << std::endl; return true;
	case LVM_GETTEXTCOLOR: std::cout << "��Listview�ؼ��л�ȡ�ı���ɫ,��:ListView_GetTextColor" << std::endl; return true;
	case LVM_SETTEXTCOLOR: std::cout << "��Listview�ؼ��������ı���ɫ,��:ListView_SetTextColor" << std::endl; return true;
	case LVM_GETTEXTBKCOLOR: std::cout << "��Listview�ؼ��л�ȡ�ı�����ɫ,��:ListView_SetTextBkC" << std::endl; return true;
	case LVM_SETTEXTBKCOLOR: std::cout << "��Listview�ؼ��������ı�����ɫ,��:ListView_SetTextBkColor" << std::endl; return true;
	case LVM_GETTOPINDEX: std::cout << "��Listview�ؼ��л�ȡ���ӷ�Χ���׸���Ŀ������,��:ListView_GetTopInd" << std::endl; return true;
	case LVM_GETCOUNTPERPAGE: std::cout << "��Listview�ؼ��л�ȡ��ǰ������Ŀ����,��:ListView_GetCountPerPage" << std::endl; return true;
	case LVM_GETORIGIN: std::cout << "��Listview�ؼ��л�ȡ��ǰ��ԭ��,��:ListView_GetOrigin" << std::endl; return true;
	case LVM_UPDATE: std::cout << "����ˢ��Listview�ؼ�,���ÿؼ�����LVS_AUTOARRANGE���,����������в���,��:ListView_Update" << std::endl; return true;
	case LVM_SETITEMSTATE: std::cout << "��Listview�ؼ���������Ŀ״̬,��:ListView_SetItemState" << std::endl; return true;
	case LVM_GETITEMSTATE: std::cout << "��Listview�ؼ��л�ȡ��Ŀ״̬,��:ListView_GetItemS" << std::endl; return true;
	case LVM_GETITEMTEXTA: std::cout << "��Listview�ؼ��л�ȡ��Ŀ�ı�,��:ListView_GetItemText" << std::endl; return true;
	case LVM_GETITEMTEXTW: std::cout << "��Listview�ؼ��л�ȡ��Ŀ�ı�,��:ListView_GetItemText" << std::endl; return true;
	case LVM_SETITEMTEXTA: std::cout << "��Listview�ؼ���������Ŀ�ı�,��:ListView_SetItemText" << std::endl; return true;
	case LVM_SETITEMTEXTW: std::cout << "��Listview�ؼ���������Ŀ�ı�,��:ListView_SetItemText" << std::endl; return true;
	case LVM_SETITEMCOUNT: std::cout << "��Listview�ؼ�(�����б���ͼ�ؼ�,LVS_OWNERDATA���)�����õ�ǰ�б�����Ŀ������,��:ListView_SetItemCount/ListView_SetItemCountEx" << std::endl; return true;
	case LVM_SORTITEMS: std::cout << "��Listview�ؼ��н�����Ŀ��������,��:ListView_SortItems" << std::endl; return true;
	case LVM_SETITEMPOSITION32: std::cout << "����Listview�ؼ�����Ŀλ��,LVM_SETITEMPOSITION��Ϣ��ͬ����,����Ϣʹ��32λ������,��:ListView_SetItemPosition32" << std::endl; return true;
	case LVM_GETSELECTEDCOUNT: std::cout << "��ȡListview�ؼ��б�ѡ����Ŀ����,��:ListView_GetSelectedCount" << std::endl; return true;
	case LVM_GETITEMSPACING: std::cout << "��ȡListview�ؼ�����Ŀ���,��:ListView_GetItemSpacing" << std::endl; return true;
	case LVM_GETISEARCHSTRINGA: std::cout << "��Listview�ؼ��л�ȡ��������ģʽ���ַ���,��:ListView_GetISearchString" << std::endl; return true;
	case LVM_GETISEARCHSTRINGW: std::cout << "��Listview�ؼ��л�ȡ��������ģʽ���ַ���,��:ListView_GetISearchString" << std::endl; return true;
	case LVM_SETICONSPACING: std::cout << "��Listview�ؼ�������ͼ����,��:ListView_SetIconSpacing" << std::endl; return true;
	case LVM_SETEXTENDEDLISTVIEWSTYLE: std::cout << "����Listview�ؼ�����չ���,��:ListView_SetExtendedListViewStyle/ListView_SetExtendedListViewStyleEx" << std::endl; return true;
	case LVM_GETEXTENDEDLISTVIEWSTYLE: std::cout << "��ȡListview�ؼ�����չ���,��:ListView_GetExtendedListViewStyle" << std::endl; return true;
	case LVM_GETSUBITEMRECT: std::cout << "��ȡListview�ؼ�����Ŀ�ľ��η�Χ,��:ListView_GetSubItemRect" << std::endl; return true;
	case LVM_SUBITEMHITTEST: std::cout << "��Listview�ؼ����е������,��ȷ���ĸ���Ŀ������Ŀ���ڸ�����λ��,��:ListView_SubItemHitTest" << std::endl; return true;
	case LVM_SETCOLUMNORDERARRAY: std::cout << "��Listview�ؼ�����������ʾ������(��������),��:ListView_SetColumnOrderArray" << std::endl; return true;
	case LVM_GETCOLUMNORDERARRAY: std::cout << "��Listview�ؼ��з��ص�ǰ���е����Ҷ��뷽ʽ,��:ListView_GetColumnOrderArray" << std::endl; return true;
	case LVM_SETHOTITEM: std::cout << "��Listview�ؼ��������ȵ���Ŀ(����),��:ListView_SetHotItem" << std::endl; return true;
	case LVM_GETHOTITEM: std::cout << "��Listview�ؼ��л�ȡ�ȵ���Ŀ(����),��:ListView_GetHotItem" << std::endl; return true;
	case LVM_SETHOTCURSOR: std::cout << "��Listview�ؼ��������ȵ���,��:ListView_SetHotCursor" << std::endl; return true;
	case LVM_GETHOTCURSOR: std::cout << "��Listview�ؼ��л�ȡ�ȵ���,��:ListView_GetHotCursor" << std::endl; return true;
	case LVM_APPROXIMATEVIEWRECT: std::cout << "��Listview�ؼ��м�����ʾһ����Ŀ����Ŀ�Ⱥ͸߶�,��:ListView_ApproximateViewRect" << std::endl; return true;
	case LVM_SETWORKAREAS: std::cout << "��Listview�ؼ������ù�����,��:ListView_SetWorkAreas" << std::endl; return true;
	case LVM_GETSELECTIONMARK: std::cout << "��Listview�ؼ��л�ȡ��ǰѡ����Ŀ,��:ListView_GetSelectionMark" << std::endl; return true;
	case LVM_SETSELECTIONMARK: std::cout << "��Listview�ؼ������õ�ǰѡ����Ŀ,��:ListView_SetSelectionMark" << std::endl; return true;
	case LVM_SETBKIMAGEA: std::cout << "��Listview�ؼ����������ñ���ͼƬ,��:ListView_SetBkImage" << std::endl; return true;
	case LVM_GETBKIMAGEA: std::cout << "��ȡListview�ؼ��ı���ͼƬ,��:ListView_GetBkImage" << std::endl; return true;
	case LVM_GETWORKAREAS: std::cout << "��ȡListview�ؼ��Ĺ��������η�Χ,��:ListView_GetWorkAreas" << std::endl; return true;
	case LVM_SETHOVERTIME: std::cout << "��Listview�ؼ������������ѡ��ĳ��ǰͣ���ڸ�����ӳٺ�����,��:ListView_SetHoverTime" << std::endl; return true;
	case LVM_GETHOVERTIME: std::cout << "��ȡListview�ؼ��������ѡ��ĳ��ǰͣ���ڸ�����ӳٺ�����,��:ListView_GetHoverTime" << std::endl; return true;
	case LVM_GETNUMBEROFWORKAREAS: std::cout << "��ȡListview�ؼ��ĵ�ǰ�������е���Ŀ����,��:ListView_GetNumberOfWorkAreas" << std::endl; return true;
	case LVM_SETTOOLTIPS: std::cout << "����Listview�ؼ��Ĺ�����ʾ�ؼ�,��:ListView_SetToolTips" << std::endl; return true;
	case LVM_GETTOOLTIPS: std::cout << "��ȡListview�ؼ��Ĺ�����ʾ�ؼ�,��:ListView_GetToolTips" << std::endl; return true;
	case LVM_SETBKIMAGEW: std::cout << "�Կ��ַ���ʽ����Listview�ؼ��ı���ͼ,��:ListView_SetBkImage" << std::endl; return true;
	case LVM_GETBKIMAGEW: std::cout << "�Կ��ַ���ʽ��ȡListview�ؼ��ı���ͼ,��:ListView_GetBkImage" << std::endl; return true;

	case LVN_ITEMCHANGING: std::cout << "֪ͨListview�ؼ��ĸ���:�и���Ŀ�����ı�" << std::endl; return true;
	case LVN_ITEMCHANGED: std::cout << "֪ͨListview�ؼ��ĸ���:�и���Ŀ�Ѿ����ı�" << std::endl; return true;
	case LVN_INSERTITEM: std::cout << "֪ͨListview�ؼ��ĸ���:�и�����Ŀ��������" << std::endl; return true;
	case LVN_DELETEITEM: std::cout << "֪ͨListview�ؼ��ĸ���:�и���ĿҪ��ɾ��" << std::endl; return true;
	case LVN_DELETEALLITEMS: std::cout << "֪ͨListview�ؼ��ĸ���:������ĿҪ��ɾ��" << std::endl; return true;
	case LVN_BEGINLABELEDITA: std::cout << "֪ͨListview�ؼ��ĸ���:һ����Ŀ���ı���ǩ�༭�����" << std::endl; return true;
	case LVN_BEGINLABELEDITW: std::cout << "֪ͨListview�ؼ��ĸ���:һ����Ŀ���ı���ǩ�༭�����" << std::endl; return true;
	case LVN_ENDLABELEDITA: std::cout << "֪ͨListview�ؼ��ĸ���:һ����Ŀ���ı���ǩ�༭���յ�" << std::endl; return true;
	case LVN_ENDLABELEDITW: std::cout << "֪ͨListview�ؼ��ĸ���:һ����Ŀ���ı���ǩ�༭���յ�" << std::endl; return true;
	case LVN_COLUMNCLICK: std::cout << "֪ͨListview�ؼ��ĸ���:һ���б������" << std::endl; return true;
	case LVN_BEGINDRAG: std::cout << "֪ͨListview�ؼ��ĸ���:���������ϷŲ�����ʼ��" << std::endl; return true;
	case LVN_BEGINRDRAG: std::cout << "֪ͨListview�ؼ��ĸ���:����Ҽ����ϷŲ�����ʼ��" << std::endl; return true;
	case LVN_ODCACHEHINT: std::cout << "֪ͨ��Ϣ,�����б�ؼ����������������ݱ��ı�ʱ����" << std::endl; return true;
	case LVN_ODFINDITEMA: std::cout << "֪ͨ��Ϣ,�����б�ؼ�����Ҫ�������߲����ض��Ļص���Ŀʱ����" << std::endl; return true;
	case LVN_ODFINDITEMW: std::cout << "֪ͨ��Ϣ,�����б�ؼ�����Ҫ�������߲����ض��Ļص���Ŀʱ����" << std::endl; return true;
	case LVN_ITEMACTIVATE: std::cout << "֪ͨ��Ϣ,�����б�ؼ����û�������ĳ����Ŀʱ����" << std::endl; return true;
	case LVN_ODSTATECHANGED: std::cout << "֪ͨ��Ϣ,�����б�ؼ���һ����Ŀ��״̬�������ѱ��ı�ʱ����" << std::endl; return true;
	case LVN_HOTTRACK: std::cout << "֪ͨ��Ϣ,�����б�ؼ����û�������ӹ�ĳ����Ŀʱ����" << std::endl; return true;
	case LVN_GETDISPINFOA: std::cout << "�����б�ؼ�����,���󸸴��ṩ��ʾ��Ŀ���ĳ����Ŀ�����������Ϣ" << std::endl; return true;
	case LVN_GETDISPINFOW: std::cout << "�����б�ؼ�����,���󸸴��ṩ��ʾ��Ŀ���ĳ����Ŀ�����������Ϣ" << std::endl; return true;
	case LVN_SETDISPINFOA: std::cout << "֪ͨListview�ؼ��ĸ���:���ø���ĳ����Ŀ����Ϣ" << std::endl; return true;
	case LVN_SETDISPINFOW: std::cout << "֪ͨListview�ؼ��ĸ���:���ø���ĳ����Ŀ����Ϣ" << std::endl; return true;
	case LVN_MARQUEEBEGIN: std::cout << "֪ͨListview�ؼ��ĸ���:ĳ���߿�ѡ���ѿ�ʼ" << std::endl; return true;
	case LVN_GETINFOTIPA: std::cout << "֪ͨ��Ϣ,�ɴ�ͼ��ľ���LVS_EX_INFOTIP��չ����Listview�ؼ�����Ҫ���󸽼ӵ��ı���Ϣ(��ʾ�ڹ�����ʾ�ؼ���)ʱ����" << std::endl; return true;
	case LVN_GETINFOTIPW: std::cout << "֪ͨ��Ϣ,�ɴ�ͼ��ľ���LVS_EX_INFOTIP��չ����Listview�ؼ�����Ҫ���󸽼ӵ��ı���Ϣ(��ʾ�ڹ�����ʾ�ؼ���)ʱ����" << std::endl; return true;

	case TVM_CREATEDRAGIMAGE: std::cout << "�������ؼ���RAGͼ��,��:TreeView_CreateDragImage" << std::endl; return true;
	case TVM_DELETEITEM: std::cout << "ɾ�����ؼ�����Ŀ,��:TreeView_DeleteAllItems" << std::endl; return true;
	case TVM_EDITLABELA: std::cout << "�༭���ؼ��ı�ǩ,��:TVN_BEGINLABELEDIT" << std::endl; return true;
	case TVM_EDITLABELW: std::cout << "�༭���ؼ��ı�ǩ,��:TVN_BEGINLABELEDIT" << std::endl; return true;
	case TVM_ENDEDITLABELNOW: std::cout << "�����༭���ؼ����±�ǩ,��:TreeView_EndEditLabelNow" << std::endl; return true;
	case TVM_ENSUREVISIBLE: std::cout << "��֤���ؼ���ĳ����Ŀ����,��:TreeView_EnsureVisible" << std::endl; return true;
	case TVM_EXPAND: std::cout << "��չ���������ؼ�ĳ�ڵ��µ������ӽڵ�,��:TreeView_Expand" << std::endl; return true;
	case TVM_GETBKCOLOR: std::cout << "��ȡ���ؼ��ı���ɫ,��:TreeView_GetBkColor" << std::endl; return true;
	case TVM_GETCOUNT: std::cout << "��ȡ���ؼ�����Ŀ����,��:TreeView_GetCount" << std::endl; return true;
	case TVM_GETEDITCONTROL: std::cout << "��ȡ���ؼ��༭��ؼ����(���ڱ༭ĳ���ı�),��:TreeView_GetEditControl" << std::endl; return true;
	case TVM_GETIMAGELIST: std::cout << "��ȡ���ؼ����б�ͼƬ,����ͼ����,��:TreeView_GetImageList" << std::endl; return true;
	case TVM_GETINDENT: std::cout << "��ȡ���ؼ�����������ڸ�����������(����),��:TreeView_GetIndent" << std::endl; return true;
	case TVM_GETINSERTMARKCOLOR: std::cout << "����һ��������ǰ����༭��ɫ��COLORREF,��:TreeView_GetInsertMarkColor" << std::endl; return true;
	case TVM_GETISEARCHSTRING: std::cout << "��ȡ���ؼ��������ı�,��:TreeView_GetISearchString" << std::endl; return true;
	case TVM_GETITEMA: std::cout << "��ȡ���ؼ�ָ���������,��ؽṹ:TVITEM,��:TreeView_GetItem" << std::endl; return true;
	case TVM_GETITEMW: std::cout << "��ȡ���ؼ�ָ���������,��ؽṹ:TVITEM,��:TreeView_GetItem" << std::endl; return true;
	case TVM_GETITEMHEIGHT: std::cout << "ȡ���ؼ���Ŀ���о�,��:TreeView_GetItemHeight" << std::endl; return true;
	case TVM_GETITEMRECT: std::cout << "��ȡ���ؼ���Ŀ�ı߽����,��ȷ�������Ƿ����,��:TreeView_GetItemRect" << std::endl; return true;
	case TVM_GETNEXTITEM: std::cout << "��ȡ���ؼ���һ��Ŀ�ľ��,��:TreeView_GetNextItem/TreeView_GetChild/TreeView_GetDropHilight/TreeView_GetFirstVisible/TreeView_GetLastVisible/TreeView_GetNextSibling/TreeView_GetNextVisible/TreeView_GetParent/TreeView_GetPrevSibling/TreeView_GetPrevVisible/TreeView_GetRoot/TreeView_GetSelection " << std::endl; return true;
	case TVM_GETSCROLLTIME: std::cout << "��ȡ���ؼ���������ʱ��(����),��:TreeView_GetScrollTime" << std::endl; return true;
	case TVM_GETTEXTCOLOR: std::cout << "��ȡ���ؼ����ı���ɫ,��:TreeView_GetTextColor" << std::endl; return true;
	case TVM_GETTOOLTIPS: std::cout << "��ȡ���ؼ���ʹ�õĹ�����ʾ�ؼ��ľ��,��:TreeView_GetToolTips" << std::endl; return true;
	case TVM_GETUNICODEFORMAT: std::cout << "��ȡ���ؼ���UNICODE��ʽ��־,��:TreeView_GetUnicodeFormat" << std::endl; return true;
	case TVM_GETVISIBLECOUNT: std::cout << "��ȡ���ؼ��еĿ�����Ŀ����,��:TreeView_GetVisibleCount" << std::endl; return true;
	case TVM_HITTEST: std::cout << "���ؼ��������,��:TreeView_HitTest" << std::endl; return true;
	case TVM_INSERTITEMA: std::cout << "�����ؼ��в�����Ŀ,��:TreeView_InsertItem" << std::endl; return true;
	case TVM_INSERTITEMW: std::cout << "�����ؼ��в�����Ŀ,��:TreeView_InsertItem" << std::endl; return true;
	case TVM_SELECTITEM: std::cout << "ѡȡ���ؼ�����Ŀ,��:TreeView_Select/TreeView_SelectItem/TreeView_SelectDropTarget" << std::endl; return true;
	case TVM_SETBKCOLOR: std::cout << "�������ؼ��ı���ɫ,��:TreeView_SetBkColor" << std::endl; return true;
	case TVM_SETIMAGELIST: std::cout << "�������ؼ���ͼ���б�,��:TreeView_SetImageList" << std::endl; return true;
	case TVM_SETINDENT: std::cout << "�������ؼ������Ŀ��,���ػ��ؼ��Է�ӳ�µĿ��,��:TreeView_SetIndent" << std::endl; return true;
	case TVM_SETINSERTMARK: std::cout << "�������ؼ��Ĳ����־,��:TreeView_SetInsertMark" << std::endl; return true;
	case TVM_SETINSERTMARKCOLOR: std::cout << "�������ؼ��Ĳ����־ɫ(���ذ�����ǰ�Ĳ�����ɫ��COLORREFֵ),��:TreeView_SetInsertMarkColor" << std::endl; return true;
	case TVM_SETITEMA: std::cout << "�������ؼ�����Ŀ,��ؽṹ:TVITEM,��:TreeView_SetItem" << std::endl; return true;
	case TVM_SETITEMW: std::cout << "�������ؼ�����Ŀ,��ؽṹ:TVITEM,��:TreeView_SetItem" << std::endl; return true;
	case TVM_SETITEMHEIGHT: std::cout << "�������ؼ�����Ŀ�߶�(����)(������Ŀ��ǰ�ĸ߶�),��:TreeView_SetItemHeight" << std::endl; return true;
	case TVM_SETSCROLLTIME: std::cout << "�������ؼ���������ʱ��(����),��:TreeView_SetScrollTime" << std::endl; return true;
	case TVM_SETTEXTCOLOR: std::cout << "�������ؼ����ı���ɫ,��:TreeView_SetTextColor" << std::endl; return true;
	case TVM_SETTOOLTIPS: std::cout << "�������ؼ��Ĺ�����ʾ�ؼ�(���),��:TreeView_SetToolTips" << std::endl; return true;
	case TVM_SETUNICODEFORMAT: std::cout << "�������ؼ���UNICODE��ʽ��־,��:TreeView_SetUnicodeFormat" << std::endl; return true;
	case TVM_SORTCHILDREN: std::cout << "�����ؼ���ָ������������������,��:TreeView_SortChildren" << std::endl; return true;
	case TVM_SORTCHILDRENCB: std::cout << "ͨ��һ�������Ƚϸ���Ļص����������ؼ���������,��:TreeView_SortChildrenCB" << std::endl; return true;

	case TVN_BEGINDRAG: std::cout << "���ؼ����������Ϸſ�ʼ" << std::endl; return true;
	case TVN_BEGINLABELEDIT: std::cout << "��ʼ�༭��Ŀ�ı�ǩ" << std::endl; return true;
	case TVN_BEGINRDRAG: std::cout << "���ؼ�������Ҽ��Ϸſ�ʼ" << std::endl; return true;
	case TVN_DELETEITEM: std::cout << "ɾ�����ؼ��е���Ŀ" << std::endl; return true;
	case TVN_ENDLABELEDIT: std::cout << "�����û��༭��Ŀ�ı�ǩ" << std::endl; return true;
	case TVN_GETDISPINFO: std::cout << "����Ҫ�õ����ؼ���ĳ�����Ϣʱ����(��õ�������ʾ�ַ�)" << std::endl; return true;
	case TVN_GETINFOTIP: std::cout << "��ȡ������ʾ�ؼ���Ϣ" << std::endl; return true;
	case TVN_ITEMEXPANDED: std::cout << "���ؼ�ĳ����ѱ�չ��������,���ýṹ:NMTREEVIEW" << std::endl; return true;
	case TVN_ITEMEXPANDING: std::cout << "���ؼ�ĳ��㽫��չ��������,���ýṹ:NMTREEVIEW" << std::endl; return true;
	case TVN_KEYDOWN: std::cout << "�û������ؼ��а�����ĳ���̰���,�������ؼ���������뽹��" << std::endl; return true;
	case TVN_SELCHANGED: std::cout << "�û��Ѹı����ؼ���Ŀ��ѡ��" << std::endl; return true;
	case TVN_SELCHANGING: std::cout << "�û������ؼ���Ŀ��ѡ�񽫸ı�" << std::endl; return true;
	case TVN_SETDISPINFO: std::cout << "֪ͨ�������ؼ���Ŀ����Ϣ" << std::endl; return true;
	case TVN_SINGLEEXPAND: std::cout << "�û�ʹ����굥���򿪻�ر����ؼ���Ŀʱ���͵�֪ͨ��Ϣ" << std::endl; return true;

	case HDM_DELETEITEM: std::cout << "ɾ����ͷ�ؼ�������Ŀ,��:Header_DeleteItem" << std::endl; return true;
	case HDM_GETIMAGELIST: std::cout << "��ȡ��ͷ�ؼ�����ͼ���б�,��:Header_GetImageList" << std::endl; return true;
	case HDM_GETITEMA: std::cout << "��ȡ��ͷ�ؼ�������Ŀ,��:Header_GetItemRect" << std::endl; return true;
	case HDM_GETITEMW: std::cout << "��ȡ��ͷ�ؼ�������Ŀ,��:Header_GetItemRect" << std::endl; return true;
	case HDM_GETITEMCOUNT: std::cout << "��ȡ��ͷ�ؼ�������Ŀ����,��:Header_GetItemCount " << std::endl; return true;
	case HDM_GETITEMRECT: std::cout << "��ȡ��ͷ�ؼ�����Ŀ���η�Χ,��:Header_GetItemRect" << std::endl; return true;
	case HDM_GETORDERARRAY: std::cout << "��ȡ��ͷ�ؼ��е����Ҷ��뷽ʽ(�ṩ�Ա�ͷ��Ŀ�����֧��),��:Header_GetOrderArray" << std::endl; return true;
	case HDM_HITTEST: std::cout << "��ͷ�ؼ��������,�ҵ������������" << std::endl; return true;
	case HDM_INSERTITEM: std::cout << "Ϊ��ͷ�ؼ���������Ŀ,��:Header_InsertItem" << std::endl; return true;
	case HDM_LAYOUT: std::cout << "��ȡ��һ��ָ�������ڵı�ͷ�ؼ��Ĵ�С��λ��,��:Header_Layout" << std::endl; return true;
	case HDM_ORDERTOINDEX: std::cout << "��ȡ��ͷ�ؼ��е����Ҷ��뷽ʽ(��Ŀ����,���ڱ�ͷ�ؼ�����Ŀ��˳��,������������0��ʼ),��:Header_OrderToIndex" << std::endl; return true;
	case HDM_SETHOTDIVIDER: std::cout << "���ø����ȷָ���,�ṩ�Ա�ͷ��Ŀ���Ϸ�,��:Header_SetHotDivider" << std::endl; return true;
	case HDM_SETIMAGELIST: std::cout << "Ϊ��ͷ�ؼ�����ͼ���б�,��:Header_SetImageList" << std::endl; return true;
	case HDM_SETITEMA: std::cout << "���ñ�ͷ�ؼ�����Ŀ������,��:Header_SetItem" << std::endl; return true;
	case HDM_SETITEMW: std::cout << "���ñ�ͷ�ؼ�����Ŀ������,��:Header_SetItem" << std::endl; return true;
	case HDM_SETORDERARRAY: std::cout << "���ñ�ͷ�ؼ��е����Ҷ��뷽ʽ,��:Header_SetOrderArray" << std::endl; return true;

	case STM_GETICON: std::cout << "��ȡ��ʾ�ھ�̬�ؼ��е�ͼ��ľ��,���SS_ICON���,��:Static_GetIcon" << std::endl; return true;
	case STM_GETIMAGE: std::cout << "��ȡ��ʾ�ھ�̬�ؼ��е�ͼ��ľ��,֧��ͼ��(SS_ICON)�����(SS_ICON)��λͼ(SS_BITMAP)��ͼԪ(SS_ENHMETAFILE)" << std::endl; return true;
	case STM_SETICON: std::cout << "���ý�Ҫ�ھ�̬�ؼ�����ʾ��ͼ��(���),���SS_ICON���,��:Static_SetIcon" << std::endl; return true;
	case STM_SETIMAGE: std::cout << "���ý�Ҫ�ھ�̬�ؼ�����ʾ��ͼ��(���),֧��ͼ��(SS_ICON)�����(SS_ICON)��λͼ(SS_BITMAP)��ͼԪ(SS_ENHMETAFILE),�Զ����(λͼ):#define Static_SetBitmap(hwndCtl, hBitmap)  ((HBITMAP)(UINT_PTR)SendMessage((hwndCtl), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(HBITMAP)hBitmap))" << std::endl; return true;
	case TB_ADDBUTTONS: std::cout << "��һ�������������һ��������ť,��ؽṹ:TBBUTTON" << std::endl; return true;
	case TB_ADDSTRING: std::cout << "���һ�����ַ��������������ڲ��ַ����б���" << std::endl; return true;
	case TB_AUTOSIZE: std::cout << "�����������ĳߴ硣�����ð�ť��λͼ�ߴ硢����ַ���ʱ,���ͱ���Ϣ" << std::endl; return true;
	case TB_BUTTONSTRUCTSIZE: std::cout << "ָ��������TBBUTTON�ṹ�Ĵ�С" << std::endl; return true;
	case TB_CHANGEBITMAP: std::cout << "�ı乤�����İ�ťͼ��(ͼ���б��е�����)" << std::endl; return true;
	case TB_CHECKBUTTON: std::cout << "��ѡ�������������ָ����ť����һ����ť����ѡʱ,�����������񱻰���һ��" << std::endl; return true;
	case TB_GETANCHORHIGHLIGHT: std::cout << "��ȡ�������Ĺ̶��ļ�������" << std::endl; return true;
	case TB_GETBITMAP: std::cout << "��ȡ�빤������ť�������λͼ������" << std::endl; return true;
	case TB_GETBITMAPFLAGS: std::cout << "�ӹ�������ȡλͼ��־(DWORDֵ)" << std::endl; return true;
	case TB_GETBUTTONINFO: std::cout << "��ȡ�������а�ť����չ��Ϣ,��ؽṹ:TBBUTTONINFO" << std::endl; return true;
	case TB_GETBUTTONTEXT: std::cout << "��ȡ��������ť���ı�" << std::endl; return true;
	case TB_GETCOLORSCHEME: std::cout << "��ȡ��������ɫ�ʷ�����Ϣ" << std::endl; return true;
	case TB_GETEXTENDEDSTYLE: std::cout << "��ȡ����������չ���" << std::endl; return true;
	case TB_GETHOTITEM: std::cout << "��ȡ���������ȵ��������(���㿪ʼ)" << std::endl; return true;
	case TB_GETIMAGELIST: std::cout << "��ȡ��������������ʾ��ť����״̬ͼ����б�" << std::endl; return true;
	case TB_GETINSERTMARK: std::cout << "��ȡ�������Ĳ�����,��ؽṹ:TBINSERTMARK" << std::endl; return true;
	case TB_GETINSERTMARKCOLOR: std::cout << "��ȡ�������а�����ǰ�����ǵ���ɫ(COLORREFֵ)" << std::endl; return true;
	case TB_GETMAXSIZE: std::cout << "��ȡ�����������пɼ���ť��ָ������ܳߴ�" << std::endl; return true;
	case TB_GETOBJECT: std::cout << "��ȡ��������IDropTarget�ӿ�,IDropTarget����ʵ�����Ϸŷ�ʽ��ȡ��Դ��" << std::endl; return true;
	case TB_GETPADDING: std::cout << "��ȡ������������(����)(padding:���ס�����)" << std::endl; return true;
	case TB_GETROWS: std::cout << "��ȡ�������е�ǰ��ʾ�İ�ť����,ע:���ǹ���������TBSTYLE_WRAPABLE��񴴽���,����������Ϊ1" << std::endl; return true;
	case TB_GETTEXTROWS: std::cout << "��ȡ������ʾ�ڹ�������ť�ϵ��ı��е��������" << std::endl; return true;
	case TB_GETTOOLTIPS: std::cout << "��ȡ�빤����������Ĺ�����ʾ�ؼ�(����)�ľ��,ͨ�����������Լ�������ά���乤����ʾ�ؼ�,һ�㲻��ʹ�ñ���Ϣ" << std::endl; return true;
	case TB_HITTEST: std::cout << "�������,��ȡ������ָ�����λ�õ�����ֵ,����ֵΪ�����ֵ,���ʾ�õ����ڰ�ť������(�Ƿָ���),Ϊ����õ㲻��ĳ��ť��" << std::endl; return true;
	case TB_INSERTBUTTON: std::cout << "�ڹ������в���һ����ť" << std::endl; return true;
	case TB_INSERTMARKHITTEST: std::cout << "Ϊ�������ϵ�ĳ��(�õ�����������ڹ������ͻ�����)��ȡ�����ǵ���Ϣ,��ؽṹ:TBINSERTMARK" << std::endl; return true;

	case TB_MAPACCELERATOR: std::cout << "Ϊһ����������ťӳ���ݼ��ַ�,���ַ��밴ť�ı��м��»��ߵ��ַ���һ����" << std::endl; return true;
	case TB_MOVEBUTTON: std::cout << "��һ����������ť��һ�������ƶ�����һ������" << std::endl; return true;
	case TB_REPLACEBITMAP: std::cout << "����λͼ�滻�����������е�λͼ,��ؽṹ:TBREPLACEBITMAP" << std::endl; return true;
	case TB_SAVERESTORE: std::cout << "�����ָ���������״̬,��ؽṹ:TBSAVEPARAMS" << std::endl; return true;
	case TB_SETANCHORHIGHLIGHT: std::cout << "�Թ��������м�������" << std::endl; return true;
	case TB_SETBITMAPSIZE: std::cout << "������ӵ���������λͼ�ߴ�" << std::endl; return true;
	case TB_SETBUTTONINFO: std::cout << "���ù�����ָ����ť����Ϣ" << std::endl; return true;
	case TB_SETBUTTONSIZE: std::cout << "���ù������İ�ť�ߴ�(�ߴ�������������λͼ�ߴ�һ��)" << std::endl; return true;
	case TB_SETCMDID: std::cout << "���ù�����ָ����ť�������ʶ��(��ť������ʱ,��ʶ���������͸���������)" << std::endl; return true;
	case TB_SETCOLORSCHEME: std::cout << "���ù�������ɫ�ʷ�����Ϣ" << std::endl; return true;
	case TB_SETDRAWTEXTFLAGS: std::cout << "���ù��������ı����Ʊ�־" << std::endl; return true;
	case TB_SETEXTENDEDSTYLE: std::cout << "���ù���������չ���" << std::endl; return true;
	case TB_SETHOTITEM: std::cout << "���ù������ȵ�������(����TBSTYLE_FLAT���,����Ϣ���ᱻ����)" << std::endl; return true;
	case TB_SETIMAGELIST: std::cout << "���ù�������Ĭ��ͼ���б�" << std::endl; return true;
	case TB_SETINDENT: std::cout << "���ù��������׸���ť������" << std::endl; return true;
	case TB_SETINSERTMARK: std::cout << "���ù������ĵ�ǰ������" << std::endl; return true;
	case TB_SETINSERTMARKCOLOR: std::cout << "���ù����������ǵ���ɫ" << std::endl; return true;
	case TB_SETMAXTEXTROWS: std::cout << "������ʾ�ڹ�������ť�е��ı����������" << std::endl; return true;
	case TB_SETPADDING: std::cout << "���ù�����������(����)(padding:���ס�����)" << std::endl; return true;
	case TB_SETPARENT: std::cout << "Ϊ������ָ��һ������(���ڷ���֪ͨ��Ϣ)" << std::endl; return true;
	case TB_SETROWS: std::cout << "���ù�������ť������" << std::endl; return true;
	case TB_SETTOOLTIPS: std::cout << "��һ��������ʾ�ؼ��빤�������й���" << std::endl; return true;

	case TBN_BEGINADJUST: std::cout << "֪ͨ�������ĸ���:�û��ѿ�ʼ�Զ���һ��������,��ؽṹ:NMHDR" << std::endl; return true;
	case TBN_BEGINDRAG: std::cout << "֪ͨ�������ĸ���:�û��ѿ�ʼ�϶���������ĳ����ť,��ؽṹ:NMTOOLBAR" << std::endl; return true;
	case TBN_CUSTHELP: std::cout << "֪ͨ�������ĸ���:�û��Ѿ�ѡ�����Զ��幤�����Ի����ϵİ�����ť,��ؽṹ:NMHDR" << std::endl; return true;
	case TBN_ENDADJUST: std::cout << "֪ͨ�������ĸ���:�û���ֹͣ�Զ���һ��������,��ؽṹ:NMHDR" << std::endl; return true;
	case TBN_ENDDRAG: std::cout << "֪ͨ�������ĸ���:�û���ֹͣ�϶���������ĳ����ť,��ؽṹ:NMTOOLBAR" << std::endl; return true;
	case TBN_GETBUTTONINFO: std::cout << "��ȡ���������Զ�����Ϣ(֪ͨ��Ϣ),��ؽṹ:NMTOOLBAR" << std::endl; return true;
	case TBN_QUERYDELETE: std::cout << "֪ͨ�������ĸ���:�û��Զ��幤����ʱһ����ť�Ƿ�ɱ�ɾ��" << std::endl; return true;
	case TBN_QUERYINSERT: std::cout << "֪ͨ�������ĸ���:�û��Զ��幤����ʱһ����ť�Ƿ�ɱ����뵽ָ����ť�����" << std::endl; return true;
	case TBN_RESET: std::cout << "֪ͨ�������ĸ���:�û����Զ��幤�����Ի������Ѿ��Թ���������������" << std::endl; return true;
	case TBN_TOOLBARCHANGE: std::cout << "֪ͨ�������ĸ���:�û��Ѿ�������һ��������" << std::endl; return true;

	case IPM_CLEARADDRESS: std::cout << "���IP��ַ�ؼ��е�����" << std::endl; return true;
	case IPM_GETADDRESS: std::cout << "��IP��ַ�ؼ���4�����л�ȡ��ֵַ,��غ�:FIRST_IPADDRESS/SECOND_IPADDRESS/THIRD_IPADDRESS/FOURTH_IPADDRESS" << std::endl; return true;
	case IPM_ISBLANK: std::cout << "ȷ��IP��ַ�ؼ���ȫ�����Ƿ�Ϊ��" << std::endl; return true;
	case IPM_SETADDRESS: std::cout << "ΪIP��ַ�ؼ���4�������õ�ַ,��:MAKEIPADDRESS" << std::endl; return true;
	case IPM_SETFOCUS: std::cout << "ΪIP��ַ�ؼ�ָ�������ü��̽���,ͬʱ�����е��ı��ᱻѡ��" << std::endl; return true;
	case IPM_SETRANGE: std::cout << "ΪIP��ַ�ؼ�ָ����������Ч��Χ,��:MAKEIPRANGE" << std::endl; return true;

	case IPN_FIELDCHANGED: std::cout << "�û��ı���IP��ַ�ؼ�ĳ����һ�����ƶ�������ʱ���ͱ�֪ͨ��Ϣ,��ؽṹNMIPADDRESS" << std::endl; return true;

	case PBM_SETBKCOLOR: std::cout << "���ý������ؼ��ı���ɫ" << std::endl; return true;


	case TCM_ADJUSTRECT: std::cout << "���ݸ����Ĵ��ھ��μ����ǩ�ؼ�����ʾ����,�����һ����������ʾ���������Ӧ�Ĵ��ھ���" << std::endl; return true;
	case TCM_DELETEALLITEMS: std::cout << "ɾ����ǩ�ؼ�������ѡ�" << std::endl; return true;
	case TCM_DELETEITEM: std::cout << "�ӱ�ǩ�ؼ���ɾ��ָ��ѡ�" << std::endl; return true;
	case TCM_DESELECTALL: std::cout << "�������ñ�ǩ�ؼ��е���,����κα����µ�ѡ�" << std::endl; return true;
	case TCM_GETCURFOCUS: std::cout << "��ȡ��ǩ�ؼ���ӵ�е�ǰ�����ѡ�����" << std::endl; return true;
	case TCM_GETCURSEL: std::cout << "��ȡ��ǩ�ؼ��е�ǰѡ���ѡ�" << std::endl; return true;
	case TCM_GETEXTENDEDSTYLE: std::cout << "��ȡ��ǩ�ؼ�����չ���" << std::endl; return true;
	case TCM_GETIMAGELIST: std::cout << "��ȡ���ǩ�ؼ��������ͼ���б�" << std::endl; return true;
	case TCM_GETITEM: std::cout << "��ȡ��ǩ�ؼ���ĳѡ�����Ϣ" << std::endl; return true;
	case TCM_GETITEMCOUNT: std::cout << "��ȡ��ǩ�ؼ��е�����" << std::endl; return true;
	case TCM_GETITEMRECT: std::cout << "��ȡ��ǩ�ؼ���ָ��ѡ��ı߽����" << std::endl; return true;
	case TCM_GETROWCOUNT: std::cout << "��ȡ��ǩ�ؼ��е�ǰ����,ע:TCS_MULTILINT���ı�ǩ�ؼ����ж���ѡ�" << std::endl; return true;
	case TCM_GETTOOLTIPS: std::cout << "��ȡ���ǩ�ؼ�������Ĺ�����ʾ�ؼ��ľ��" << std::endl; return true;
	case TCM_HIGHLIGHTITEM: std::cout << "ʹ��ǩ�ؼ���ĳѡ����ڸ���״̬" << std::endl; return true;
	case TCM_HITTEST: std::cout << "ȷ�����ĸ�ѡ�(����)λ��ָ������Ļλ��" << std::endl; return true;
	case TCM_INSERTITEM: std::cout << "����һ����ѡ�����ǩ�ؼ�" << std::endl; return true;
	case TCM_REMOVEIMAGE: std::cout << "�ӱ�ǩ�ؼ���ͼ���б���ɾ��ָ��ͼ��" << std::endl; return true;
	case TCM_SETCURFOCUS: std::cout << "���ý��㵽��ǩ�ؼ���ָ����ѡ�" << std::endl; return true;
	case TCM_SETCURSEL: std::cout << "���ñ�ǩ�ؼ��е�ǰѡ���ѡ�" << std::endl; return true;
	case TCM_SETEXTENDEDSTYLE: std::cout << "���ñ�ǩ�ؼ�����չ���" << std::endl; return true;
	case TCM_SETIMAGELIST: std::cout << "Ϊ��ǩ�ؼ�ָ��һ���Ѵ�����ͼ���б�" << std::endl; return true;
	case TCM_SETITEM: std::cout << "���ñ�ǩ�ؼ���ѡ���ĳЩ����������" << std::endl; return true;
	case TCM_SETITEMEXTRA: std::cout << "��ǩ�ؼ�Ϊ��ʱ,�ı��ǩ�ؼ��и�ѡ��Ķ����ֽ���(Ĭ��4�ֽ�),ע:�����ֽڿɰѳ����������ѡ���������" << std::endl; return true;
	case TCM_SETITEMSIZE: std::cout << "���ñ�ǩ�ؼ���ѡ��Ŀ�Ⱥ͸߶�" << std::endl; return true;
	case TCM_SETMINTABWIDTH: std::cout << "���ñ�ǩ�ؼ���ѡ�����С���" << std::endl; return true;
	case TCM_SETPADDING: std::cout << "���ñ�ǩ�ؼ��е�ÿ��ѡ���ͼ��ͱ�ǩ��Χ�Ŀռ��С(����)" << std::endl; return true;
	case TCM_SETTOOLTIPS: std::cout << "�������ǩ�ؼ�������Ĺ�����ʾ�ؼ�(�þ����������)" << std::endl; return true;

	case TCN_GETOBJECT: std::cout << "�������϶�����ǩ�ؼ���ѡ���ʱ,��ǩ�ؼ�����TCN_GETOBJECT֪ͨ��Ϣ���������Ŀ�����,ע:�����AfxOleInit��ʼ��OLE��" << std::endl; return true;
	case TCN_KEYDOWN: std::cout << "֪ͨ��ǩ�ؼ��ĸ���:�û��Ѱ���ĳ��" << std::endl; return true;
	case TCN_SELCHANGE: std::cout << "֪ͨ��ǩ�ؼ��ĸ���:ѡ��ĵ�ǰѡ���ѱ��ı�" << std::endl; return true;
	case TCN_SELCHANGING: std::cout << "֪ͨ��ǩ�ؼ��ĸ���:ѡ��ĵ�ǰѡ��Ҫ�ı�" << std::endl; return true;

	case EM_GETSEL: std::cout << "��ȡ�༭�ؼ���ǰ��ѡ�в���(����)�Ŀ�ʼ�ͽ���λ��;����˫��,��λ��Ϊ��ʼλ��,��λ��Ϊ�׸�δ��ѡ�е��ַ�λ��" << std::endl; return true;
	case EM_SETSEL: std::cout << "�ڱ༭�ؼ���ѡ���ı�" << std::endl; return true;
	case EM_GETRECT: std::cout << "��ȡ�༭�ؼ��Ĵ���ʽ���ı��߽����(���䴰�ڴ�С�޹�)" << std::endl; return true;
	case EM_SETRECT: std::cout << "���ñ༭�ؼ��Ĵ���ʽ���ı��߽����(���䴰�ڴ�С�޹�)" << std::endl; return true;
	case EM_SETRECTNP: std::cout << "���ö��б༭�ؼ��Ĵ���ʽ���ı��߽����,�������ػ�" << std::endl; return true;
	case EM_SCROLL: std::cout << "ʹ���б༭�ؼ�����һ��(SB_LINEDOWN/SB_LINEUP)��һҳ(SB_PAGEDOWN/SB_PAGEUP)" << std::endl; return true;
	case EM_LINESCROLL: std::cout << "����Ϊ��λʹ���б༭�ؼ����һ����¹���" << std::endl; return true;
	case EM_SCROLLCARET: std::cout << "�����༭�ؼ��е�caret������,ʹ֮����" << std::endl; return true;
	case EM_GETMODIFY: std::cout << "���Ա༭�ؼ��������Ƿ񱻸ı�(���и��ڲ�����������������Ƿ񱻸ı�)" << std::endl; return true;
	case EM_SETMODIFY: std::cout << "���û�����༭�ؼ��ĸı��־;�ı��Ǳ����ı��Ƿ񱻸ı�(�û��ı��ı�ʱ,���Զ����ô˱�־)" << std::endl; return true;
	case EM_GETLINECOUNT: std::cout << "��ȡ���б༭�ؼ��е�������" << std::endl; return true;
	case EM_LINEINDEX: std::cout << "��ö��б༭�ؼ���ĳ�е��ַ�����" << std::endl; return true;
	case EM_SETHANDLE: std::cout << "���ÿɱ����б༭�ؼ�ʹ�õľֲ��ڴ�ľ��,�༭�ؼ����ô˻������洢��ǰ��ʾ���ı�,�������Լ�����" << std::endl; return true;
	case EM_GETHANDLE: std::cout << "��ȡ���б༭�ؼ��е�ǰ������ڴ���,�˾���Ǹ��ֲ��ڴ���" << std::endl; return true;
	case EM_GETTHUMB: std::cout << "ȡ�ö����ı��༭�ؼ��Ĺ�����ĵ�ǰλ��(����)" << std::endl; return true;
	case EM_LINELENGTH: std::cout << "��ñ༭�ؼ��е��еĳ���" << std::endl; return true;
	case EM_REPLACESEL: std::cout << "��ָ���ı����Ǳ༭�ؼ��е�ǰ��ѡ�е��ı�" << std::endl; return true;
	case EM_GETLINE: std::cout << "�ڱ༭�ؼ��л��һ���ı�" << std::endl; return true;
	case EM_LIMITTEXT: std::cout << "�û��ڱ༭�ؼ��������ı�ʱ���ı���������" << std::endl; return true;
	case EM_CANUNDO: std::cout << "ȷ���Ա༭�ؼ��Ĳ����ܷ���" << std::endl; return true;
	case EM_UNDO: std::cout << "������һ�ζԱ༭�ؼ��Ĳ���" << std::endl; return true;
	case EM_FMTLINES: std::cout << "���ö��б༭�ؼ����Ƿ������س���" << std::endl; return true;
	case EM_LINEFROMCHAR: std::cout << "��ȡ����ָ���ַ��������е��к�(�ַ�����ָ�ӿ�ʼ��ָ���ַ����ַ���)" << std::endl; return true;
	case EM_SETTABSTOPS: std::cout << "�ڶ��б༭�ؼ������������������(�ؼ����ı����κ��Ʊ��������һ�οհ�)" << std::endl; return true;
	case EM_SETPASSWORDCHAR: std::cout << "���û�����༭�ؼ���������滻��ʾ�ַ�" << std::endl; return true;
	case EM_EMPTYUNDOBUFFER: std::cout << "����ؼ��ĳ���������,ʹ�䲻�ܳ�����һ�α༭����" << std::endl; return true;
	case EM_GETFIRSTVISIBLELINE: std::cout << "ȷ���༭�ؼ��п��ӵ�����е��к�" << std::endl; return true;
	case EM_SETREADONLY: std::cout << "���ñ༭�ؼ���ֻ��״̬" << std::endl; return true;
	case EM_SETWORDBREAKPROC: std::cout << "���ñ༭�ؼ����µĶ��ִ���ص�����" << std::endl; return true;
	case EM_GETWORDBREAKPROC: std::cout << "��ȡ�༭�ؼ����µĶ��ִ���ص�����" << std::endl; return true;
	case EM_GETPASSWORDCHAR: std::cout << "��ȡ�༭�ؼ���������滻��ʾ�ַ�" << std::endl; return true;
	case EM_SETMARGINS: std::cout << "���ñ༭�ؼ������ұ߿�" << std::endl; return true;
	case EM_GETMARGINS: std::cout << "��ȡ�༭�ؼ������ұ߿�" << std::endl; return true;
	case EM_GETLIMITTEXT: std::cout << "��ȡ�༭�ؼ����ı������볤������" << std::endl; return true;
	case EM_POSFROMCHAR: std::cout << "���ָ���ַ����������Ͻǵ�����" << std::endl; return true;
	case EM_CHARFROMPOS: std::cout << "��ñ༭�ؼ������ָ��λ�õ��ַ����к��ַ�����" << std::endl; return true;
	case EM_SETIMESTATUS: std::cout << "�޸ı༭�ؼ���IME���뷨������" << std::endl; return true;
	case EM_GETIMESTATUS: std::cout << "��ȡ�༭�ؼ���IME���뷨������" << std::endl; return true;

	case EN_ERRSPACE: std::cout << "�༭�ؼ�����Ϊ�ض���������㹻�Ŀռ��֪ͨ��Ϣ" << std::endl; return true;
	case EN_MAXTEXT: std::cout << "֪ͨ����,�༭�ؼ���ǰ�����ı��ѳ���ָ���ַ���(������β����)" << std::endl; return true;
	case EN_HSCROLL: std::cout << "�û������˱༭�ؼ���ˮƽ������,��������Ļ����֮ǰ��֪ͨ" << std::endl; return true;
	case EN_VSCROLL: std::cout << "�û������˱༭�ؼ��Ĵ�ֱ������,��������Ļ����֮ǰ��֪ͨ" << std::endl; return true;


	case MCN_GETDAYSTATE: std::cout << "��ȡ�����ؼ���������ʾ���(����/Ȧ����),Ҫ��MCS_DAYSTATE���,��ؽṹ:NMDAYSTATE" << std::endl; return true;
	case MCN_SELCHANGE: std::cout << "��ǰѡ�����ѱ��ı�,��ؽṹ:NMSELCHANGE" << std::endl; return true;
	case MCN_SELECT: std::cout << "�������ؼ���ѡ��һ������,��ؽṹ:NMSELCHANGE" << std::endl; return true;
	case NM_RELEASEDCAPTURE: std::cout << "�ͷ������ؼ������Ĳ�����Ϣ,��ؽṹ:NMHDR" << std::endl; return true;


	case DTN_CLOSEUP: std::cout << "DTP�ؼ��е������ؼ���Ҫ�ر�,��ؽṹ:NMHDR" << std::endl; return true;
	case DTN_DATETIMECHANGE: std::cout << "DTP�ؼ��е������ѷ����ı�,��ؽṹ:NMDATETIMECHANGE" << std::endl; return true;
	case DTN_DROPDOWN: std::cout << "DTP�ؼ��е������ؼ���Ҫ��ʾ,��ؽṹ:NMHDR" << std::endl; return true;
	case DTN_FORMAT: std::cout << "DTP�ؼ��е��ı���Ҫ��ʾ�ڻص�����,��ؽṹ:NMDATETIMEFORMAT" << std::endl; return true;
	case DTN_FORMATQUERY: std::cout << "DTP�ؼ��е��ı���ʾ�������ߴ�,��ؽṹ:NMDATETIMEFORMATQUERY" << std::endl; return true;
	case DTN_USERSTRING: std::cout << "�û������DTP�ؼ��ı༭,��ؽṹ:NMDATETIMESTRING" << std::endl; return true;
	case DTN_WMKEYDOWN: std::cout << "�û�������DTP�ؼ��Ļص���,��ؽṹ:NMDATETIMEWMKEYDOWN" << std::endl; return true;


	case SBN_SIMPLEMODECHANGE: std::cout << "����SB_SIMPLE��Ϣ����״̬����ģʽ�����ı�ʱ���͵�֪ͨ��Ϣ" << std::endl; return true;


	case RBN_AUTOSIZE: std::cout << "��Rebar�ؼ��Լ��Զ�������Сʱ��(��RBS_AUTOSIZE��񴴽���)Rebar�ؼ�����" << std::endl; return true;
	case RBN_BEGINDRAG: std::cout << "���û���ʼ�϶�����ʱ��Rebar�ؼ�����" << std::endl; return true;
	case RBN_CHILDSIZE: std::cout << "�������������Ӵ��ڴ�Сʱ��Rebar�ؼ�����" << std::endl; return true;
	case RBN_DELETEDBAND: std::cout << "�ڴ����ѱ�ɾ������Rebar�ؼ�����" << std::endl; return true;
	case RBN_DELETINGBAND: std::cout << "������������ɾ��ʱ��Rebar�ؼ�����" << std::endl; return true;
	case RBN_ENDDRAG: std::cout << "���û�ֹͣ�϶�����ʱ��Rebar�ؼ�����" << std::endl; return true;
	case RBN_GETOBJECT: std::cout << "�������϶����˿ؼ��еĴ�������(��RBS_REGISTERDROP��񴴽���)Rebar�ؼ�����" << std::endl; return true;
	case RBN_HEIGHTCHANGE: std::cout << "����߶��ѱ�����ʱ��Rebar�ؼ�����" << std::endl; return true;
	case RBN_LAYOUTCHANGED: std::cout << "���û����Ĵ˿ؼ��Ĵ�������ʱ��Rebar�ؼ�����" << std::endl; return true;

	case PSM_APPLY: std::cout << "Ӧ�����Ա��������,��:PropSheet_Apply" << std::endl; return true;
	case PSM_CANCELTOCLOSE: std::cout << "ʹ���Ա�ı�ȷ����ť�ϵ��ı�Ϊ�ر�(��־��Ӧ�õĸı䲻�ɱ�ȡ��),��:PropSheet_CancelToClose" << std::endl; return true;
	case PSM_GETCURRENTPAGEHWND: std::cout << "��ȡ���Ա�ǰҳ�Ĵ��ھ��,��:PropSheet_GetCurrentPageHwnd" << std::endl; return true;
	case PSM_GETTABCONTROL: std::cout << "��ȡ���Ա��б�ǩ�ؼ��ľ��,��:PropSheet_GetTabControl" << std::endl; return true;
	case PSM_ISDIALOGMESSAGE: std::cout << "����һ����Ϣ�����Ա�Ի���,��ָ���öԻ����Ƿ��Ѵ����˸���Ϣ,��:PropSheet_IsDialogMessage" << std::endl; return true;
	case PSM_PRESSBUTTON: std::cout << "�����Ա���ģ��ѡ��ĳָ����ť,��:PropSheet_PressButton" << std::endl; return true;
	case PSM_QUERYSIBLINGS: std::cout << "��ѯ���Ա���ֵ�ҳ,�໥���ݲ���,��:PropSheet_QuerySiblings" << std::endl; return true;
	case PSM_REBOOTSYSTEM: std::cout << "ָ��������ϵͳ��ʹ�ı���Ч(��ĳҳ�ı���ϵͳ����),��:PropSheet_RebootSystem" << std::endl; return true;
	case PSM_SETCURSELID: std::cout << "����ҳIDѡ��(����)���Ա���ĳҳ,��:PropSheet_SetCurSelByID" << std::endl; return true;
	case PSM_SETFINISHTEXT: std::cout << "�������Ա���Finish��ť���ı�,��:PropSheet_SetFinishText" << std::endl; return true;
	case PSM_SETTITLE: std::cout << "ָ�����Ա�ı���,��:PropSheet_SetTitle" << std::endl; return true;
	case PSM_SETWIZBUTTONS: std::cout << "ʹ�����Ա��е�Back,Next��Finish��ť��Ч����Ч,��:PropSheet_SetWizButtons" << std::endl; return true;
	case PSM_UNCHANGED: std::cout << "��ֹӦ�ð�ť,���³�ʼ�����Ա�,��:PropSheet_UnChanged" << std::endl; return true;

	case PSN_APPLY: std::cout << "����ҳ��Ӧ�ð�ť������,��ؽṹ:PSHNOTIFY" << std::endl; return true;
	case PSN_GETOBJECT: std::cout << "֪ͨ��������ҳ����OLE�ϷŶ���,��ؽṹ:NMOBJECTNOTIFY" << std::endl; return true;
	case PSN_HELP: std::cout << "�û����������ҳ�İ�����Ť,��ؽṹ:PSHNOTIFY" << std::endl; return true;
	case PSN_KILLACTIVE: std::cout << "����ҳʧȥ����(��������ҳ����˽�����û������ȷ����ť),��ؽṹ:PSHNOTIFY" << std::endl; return true;
	case PSN_QUERYCANCEL: std::cout << "�û����������ҳ��ȡ����Ť,��ؽṹ:PSHNOTIFY" << std::endl; return true;
	case PSN_RESET: std::cout << "�û������ȡ����Ť,����ҳ��������,��ؽṹ:PSHNOTIFY" << std::endl; return true;
	case PSN_SETACTIVE: std::cout << "ĳ����ҳ�������ý���,��ؽṹ:PSHNOTIFY" << std::endl; return true;
	case PSN_WIZBACK: std::cout << "�û�����������Ա��ǰһҳ��ť,��ؽṹ:PSHNOTIFY" << std::endl; return true;
	case PSN_WIZFINISH: std::cout << "�û�����������Ա�Ľ�����ť,��ؽṹ:PSHNOTIFY" << std::endl; return true;
	case PSN_WIZNEXT: std::cout << "�û�����������Ա����һҳ��ť,��ؽṹ:PSHNOTIFY" << std::endl; return true;


	case CBEN_BEGINEDIT: std::cout << "�û����������б����CComboBoxEx�ؼ��ı༭��,��ؽṹ:NMHDR" << std::endl; return true;
	case CBEN_DELETEITEM: std::cout << "CComboBoxEx�ؼ���ĳ��Ŀ�ѱ�ɾ��,��ؽṹ:NMCOMBOBOXEX" << std::endl; return true;
	case CBEN_DRAGBEGIN: std::cout << "�û���ʼ�϶�CComboBoxEx�ؼ��ı༭���е���Ŀͼ��,��ؽṹ:NMCBEDRAGBEGIN" << std::endl; return true;
	case CBEN_ENDEDIT: std::cout << "�û��ѽ�����CComboBoxEx�ؼ��ı༭��Ĳ�����ѡ���������б�,��ؽṹ:NMCBEENDEDIT" << std::endl; return true;
	case CBEN_GETDISPINFO: std::cout << "��ȡCComboBoxEx�ؼ��Ļص���Ŀ����ʾ��Ϣ,��ؽṹ:NMCOMBOBOXEX" << std::endl; return true;
	case CBEN_INSERTITEM: std::cout << "һ������Ŀ�Ѳ��뵽CComboBoxEx�ؼ�,��ؽṹ:NMCOMBOBOXEX" << std::endl; return true;

	case NM_CHAR: std::cout << "�ַ�����������ɿؼ�����,��ؽṹ:NMCHAR" << std::endl; return true;
	case NM_DBLCLK: std::cout << "֪ͨ����:�û��ڿؼ���˫����������,��ؽṹ:NMHDR" << std::endl; return true;
	case NM_HOVER: std::cout << "�û�������ӹ��ؼ�,��ؽṹ:NMHDR" << std::endl; return true;
	case NM_KEYDOWN: std::cout << "�ؼ���ü��̽��㲢����ĳ��,��ؽṹ:NMKEY" << std::endl; return true;
	case NM_KILLFOCUS: std::cout << "֪ͨ����:�û�ʧȥ���뽹��,��ؽṹ:NMHDR" << std::endl; return true;
	case NM_NCHITTEST: std::cout << "�ؼ��յ�WM_NCHITTEST��Ϣ,��ؽṹ:NMMOUSE" << std::endl; return true;
	case NM_RCLICK: std::cout << "֪ͨ����:�û��ڿؼ��ϵ��������Ҽ�,��ؽṹ:NMHDR" << std::endl; return true;
	case NM_RDBLCLK: std::cout << "֪ͨ����:�û��ڿؼ���˫��������Ҽ�,��ؽṹ:NMHDR" << std::endl; return true;
	case NM_RETURN: std::cout << "֪ͨ����:�ؼ���ü��̽��㲢���»س���,��ؽṹ:NMHDR" << std::endl; return true;
	case NM_SETCURSOR: std::cout << "֪ͨ����:�ؼ���ӦWM_SETCURSORҪ���ù��,��ؽṹ:NMMOUSE" << std::endl; return true;
	case NM_SETFOCUS: std::cout << "֪ͨ����:�ؼ��յ����뽹��,��ؽṹ:NMHDR" << std::endl; return true;
	case NM_TOOLTIPSCREATED: std::cout << "֪ͨ����:�ؼ��Ѵ���һ��������ʾ�ؼ�,��ؽṹ:NMTOOLTIPSCREATED" << std::endl; return true;

	case UDM_SETRANGE32: std::cout << "������ת��ť�������޷�Χ(32λDWORDֵ)" << std::endl; return true;

	case UDN_DELTAPOS: std::cout << "΢����ť�ؼ���λ�ý�Ҫ�ı�(���丸���ڷ���),��ؽṹ:NMUPDOWN" << std::endl; return true;

	case PGM_FORWARDMOUSE: std::cout << "���û����Pager��ҳ�ؼ��������Ϣת��,��������Pager�ؼ�ת��WM_MOUSEMOVE��Ϣ�������Ĵ���,��:Pager_ForwardMouse" << std::endl; return true;
	case PGM_GETBKCOLOR: std::cout << "��ȡPager��ҳ�ؼ��ĵ�ǰ����ɫ,��:Pager_GetBkColor" << std::endl; return true;
	case PGM_GETBORDER: std::cout << "��ȡPager��ҳ�ؼ��ĵ�ǰ�߽�ߴ�,��:Pager_GetBorder" << std::endl; return true;
	case PGM_GETBUTTONSIZE: std::cout << "��ȡPager��ҳ�ؼ��ĵ�ǰ��ť��С,��:Pager_GetButtonSize" << std::endl; return true;
	case PGM_GETBUTTONSTATE: std::cout << "��ȡPager��ҳ�ؼ���ָ����ť��״̬,��:Pager_GetButtonState" << std::endl; return true;
	case PGM_GETDROPTARGET: std::cout << "��ȡPager��ҳ�ؼ���IDropTarget�ӿ�ָ��,��:Pager_GetDropTarget" << std::endl; return true;
	case PGM_GETPOS: std::cout << "��ȡPager��ҳ�ؼ��Ĺ������ĵ�ǰλ��,��:Pager_GetPos" << std::endl; return true;
	case PGM_RECALCSIZE: std::cout << "ǿ�����¼���Pager��ҳ�ؼ������Ĵ��ڵĴ�С,��:Pager_RecalcSize" << std::endl; return true;
	case PGM_SETBKCOLOR: std::cout << "����Pager��ҳ�ؼ��ĵ�ǰ����ɫ,��:Pager_SetBkColor" << std::endl; return true;
	case PGM_SETBORDER: std::cout << "����Pager��ҳ�ؼ��ĵ�ǰ�߽�ߴ�,��:Pager_SetBorder" << std::endl; return true;
	case PGM_SETBUTTONSIZE: std::cout << "����Pager��ҳ�ؼ��ĵ�ǰ��ť��С,��:Pager_SetButtonSize" << std::endl; return true;
	case PGM_SETCHILD: std::cout << "����Pager��ҳ�ؼ��а����Ĵ���,��:Pager_SetChild" << std::endl; return true;
	case PGM_SETPOS: std::cout << "��ȡPager��ҳ�ؼ��Ĺ������ĵ�ǰλ��,��:Pager_SetPos" << std::endl; return true;

	case PGN_CALCSIZE: std::cout << "Pager�ؼ����͵�֪ͨ��Ϣ,���Ի�����������ڵĹ����ߴ�,��ؽṹ:NMPGCALCSIZE" << std::endl; return true;
	case PGN_SCROLL: std::cout << "�����������ڱ�����֮ǰ,Pager�ؼ����͵�֪ͨ��Ϣ,��ؽṹ:NMPGSCROLL" << std::endl; return true;
	default: return false;
	}
}


/* ͼ���㷨���� */
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

/* Բ��ͳ��ͼ�㷨�� */
CircularChart::CircularChart() {
	SetAlgorithmName(L"CircularChart");
}

CircularChart::~CircularChart() {
}

WCHAR* CircularChart::GetGAName(){
	return GetAlgorithmName();
}
