/*
 * SinaWbPlatform.h
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#ifndef SINAWBPLATFORM_H_
#define SINAWBPLATFORM_H_

#include "IOpenPlatform.h"
#include "Common.h"

class CSinaWbPlatform : public IOpenPlatform
{
public:
	CSinaWbPlatform();
	virtual ~CSinaWbPlatform();
	virtual int Initialize(const string &appid, const string &appkey, map<string, string> &params);
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage();

private:
	string m_errorMessage;
};

#endif /* SINAWBPLATFORM_H_ */
