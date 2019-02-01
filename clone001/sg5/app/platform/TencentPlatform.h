/*
 * 支持腾讯v3版api
 */

#ifndef _TENCENT_PLATFORM_H_
#define _TENCENT_PLATFORM_H_

#include "IOpenPlatform.h"
#include "Common.h"

class CTencentPlatform : public IOpenPlatform
{
public:
	CTencentPlatform();
	virtual int Initialize(const string &appid, const string &appkey, map<string, string> &params);
	virtual void SetParameter(const map<string, string> &params);
	virtual unsigned GetPlatformByPF();
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage();
	virtual int Is_Login(const string &openid, const string &openkey,const string &pf);
	virtual int Is_area_Login(const string &openid, const string &openkey,const string &seqid);
	virtual int Set_Feed(const string &openid, const string &openkey);
	virtual int Set_Request(const string &openid, const string &openkey,const string &fopenid);
	virtual int Send_Notification(const string &openid, const string &openkey);
	virtual int Get_Figure(OPUserInfo &userInfo,const string &openid, const string &openkey);
private:
	string m_errorMessage;
	string m_pf;
	string m_userip;
};

#endif
