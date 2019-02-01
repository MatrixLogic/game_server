

#ifndef _VN_PLATFORM_H_
#define _VN_PLATFORM_H_

#include "IOpenPlatform.h"
#include "Common.h"

class CVNPlatform : public IOpenPlatform
{
public:
	CVNPlatform();
	virtual int Initialize(const string &appid, const string &appkey, map<string, string> &params);
	virtual void SetParameter(const map<string, string> &params);
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage();
	virtual int Is_Login(const string &openid, const string &openkey,const string &pf);
private:
	string m_errorMessage;
	string m_pf;
	string m_userip;
};

#endif
