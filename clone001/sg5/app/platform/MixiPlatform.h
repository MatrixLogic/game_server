/*
 * MixiPlatform.h
 *
 *  Created on: 2011-8-29
 *      Author: dada
 */

#ifndef MIXIPLATFORM_H_
#define MIXIPLATFORM_H_

#include "IOpenPlatform.h"

class CMixiPlatform: public IOpenPlatform
{
public:

	CMixiPlatform() {}
	virtual ~CMixiPlatform() {}
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage() { return m_errMsg; };

protected:

	int GetAccessToken(const string &openid, const string &openkey);

	string m_errMsg;
	string m_lastOpenId;
	string m_lastToken;
};

#endif /* MIXIPLATFORM_H_ */
