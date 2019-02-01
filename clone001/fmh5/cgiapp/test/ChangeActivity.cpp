/*
 * ChangeActivity.cpp
 *
 *  Created on: 2016-12-16
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

	if(argc < 5)
	{
		cout<<"para error"<<endl;
		return 1;
	}
	unsigned id = atoi(argv[1]);
	unsigned bts = atoi(argv[2]);
	unsigned ets = atoi(argv[3]);
	unsigned ver = atoi(argv[4]);

	CLogicAdmin logicAdmin;
	cout<<logicAdmin.SetActTime(id, bts, ets, ver)<<endl;

	return 0;
}



