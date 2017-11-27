/* FileName:MyCodes.h */

#ifndef MY_CODES_H_
#define MY_CODES_H_

#include <Windows.h>
#include <iostream>
#include <fstream>

/* �ַ������ط���ת��ϣֵ""_hash */
constexpr uint64_t prime = 0x100000001B3ull;
constexpr uint64_t basis = 0xCBF29CE484222325ull;
// �ַ������ط���תhashֵ֧�ֺ���
uint64_t hash_(char const* str);
constexpr uint64_t hash_compile_time(char const* str, uint64_t last_value = basis) {
	return *str ? hash_compile_time(str + 1, (*str ^ last_value) * prime) : last_value;
}
constexpr unsigned long long operator""_hash(char const * p, size_t) {
	return hash_compile_time(p);
}
/***************************************************************************************/
/* ָ���ļ�·����ʵ�ֿ���̨��·���ļ�ͬʱ����� */
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
/* ���� */
// չʾ��һ����������������ʾ��Ϣ
extern void ShowLastError();
// �����ù���ԱȨ��
extern void GetToken();
// ��������ģ����
extern HMODULE GetSelfModuleHandle();
// string �� wstring ��ת
std::string wsTos(const std::wstring& ws);
std::wstring sTows(const std::string& ws);

#endif
