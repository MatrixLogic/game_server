#include "LogicInc.h"
#include <fstream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	if (argc != 4)
	{
		cout << "usage: " << argv[0] << " [openid] [instanceid] [dir]" << endl;
		return 1;
	}

	unsigned uid = 0;
	CLogicUserBasic logicUserBasic;
	int ret = logicUserBasic.GetUid(uid, PT_TEST, argv[1]);
	if (ret != 0)
	{
		cout << GetErrorMessage() << endl;
		return ret;
	}
	unsigned instanceId = CTrans::STOI(argv[2]);
	CLogicInstance logicInstance;
	ret = logicInstance.ExportInstanceData(uid, instanceId, argv[3]);
	if (ret != 0)
	{
		cout << GetErrorMessage() << endl;
		return ret;
	}

	cout << "ok" << endl;

	return 0;
}
