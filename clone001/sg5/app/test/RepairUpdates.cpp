#include "LogicInc.h"

int main(int argc, char *argv[]) {
	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "init kernel fail" << endl;
		return -1;
	}
	Config::SetDomain(0);

	if (argc != 2) {
		cout << "usage: " << argv[0] << " uid" << endl;
		return -1;
	}

	int uid = CTrans::STOI(argv[1]);
	CDataUpdates db;
	int ret = db.RemoveUpdatesBeforeTs(uid, time(NULL));

	cout << "ret = " << ret << endl;

	return 0;
}
