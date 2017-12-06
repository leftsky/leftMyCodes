
#include <iostream>
#include <string.h>
#include "MyCodes.h"


leftName::ThrSfeList tsl;

LeftThrReturn Thr1(LeftThrArgv nothing) {
	for (;;) {
		LeftSleep(10);
		tsl.Add("Hello world!", strlen("Hello world!") + 1);
	}
	return 0;
}

LeftThrReturn Thr2(LeftThrArgv nothing) {
	for (;;) {
		LeftSleep(1);
		char buf[1024];
		if (tsl.GetLast(buf, 1024))
			std::cout << buf << std::endl;
	}
	return 0;
}


int main(int argc, char **argv) {
	using namespace std;
	using namespace leftName;

#if 0				// Analysis ini file test
	pIniInfo info;
	char path[256];
	char value[256];
	GetSelfModulePath(path, 256);
	snprintf(path + strlen(path), 256, "/../../config.ini");
	LEFT_ERROR rt = leftName::AnalysisIniFile(path, &info);
	if (rt != LEFT_SUCCESS)
		cout << "AnalysisIniFile failed:: " << rt << endl;
	if (leftName::GetIniInfo("hi", value, 256, info) == LEFT_SUCCESS)
		cout << value << ":::" << strlen(value) << endl;
	DelIniInfoList(info);
#endif
#if 1

	LeftThrNo no = 0;
	CURRENCY_StartThread(Thr1, NULL, no);
	CURRENCY_StartThread(Thr2, NULL, no);

#endif
	for (;;) { LeftSleep(500); }

	return 0;
}
