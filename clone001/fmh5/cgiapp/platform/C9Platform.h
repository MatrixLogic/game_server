/*
 * C9Platform.h
 *
 *  Created on: 2015-8-26
 *      Author: Ralf
 */

#ifndef C9PLATFORM_H_
#define C9PLATFORM_H_

#include "IOpenPlatform.h"
#include "Common.h"

class C9Platform : public IOpenPlatform
{
public:
	C9Platform();
	virtual int Initialize(const string &appid, const string &appkey, map<string, string> &params);
	virtual void SetParameter(const map<string, string> &params);
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage();
	virtual int Is_Login(const string &openid, const string &openkey,const string &pf);
	virtual void ReplyCharge();
	virtual void SetReply(string& reply);
private:
	string m_errorMessage;
	string m_pf;
	string m_userip;
	string m_reply;
	unsigned m_time, m_cm;
};




#endif /* C9PLATFORM_H_ */
