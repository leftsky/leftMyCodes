/* FileName:MyCodes.h */

#ifndef MY_CODES_H_
#define MY_CODES_H_

#include <Windows.h>
#include <iostream>
#include <fstream>

/* 字符串重载符号转哈希值""_hash */
constexpr uint64_t prime = 0x100000001B3ull;
constexpr uint64_t basis = 0xCBF29CE484222325ull;
// 字符串重载符号转hash值支持函数
uint64_t hash_(char const* str);
constexpr uint64_t hash_compile_time(char const* str, uint64_t last_value = basis) {
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}
constexpr unsigned long long operator""_hash(char const * p, size_t) {
	return hash_compile_time(p);
}
/***************************************************************************************/
/* 指定文件路径后，实现控制台和路径文件同时输出流 */
class CoutExA {
public:
	CoutExA(char const *filename) :m_fout(filename, std::ios::out | std::ios::app){}
	template <class T>
	CoutExA& operator<<(const T &text) {
		m_fout << text; std::cout << text; return *this; }
	CoutExA& operator<<(std::basic_ostream<char>& (*fp)(std::basic_ostream<char>&)) {
		m_fout << fp; std::cout << fp; return *this; }
private:
	std::ofstream m_fout;
};
class CoutExW {
public:
	CoutExW(WCHAR const *filename) :m_wfout(filename, std::wios::out | std::wios::app) {
		m_wfout.imbue(std::locale(std::locale("chs"), "", LC_CTYPE)); }
	template <class T>
	CoutExW& operator<<(const T &text) {
		m_wfout << text; std::wcout << text; return *this; }
	CoutExW& operator<<(std::basic_ostream<wchar_t>& (*fp)(std::basic_ostream<wchar_t>&)) {
		m_wfout << fp; std::wcout << fp; return *this; }
private:
	std::wofstream m_wfout;
};

/***************************************************************************************/
/* 杂项 */
// 展示上一个错误代码的中文提示信息
extern void ShowLastError();
// 请求获得管理员权限
extern void GetToken();
// 获得自身的模块句柄
extern HMODULE GetSelfModuleHandle();
// string 和 wstring 互转
std::string wsTos(const std::wstring& ws);
std::wstring sTows(const std::string& ws);

#endif
