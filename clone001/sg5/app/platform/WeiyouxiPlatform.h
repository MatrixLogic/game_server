/*
 * WeiyouxiPlatform.h
 *
 *  Created on: 2011-5-16
 *      Author: dada
 */

#ifndef WEIYOUXIPLATFORM_H_
#define WEIYOUXIPLATFORM_H_

#include "IOpenPlatform.h"
#include "Common.h"

class CWeiyouxiPlatform : public IOpenPlatform
{
public:
	CWeiyouxiPlatform() {};
	virtual ~CWeiyouxiPlatform() {};
	virtual int GetUserInfo(OPUserInfo &userInfo, const string &openid, const string &openkey);
	virtual int GetAppFriendList(OPFriendList &friendList, const string &openid, const string &openkey);
	virtual string GetErrorMessage() { return m_errorMessage; };

	int IsFan(const string &openid, const string &openkey, int &result);
	int Scored(const string &openid, const string &openkey, int &result);	//result<0未打分，>=0为用户所评分数
	int IgnoreInvite(const string &openid, const string &openkey);
	int SetAchievement(const string &openid, const string &openkey, unsigned id);
	int SetLeaderboard(const string &openid, const string &openkey, unsigned id, unsigned value);

	int GetFriendList(vector<OPUserInfo> &friendList, const string &openid, const string &openkey);

private:
	string m_errorMessage;
};

#endif /* WEIYOUXIPLATFORM_H_ */
