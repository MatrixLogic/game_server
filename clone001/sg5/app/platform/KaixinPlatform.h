/*
 * KaixinPlatform.h
 *
 *  Created on: 2012-2-2
 *      Author: dada
 */

#ifndef KAIXINPLATFORM_H_
#define KAIXINPLATFORM_H_

#include "IOpenPlatform.h"

class CKaixinPlatform: public IOpenPlatform
{
public:
	CKaixinPlatform() {}
	virtual ~CKaixinPlatform() {}
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage() { return m_errMsg; };
protected:
	string m_errMsg;
};

#endif /* KAIXINPLATFORM_H_ */
