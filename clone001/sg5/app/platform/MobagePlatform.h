/*
 * MobagePlatform.h
 *
 *  Created on: 2011-11-22
 *      Author: dada
 */

#ifndef MOBAGEPLATFORM_H_
#define MOBAGEPLATFORM_H_

#include "IOpenPlatform.h"

class CMobagePlatform: public IOpenPlatform
{
public:
	CMobagePlatform() {}
	virtual ~CMobagePlatform() {}
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	int GetBlackList(OPFriendList &blackList, const string &openid, const string &openkey);
	virtual string GetErrorMessage() { return m_errMsg; };
protected:
	string m_errMsg;
};

#endif /* MOBAGEPLATFORM_H_ */
