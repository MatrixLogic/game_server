/*
 * LogicCustomServiceAdmin.h
 *
 *  Created on: 2013-9-5
 *      Author: Administrator
 */

#ifndef LOGICCUSTOMSERVICEADMIN_H_
#define LOGICCUSTOMSERVICEADMIN_H_

#include "LogicInc.h"

struct UserSession
{
	string   skey;
	unsigned activeTime;
};

struct UserInfo
{
	string pwd;
};

class CLogicCustomServiceAdmin
{
public:
	int Login(const string &name, const string &pwd, string &sky);
	int ChangePwd(const string &name, const string &OldPwd,const string &NewPwd);
	int CheckSession(const string &name, const string &skey);
private:
	static map<string, UserInfo> userList;
	static map<string, UserSession> userSession;
	static int UpdateUserList();

public:
	static void Log(const string &admin, const string &op, const string &reason,
			unsigned uid, const string &value, const string &detail = "");
};

#endif /* LOGICCUSTOMSERVICEADMIN_H_ */
