/*
 * BroadCast.cpp
 *
 *  Created on: 2015-12-17
 *      Author: Ralf
 */


#include "LogicInc.h"
#include <iostream>

int main(int argc, char *argv[])
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "init kernel fail" << endl;
		return 1;
	}
	Config::SetDomain(0);

	if(argc < 4)
	{
		cout<<"para error"<<endl;
		return 1;
	}
	int repeats = atoi(argv[1]);
	int interval = atoi(argv[2]);
	string serverid = argv[3];

	string path("broadcast.dat");
	string message;
	int ret = File::Read(path,message);
	if (0 != ret)
	{
		cout << "Read file fail path=" << path << endl;
		return 1;
	}

	CLogicAdmin logicAdmin;
	return logicAdmin.BroadCast(message, repeats, interval, serverid);
}


