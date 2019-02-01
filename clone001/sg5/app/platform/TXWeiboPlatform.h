/*
 * TXWeiboPlatform.h
 *
 *  Created on: 2011-10-14
 *      Author: dada
 */

#ifndef TXWEIBOPLATFORM_H_
#define TXWEIBOPLATFORM_H_

#include "Common.h"
#include "IOpenPlatform.h"

class CTXWeiboPlatform: public IOpenPlatform
{

public:

	CTXWeiboPlatform() {}
	virtual ~CTXWeiboPlatform() {}

	int GetAuthorizeUrl(string &url);
	int GetOpenKey(const string &token, const string &verifier, string &openid, string &openkey);

	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);

	virtual string GetErrorMessage() { return m_errMsg; };

protected:

	bool MakeOpenKey(string &openkey, const string &openid, const string &token, const string &tokenSecret);
	bool ParseOpenKey(const string &openkey, const string &openid, string &token, string &tokenSecret);

	string m_errMsg;

};

#endif /* TXWEIBOPLATFORM_H_ */
