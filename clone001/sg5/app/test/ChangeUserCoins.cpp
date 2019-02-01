#include "LogicInc.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
	if (argc != 4) {
		cout << "usage: " << argv[0] << " coins type file" << endl;
		return 1;
	}

	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "init kernel fail" << endl;
		return 1;
	}
	Config::SetDomain(0);
	int ret = 0;
	int value = CTrans::STOI(argv[1]);
	string type(argv[2]);

	ifstream fin(argv[3]);
	if (!fin.good()) {
		cout << "open file fail" << endl;
		return 1;
	}
	CLogicPay logicPay;
	unsigned uid = 0;
	DataPay pay;
	CLogicUpdates logicUpdates;
	while (!fin.eof() && fin.good()) {
		uid = 0;
		fin >> uid;
		if (!IsValidUid(uid))
			continue;
		ret = logicPay.ChangePay(uid, 0, value, pay, type, 1);
		if (0 == ret) {
			Json::Value updatesData = Json::Value(Json::arrayValue);
			updatesData.resize(1);
			updatesData[0u]["ts"] = Time::GetGlobalTime();
			updatesData[0u]["s"] = "paybackmessage";
			updatesData[0u]["coins"] = value;
			ret = logicUpdates.AddUpdates(uid, updatesData);
			if (0 != ret) {
				cout << "AddUpdates fail,ret=" << ret << ",uid=" << uid << endl;
			}
		} else {
			cout << "fail,uid=" << uid << endl;
		}
	}

	cout << "ok" << endl;
	return 0;
}
