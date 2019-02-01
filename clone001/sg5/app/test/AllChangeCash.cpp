/*
 * AllChangeCash.cpp
 *
 *  Created on: 2013-4-1
 *      Author: Administrator
 */

/*
 * AllChangeCash.cpp
 *
 *  Created on: 2013-2-27
 *      Author: colin
 */
#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[]) {
	if (!Kernel::Init(APP_CONFIG_PATH)) {
		cout << "Init kernel fail!" << endl;
		return -1;
	}
	Config::SetDomain(0);
	if (argc != 5) {
		cout << "usage: " << argv[0] << " [cash] [coin] [type] [serverid]" << endl;
		return 1;
	}
	int ret;
	string strserver(argv[4]);
	vector<string> rlt;
	String::Split(strserver, '-', rlt);
	if (rlt.size() >= 2) {
		unsigned begin = Convert::StringToUInt(rlt[0]);
		unsigned end = Convert::StringToUInt(rlt[1]);
		if (end < begin || end <= 0 || begin <= 0) {
			cout << "usage: " << argv[0] << " [cash] [coin] [type] [serverid]"
					<< endl;
			return 1;
		}
		for (unsigned i = begin; i <= end; ++i) {
			uint64_t uidEnd;
			int cash = CTrans::STOI(argv[1]);
			int coin = CTrans::STOI(argv[2]);
			unsigned userid = UID_MIN + 500000 * (i - 1);
			string type(argv[3]);
			CLogicPay logicPay;
			CLogicIdCtrl logicIdCtrl;
			ret = logicIdCtrl.GetCurrentId(KEY_UID_CTRL, uidEnd, i);
			if (ret != 0) {
				error_log(
						"get current user id fail! from AllChangeCash ---start!");
				return ret;
			}
			int i = time(NULL);
			cout << "i=" << i << endl;
			DataPay pay;
			cout << "uid_min=" << userid << "uid_max=" << uidEnd << endl;
			CLogicUpdates logicUpdates;
			for (; userid <= (unsigned) uidEnd; ++userid) {
				ret = logicPay.ChangePay(userid, cash, coin, pay, type, 1);
				if (ret != 0) {
					cout << "change failed.ret:" << ret << ",uid:" << userid<<endl;
				} else {
					Json::Value updatesData = Json::Value(Json::arrayValue);
					updatesData.resize(1);
					updatesData[0u]["ts"] = Time::GetGlobalTime();
					updatesData[0u]["s"] = "paybackmessage";
					updatesData[0u]["coins"] = cash + coin;
					ret = logicUpdates.AddUpdates(userid, updatesData);
					if (0 != ret) {
						cout << "AddUpdates fail,ret=" << ret << ",uid="
								<< userid << endl;
					}
				}
			}
			int j = time(NULL);
			cout << "j=" << j << endl;
			cout << "take out:" << j - i << "s" << endl;
		}
	} else {
		int serverid = CTrans::STOI(argv[4]);
		if (serverid <= 0) {
			cout << "usage: " << argv[0] << " [cash] [coin] [type] [serverid]"
					<< endl;
			return 1;
		}
		uint64_t uidEnd;
		int cash = CTrans::STOI(argv[1]);
		int coin = CTrans::STOI(argv[2]);
		unsigned userid = UID_MIN + 500000 * (serverid - 1);
		string type(argv[3]);
		CLogicPay logicPay;
		CLogicIdCtrl logicIdCtrl;
		ret = logicIdCtrl.GetCurrentId(KEY_UID_CTRL, uidEnd, serverid);
		if (ret != 0) {
			error_log("get current user id fail! from AllChangeCash ---start!");
			return ret;
		}
		int i = time(NULL);
		cout << "i=" << i << endl;
		DataPay pay;
		cout << "uid_min=" << userid << "uid_max=" << uidEnd << endl;
		CLogicUpdates logicUpdates;
		for (; userid <= (unsigned) uidEnd; ++userid) {
			ret = logicPay.ChangePay(userid,  cash, coin, pay, type, 1);
			if (ret != 0) {
				cout << "change failed.ret:" << ret << ",uid:" << userid<<endl;
			} else {
				Json::Value updatesData = Json::Value(Json::arrayValue);
				updatesData.resize(1);
				updatesData[0u]["ts"] = Time::GetGlobalTime();
				updatesData[0u]["s"] = "paybackmessage";
				updatesData[0u]["coins"] = cash + coin;
				ret = logicUpdates.AddUpdates(userid, updatesData);
				if (0 != ret) {
					cout << "AddUpdates fail,ret=" << ret << ",uid=" << userid
							<< endl;
				}
			}
		}
		int j = time(NULL);
		cout << "j=" << j << endl;
		cout << "take out:" << j - i << "s" << endl;
	}
	return 0;
}

