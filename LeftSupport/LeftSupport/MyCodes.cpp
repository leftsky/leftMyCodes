/* FileName:MyCodes.cpp */
#include "MyCodes.h"

#include <iostream>
#include <ShlObj.h>
#include <shellapi.h>


void ShowLastError() {
	HLOCAL lpMsgBuf;
	setlocale(LC_ALL, "");
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(),
		MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::wcout << (WCHAR*)lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}

void GetToken() {
	if (IsUserAnAdmin())
		return;
	else {
		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
		sei.lpVerb = TEXT("runas");
		WCHAR path[512] = { 0 };
		GetModuleFileName(NULL, path, sizeof(path));
		sei.lpFile = path;
		sei.nShow = SW_SHOWNORMAL;
		ShellExecuteEx(&sei);
		exit(0);
	}
}

HMODULE GetSelfModuleHandle() {
	MEMORY_BASIC_INFORMATION mbi;
	return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0)
		? (HMODULE)mbi.AllocationBase : NULL);
}

std::string wsTos(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}
std::wstring sTows(const std::string& s)
{
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}
