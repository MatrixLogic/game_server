#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Kernel::Init(APP_CONFIG_PATH);
	Config::SetDomain(0);

	CDataBroadcast* pBroadcast = new CDataBroadcast();
	if (pBroadcast->Init(Config::GetValue(CONFIG_BROADCAST_DATA)) != 0)
	{
		cout << "init fail" << endl;
		return 1;
	}

	BroadcastDATA data;
	int ret = pBroadcast->GetBroadcastData(data);
	if (ret != 0)
	{
		cout << "get fail" << endl;
		return 1;
	}
	cout << "cur_id:" << data.cur_id << endl;
	for (int i = 0; i < BROADCAST_AMOUNT_MAX; i++)
	{
		cout << "index:" << i << ",id:" << data.items[i].id << ",status:" << (int)data.items[i].status << endl;
		cout << "content:" << data.items[i].content << endl;
	}

	return 0;
}
