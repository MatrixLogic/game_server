#include "LogicInc.h"

int main(int argc, char *argv[]) {
	if (argc != 4) {
		cout << "usage: AddCashUpdate uid1 uid2 res" << endl;
		return 1;
	}

	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "Init kernel fail!" << endl;
		return -1;
	}
	Config::SetDomain(0);

	int uid1 = atoi(argv[1]);
	int uid2 = atoi(argv[2]);
	int res = atoi(argv[3]);

	CLogicAdmin logicAdmin;
	int ret = logicAdmin.AddAttack(uid1, uid2, res);
	cout<<"ret="<<ret<<endl;

	return ret;
}

