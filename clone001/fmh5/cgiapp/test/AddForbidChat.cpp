#include "LogicInc.h"

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		cout << "usage: AddForbidChat uid ts server" << endl;
		return 1;
	}
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init fail" << endl;
		return 1;
	}
	Config::SetDomain(0);

	unsigned uid = atoi(argv[1]);
	unsigned ts = atoi(argv[2]);
	string strserver = argv[3];
	vector<string> rlt;
	unsigned server;
	String::Split(strserver, ':', rlt);
	if(rlt.size() == 2)
	{
		server = atoi(rlt[1].c_str());
		server -= 10000;
		strserver = CTrans::UTOS(server);
	}

	CLogicAdmin logicAdmin;
	return logicAdmin.AddForbidUser(uid, ts, strserver);
}
