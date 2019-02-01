#include "LogicInc.h"

int main(int argc, char *argv[])
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}
	Config::SetDomain(0);
	int ret = 0;
	string aid = "4011000423";

	string url = "action=viewbasematch";
	url.append("&aid=").append(aid);
	string uid = "11000006";
	//String::Format(uid,"%u",m_uid);
	url.append("&uid=");
	url += uid;

	Json::Value Result;
	CLogicBaseMatch logicBaseMatch;
	ret = logicBaseMatch.RequestBaseMatch(url, Result);
	if (0 != ret)
	{
		cout << ret << endl;
		return ret;
	}

	Json::FastWriter writer;
	 cout << writer.write(Result) << endl;
	return 0;
}
