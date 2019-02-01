/*
 * LogicCustomServiceAdmin.cpp
 *
 *  Created on: 2013-9-5
 *      Author: Administrator
 */

#include "LogicCustomServiceAdmin.h"

map<string, UserInfo> CLogicCustomServiceAdmin::userList;
map<string, UserSession> CLogicCustomServiceAdmin::userSession;

int CLogicCustomServiceAdmin::UpdateUserList()
{
	static bool alreadyUpdate = false;
	if (alreadyUpdate)
	{
		return 0;
	}
	string adminFile = Config::GetValue("custom_admin_list");
	if (adminFile.empty())
	{
		REFUSE_RETURN_MSG("The user not exist");
	}
	struct stat st;
	if (stat(adminFile.c_str(), &st) != 0)
	{
		REFUSE_RETURN_MSG("System error");
	}

	FILE *fp = fopen(adminFile.c_str(), "r");
	if (!fp)
	{
		REFUSE_RETURN_MSG("System error");
	}
	char line[256];
	char *pch, *name, *pwd;
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		name = pwd = NULL;
		pch = line;
		while (*pch ==' ' || *pch == '\t') pch++;
		name = pch;
		while (*pch != '\0' && *pch != '\r' && *pch != '\n' && *pch != ' ' && *pch != '\t') pch++;
		if (*pch == '\0' || *pch == '\r' || *pch == '\n') continue;
		*pch++ = '\0';
		while (*pch ==' ' || *pch == '\t') pch++;
		pwd = pch;
		while (*pch != '\0' && *pch != '\r' && *pch != '\n'  && *pch != ' ' && *pch != '\t') pch++;
		if (*pch != '\0') *pch = '\0';

		UserInfo info;
		info.pwd = pwd;
		userList[string(name)] = info;
	}
	alreadyUpdate = true;
	return 0;
}

int CLogicCustomServiceAdmin::Login(const string &name, const string &pwd, string &sky)
{
	int ret = UpdateUserList();
	if (ret != 0)
		return ret;
	map<string, UserInfo>::iterator it = userList.find(name);
	if (it == userList.end())
	{
		REFUSE_RETURN_MSG("The user not exist");
	}
	if ((it->second).pwd != pwd)
	{
		REFUSE_RETURN_MSG("Passwd error");
	}

	UserSession sess;
	sess.skey = Session::CreateSessionKey(0);
	sess.activeTime = Time::GetGlobalTime();
	userSession[name] = sess;
	sky = sess.skey;
	return 0;
}

int CLogicCustomServiceAdmin::ChangePwd(const string &name, const string &OldPwd,const string &NewPwd)
{
	int ret = UpdateUserList();
	if (ret != 0)
		return ret;
	map<string, UserInfo>::iterator it = userList.find(name);
	if (it == userList.end())
	{
		REFUSE_RETURN_MSG("The user not exist");
	}
	if ((it->second).pwd != OldPwd)
	{
		REFUSE_RETURN_MSG("Passwd error");
	}
	(it->second).pwd = NewPwd;
	string adminFile = Config::GetValue("custom_admin_list");
	if (adminFile.empty())
	{
		REFUSE_RETURN_MSG("System error");
	}
	FILE *fp = fopen(adminFile.c_str(), "w");
	if (!fp)
	{
		REFUSE_RETURN_MSG("System error");
	}
	for (it = userList.begin(); it != userList.end(); it++)
	{
		fprintf(fp, "%s %s\n", (it->first).c_str(), (it->second).pwd.c_str());
	}
	fclose(fp);

	return 0;
}

int CLogicCustomServiceAdmin::CheckSession(const string &name, const string &skey)
{
	if (name.empty())
	{
		SESS_ERROR_RETURN_MSG("Name is empty");
	}
	map<string, UserSession>::iterator it = userSession.find(name);
	if (it == userSession.end())
	{
		SESS_ERROR_RETURN_MSG("Session error");
	}
	if (Time::GetGlobalTime() - (it->second).activeTime > 3600)
	{
		SESS_ERROR_RETURN_MSG("Session timeout");
	}
	return 0;
}

void CLogicCustomServiceAdmin::Log
(
	const string &admin,
	const string &op,
	const string &reason,
	unsigned uid,
	const string &value,
	const string &detail
)
{
	string dir = "/data/release/sgonline/s3/adminlog/";
	char buf[256];
	time_t now;
	time( &now );
	struct tm *ptm;
	ptm = localtime(&now);
	snprintf(buf, sizeof(buf), "%scustom_op_%04d%02d%02d.log",
			dir.c_str(),
			ptm->tm_year+1900,
			ptm->tm_mon+1,
			ptm->tm_mday);
	FILE *fp = fopen(buf, "a");
	if (!fp)
	{
		return;
	}
	fprintf(fp, "%04d%02d%02d %02d:%02d:%02d|%s|%s|%s|%u|%s\n",
			ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,
			admin.c_str(),
			op.c_str(),
			reason.c_str(),
			uid,
			value.c_str()
			);
	fclose(fp);
	if (!detail.empty())
	{
		snprintf(buf, sizeof(buf), "%scustom_detail_%04d%02d%02d.log",
				dir.c_str(),
				ptm->tm_year+1900,
				ptm->tm_mon+1,
				ptm->tm_mday);

		fp = fopen(buf, "a");
		if (!fp) return;
		fprintf(fp, "%04d%02d%02d %02d:%02d:%02d|%s|%s|%u|%s\n", ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,
				ptm->tm_hour,ptm->tm_min,ptm->tm_sec,admin.c_str(),op.c_str(),uid,detail.c_str());
		fclose(fp);
	}
	return;
}
