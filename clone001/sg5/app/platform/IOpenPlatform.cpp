/*
 * CPengyouPlatform.cpp
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#include "IOpenPlatform.h"

int IOpenPlatform::Initialize(const string &appid, const string &appkey, map<string, string> &params)
{
	m_appId = appid;
	m_appKey = appkey;
	m_config = params;
	return 0;
}

string IOpenPlatform::GetConfig(const string &name)
{
	map<string, string>::iterator iterFind;
	iterFind = m_config.find(name);
	if(iterFind == m_config.end())
	{
		return "";
	}
	return iterFind->second;
}

int IOpenPlatform::GetFriendList(vector<OPUserInfo> &friendList, const string &openid, const string &openkey)
{
	return -1;
}
int IOpenPlatform::Is_Login(const string &openid, const string &openkey,const string &pf)
{
	return -1;
}
void IOpenPlatform::SetParameter(const map<string, string> &params)
{

}
