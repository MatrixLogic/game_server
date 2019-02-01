/*
 * RenrenPlatform.h
 *
 *  Created on: 2011-7-4
 *      Author: dada
 */

#ifndef RENRENPLATFORM_H_
#define RENRENPLATFORM_H_

#include "IOpenPlatform.h"
#include "Common.h"

class CRenrenPlatform : public IOpenPlatform
{
public:
	CRenrenPlatform();
	virtual ~CRenrenPlatform();
	virtual int Initialize(const string &appid, const string &appkey, map<string, string> &params);
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage();

private:
	string m_errorMessage;
	string m_apiUrl;
};

#endif /* RENRENPLATFORM_H_ */
