
#include <iostream>
#include "MyCodes.h"


int main(int argc, char **argv) {
	using namespace std;
	using namespace leftName;

	pIniInfo info;
	char path[256];
	char value[256];
	GetSelfModulePath(path, 256);
	snprintf(path + strlen(path), 256, "/../../config.ini");
	LEFT_ERROR rt = leftName::AnalysisIniFile(path, &info);
	if (rt != LEFT_SUCCESS)
		cout << "AnalysisIniFile failed:: " << rt << endl;
	if(leftName::GetIniInfo("hi", value, 256, info) == LEFT_SUCCESS)
		cout << value << ":::" << strlen(value) << endl;
	DelIniInfoList(info);

	for (;;) { LeftSleep(500); }

	return 0;
}
