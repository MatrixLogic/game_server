/*
 * CPengyouPlatform.cpp
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#include "PengyouPlatform.h"
#include "commlog.h"

CPengyouPlatform::CPengyouPlatform()
{
	m_pengyou = NULL;
}

CPengyouPlatform::~CPengyouPlatform()
{
	if(m_pengyou != NULL)
	{
		delete m_pengyou;
	}
}

int CPengyouPlatform::Initialize(const string &appid, const string &appkey, map<string, string> &params)
{
	IOpenPlatform::Initialize(appid, appkey, params);
	if(params["platform"] == "4")
	{
		SetPlatformType(PT_QZONE);
		m_pengyou =  new CPengyou(CPengyou::QPT_QZONE, appid, appkey, params["appname"], params["domain"]);
	}
	else if(params["platform"] == "11")
	{
		SetPlatformType(PT_TXWEIBO);
		m_pengyou =  new CPengyou(CPengyou::QPT_WEIBO, appid, appkey, params["appname"], params["domain"]);
	}
	else
	{
		SetPlatformType(PT_PENGYOU);
		m_pengyou =  new CPengyou(CPengyou::QPT_PENGYOU, appid, appkey, params["appname"], params["domain"]);
	}
	info_log("[init ok][appid=%s,appkey=%s,appname=%s,domain=%s]",
			appid.c_str(), appkey.c_str(), params["appname"].c_str(), params["domain"].c_str());
	return 0;
}

int CPengyouPlatform::GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey)
{
	if(m_pengyou == NULL)
	{
		return -1;
	}

	T_UserInfo pyUserInfo;
	int ret = m_pengyou->GetUserInfo(pyUserInfo,openid,openkey);
	if( 0 != ret )
	{
		error_log("[GetUserInfo fail][ret=%d,openid=%s,errMsg=%s]",
				ret,openid.c_str(),GetErrorMessage().c_str());
		return ret;
	}

	userInfo.Clear();
	userInfo.OpenId = openid;
	userInfo.Name = pyUserInfo.strNickName;
	string figureUrl = pyUserInfo.strFigureUrl;
	String::ToLower(figureUrl);
	if(figureUrl.find("http://") != 0)
	{
		userInfo.FigureUrl = string("http://") + pyUserInfo.strFigureUrl;
	}
	else
	{
		userInfo.FigureUrl = pyUserInfo.strFigureUrl;
	}
	if(pyUserInfo.strGender == "男")
	{
		userInfo.Gender = GD_MALE;
	}
	else if(pyUserInfo.strGender == "女")
	{
		userInfo.Gender = GD_FEMALE;
	}
	else
	{
		userInfo.Gender = GD_UNKNOW;
	}
	userInfo.Nation = "中国";
	userInfo.Province = pyUserInfo.strProvince;
	userInfo.City = pyUserInfo.strCity;
	userInfo.ExtraIntInfo["bIsVip"] = pyUserInfo.bIsVip ? 1 : 0;
	userInfo.ExtraIntInfo["bIsYearVip"] = pyUserInfo.bIsYearVip ? 1 : 0;
	userInfo.ExtraIntInfo["nVipLevel"] = pyUserInfo.nVipLevel;

	return ret;
}

int CPengyouPlatform::GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey)
{
	if(m_pengyou == NULL)
	{
		return -1;
	}

	T_RelationInfo rif;
	int ret = m_pengyou->GetFriendList(rif, openid, openkey, 0, 1, 0);
	if( 0 != ret )
	{
		error_log("[GetFriendList fail][ret=%d,openid=%s,errMsg=%s]",
				ret,openid.c_str(),GetErrorMessage().c_str());
		return ret;
	}

	friendList.clear();
	for(T_RelationInfo::RIIVector::iterator itr = rif.vecItem.begin(); itr != rif.vecItem.end(); itr++)
	{
		friendList.push_back(itr->strOpenId);
	}
	return ret;
}

string CPengyouPlatform::GetErrorMessage()
{
	if(m_pengyou == NULL)
	{
		return "CPengyou not init";
	}
	return m_pengyou->GetErrorMessage();
}

