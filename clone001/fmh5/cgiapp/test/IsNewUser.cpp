/*
 * IsNewUser.cpp
 *
 *  Created on: 2016-6-23
 *      Author: Ralf
 */

#include "LogicInc.h"

int main(int argc, char* argv[])
{
	if (!Kernel::Init(APP_CONFIG_PATH))
	{
		cout << "0";
		return 1;
	}
	Config::SetDomain(0);

	if (argc != 3)
	{
		cout << "0";
		return 1;
	}
	string openid = argv[1];
	unsigned db = atoi(argv[2]);

	CDataUserMapping dbUserMapping;
	unsigned uid;
	int ret = dbUserMapping.GetUID(openid, db, uid);
	if (R_ERR_NO_DATA == ret)
		cout<<1;
	else
		cout<<0;

	return 0;
}


