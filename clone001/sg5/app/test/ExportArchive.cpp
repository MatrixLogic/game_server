#include "LogicInc.h"

int main(int argc, char *argv[]) {
	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "Init kernel fail!" << endl;
		return -1;
	}
	Config::SetDomain(0);

	if (argc != 2) {
		cout << "usage: " << argv[0] << " uid " << endl;
		return 1;
	}
	int uid = atoi(argv[1]);
	if (!uid) {
		cout << "uid wrong!" << endl;
		return 1;
	}

	CLogicAdmin logicAdmin;
	Json::Value data;
	int ret = logicAdmin.ExportArchive(uid, data);
	if (ret != 0)
	{
		cout<<"export fail, ret="<<ret<<endl;
		return ret;
	}

	Json::FastWriter writer;
	cout<<writer.write(data);

	return 0;
}
