#include "LogicInc.h"

int main(int argc, char *argv[]) {
	if (argc != 4) {
		cout << "usage: AddCashUpdate uid itemid count" << endl;
		return 1;
	}

	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "Init kernel fail!" << endl;
		return -1;
	}
	Config::SetDomain(0);

	int uid = atoi(argv[1]);
	int itemid = atoi(argv[2]);
	int count = atoi(argv[3]);

	Json::Value updates;
	updates.resize(1);
	updates[(unsigned) 0]["s"] = "QQTOPUP";
	updates[(unsigned) 0]["itemid"] = itemid;
	updates[(unsigned) 0]["count"] = count;
	updates[(unsigned) 0]["ts"] = (unsigned) time(0);
	CLogicUpdates logicUpdates;
	int ret = logicUpdates.AddUpdates(uid, updates);
	cout<<"ret="<<ret<<endl;

	return ret;
}

