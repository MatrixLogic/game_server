/*
 * C9Platform.cpp
 *
 *  Created on: 2015-8-26
 *      Author: Ralf
 */




#include "C9Platform.h"
#include <iostream>

C9Platform::C9Platform()
{
	m_time = m_cm = 0;
}

int C9Platform::Initialize(const string &appid, const string &appkey, map<string, string> &params)
{
	IOpenPlatform::Initialize(appid, appkey, params);
	//info_log("[init ok][appid=%s,appkey=%s,v3domain=%s]",	appid.c_str(), appkey.c_str(), params["v3domain"].c_str());
	return 0;
}

void C9Platform::SetParameter(const map<string, string> &params)
{
	m_pf.clear();
	m_userip.clear();
	m_time = m_cm = 0;
	map<string, string>::const_iterator it = params.find("pf");
	if (it != params.end())
		m_pf = it->second;
	it = params.find("userip");
	if (it != params.end())
		m_userip = it->second;
	it = params.find("time");
	if (it != params.end())
		m_time = CTrans::STOI(it->second);
    it = params.find("cm");
    if (it != params.end())
        m_cm = CTrans::STOI(it->second);
}

int C9Platform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	userInfo.Clear();
	m_errorMessage.clear();

	if (Math::Abs(Time::GetGlobalTime() - m_time) > 7200)
	{
		m_errorMessage = "time_out";
		error_log("[time_out][openid=%s,openkey=%s,cm=%u,m_time=%u,errmsg=%s]",
				openid.c_str(), openkey.c_str(), m_cm, m_time, m_errorMessage.c_str());
		return -1;
	}

	int server = 0;
	Config::GetDB(server);
	string osig  = CTrans::ITOS(server) + openid + CTrans::ITOS(m_time)	+ CTrans::ITOS(m_cm) + m_appKey; 
	string sig = Crypt::Md5Encode(osig);
	if(sig != openkey)
	{
		m_errorMessage = "openkey_error";
		error_log("[openkey_error][openid=%s,openkey=%s,osig=%s,sig=%s]",
				openid.c_str(), openkey.c_str(), osig.c_str(), sig.c_str());
		return -1;
	}
	userInfo.OpenId = openid;

	return 0;
}

int C9Platform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	return 0;
}
int C9Platform::Is_Login(const string &openid, const string &openkey,const string &pf)
{
	return 0;
}

string C9Platform::GetErrorMessage()
{
	return m_errorMessage;
}

void C9Platform::ReplyCharge()
{
	CgiUtil::PrintText(m_reply.c_str());
	m_reply.clear();
}
void C9Platform::SetReply(string& reply)
{
	m_reply = reply;
}


