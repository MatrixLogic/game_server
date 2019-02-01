/*
 * KuaiwanPlatform.h
 *
 *  Created on: 2011-8-16
 *      Author: dada
 */

#ifndef KUAIWANPLATFORM_H_
#define KUAIWANPLATFORM_H_

#include "IOpenPlatform.h"
#include "Common.h"

class CKuaiwanPlatform: public IOpenPlatform
{
public:
	CKuaiwanPlatform() {}
	virtual ~CKuaiwanPlatform() {}
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage() { return m_errMsg; };
private:
	string m_userData;
	string m_errMsg;
};

#endif /* KUAIWANPLATFORM_H_ */
