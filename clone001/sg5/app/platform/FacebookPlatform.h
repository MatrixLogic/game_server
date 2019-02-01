/*
 * CFacebookPlatform.h
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#ifndef FACEBOOKPLATFORM_H_
#define FACEBOOKPLATFORM_H_

#include "IOpenPlatform.h"
#include "Common.h"

struct FacebookFeed
{
	string message;
	string picture;
	string link;
	string name;
	string caption;
	string description;
	string source;
	string actions;
	string to;
};

class CFacebookPlatform : public IOpenPlatform
{
public:
	CFacebookPlatform();
	virtual ~CFacebookPlatform();
	virtual int Initialize(const string &appid, const string &appkey, map<string, string> &params);
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage();

	int GetOtherUserInfo(OPUserInfo userInfo, const string &queryOpenId, const string &openid, const string &openkey);
	int GetLikes(vector<string> &appIds, const string &openid, const string &openkey);
	int SendFeed(const string &openid, const string &openkey, const FacebookFeed &feed);

private:
	string m_errorMessage;
};

#endif /* FACEBOOKPLATFORM_H_ */
