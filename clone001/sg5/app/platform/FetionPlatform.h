/*
 * FetionPlatform.h
 *
 *  Created on: 2012-2-9
 *      Author: dada
 */

#ifndef FETIONPLATFORM_H_
#define FETIONPLATFORM_H_

#include "IOpenPlatform.h"

class CFetionPlatform : public IOpenPlatform
{
public:
	CFetionPlatform() {}
	virtual ~CFetionPlatform() {}
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage() { return m_errMsg; };
protected:
	string m_errMsg;
};

#endif /* FETIONPLATFORM_H_ */
