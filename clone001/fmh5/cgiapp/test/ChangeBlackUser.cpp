/*
 * ChangeBlackUser.cpp
 *
 *  Created on: 2014-7-3
 *      Author: Ralf
 */


#include "LogicInc.h"
#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "usage: " << argv[0] << " [openid] [block ts/gm flag] " << endl;
		return 1;
	}
	string openid = argv[1];
	unsigned ts = atoi(argv[2]);

	CDataBlackUser dbBlackUser;
	DataBlackUser blackUser;
	blackUser.block_time = ts;
	if(openid == "openid2change")
	{
		ifstream fin("openid2change");
		if (!fin.good()) {
			cout << "open openid2change fail" << endl;
			return 1;
		}
		while (!fin.eof() && fin.good()) {
			string id;
			fin >> blackUser.open_id;
			int ret = dbBlackUser.SetBlackUser(blackUser);
			if(ret)
				cout<<id<<" fail"<<endl;
		}
	}
	else
	{
		blackUser.open_id = openid;
		int ret = dbBlackUser.SetBlackUser(blackUser);
		if(ret == 0)
			cout<<"ok"<<endl;
		else
			cout<<"ret="<<ret<<endl;
	}

	return 0;
}





