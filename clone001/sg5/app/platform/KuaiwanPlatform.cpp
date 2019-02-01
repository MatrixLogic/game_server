/*
 * KuaiwanPlatform.cpp
 *
 *  Created on: 2011-8-16
 *      Author: dada
 */

#include "KuaiwanPlatform.h"

int CKuaiwanPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	string sUserInfo;
	string checkKey = Crypt::UrlDecode(openkey);
	size_t spIndex = checkKey.find("|");
	if(spIndex != string::npos)
	{
		sUserInfo = checkKey.substr(spIndex + 1);
		checkKey = checkKey.substr(0, spIndex);
	}

	string check = openid + GetAppKey();
	string key = Crypt::Md5Encode(check);
	if(!String::EqualNoCase(key, checkKey))
	{
		m_errMsg = "openkey_error";
		error_log("[openkey_error][openid=%s,openkey=%s,realkey=%s]",
				openid.c_str(), checkKey.c_str(), key.c_str());
		return -1;
	}

	userInfo.Clear();
	userInfo.OpenId = openid;
	Json::Value jsonUserInfo;
	if(Json::FromString(jsonUserInfo, sUserInfo))
	{
		if(jsonUserInfo.isObject())
		{
			Json::GetString(jsonUserInfo, "name", userInfo.Name);
			Json::GetString(jsonUserInfo, "photo", userInfo.FigureUrl);
		}
	}

	return 0;
}

int CKuaiwanPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	friendList.clear();
	return 0;
}

