
#include "easylogging/easylogging++.h"
#include "LeftMyCodes/MyCodes.h"
#include "LeftSerialPort.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv) {

	using namespace leftSilPt;
	LeftSPOpen(1);
	LeftSPIoctl(1, ChoseBaud(115200), P_NONE, BIT_8, STOP_1);
	LeftSPClose(1);
	system("pause");

	return 0;
}
