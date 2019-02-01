#include "LogicInc.h"

int main(int argc, char *argv[]) {
	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "Init kernel fail!" << endl;
		return -1;
	}
	Config::SetDomain(0);

	if (argc != 3) {
		cout << "usage: " << argv[0] << " uid datafile" << endl;
		return 1;
	}
	int uid = atoi(argv[1]);
	if (!uid) {
		cout << "uid wrong!" << endl;
		return 1;
	}

	char buf[1024 * 1024];
	FILE *fp = fopen(argv[2], "r");
	if (!fp) {
		cout << "open datafile fail" << endl;
		return 1;
	}
	int len = fread(buf, 1, 1024 * 1024, fp);
	buf[len] = 0;
	fclose(fp);

	string sdata(buf);
	Json::Reader reader;
	Json::Value data;
	if (!reader.parse(sdata, data)) {
		cout << "parse fail!" << endl;
		return 1;
	}
	unsigned userid = 0;
	if (!Json::GetUInt(data, "userid", userid)) {
		cout << "uid fail!" << endl;
		return 1;
	}
	if (userid != uid)
		data["userid"] = uid;

	CLogicAdmin logicAdmin;
	int ret = logicAdmin.ImportArchive(uid, data);
	if (ret != 0) {
		cout << "import fail!" << endl;
		return ret;
	}

	cout << "import ok!" << endl;
	return 0;
}
