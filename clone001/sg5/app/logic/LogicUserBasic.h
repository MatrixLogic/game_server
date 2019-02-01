/*
 * LogicUserBasicBasic.h
 *
 *  Created on: 2011-7-11
 *      Author: dada
 */

#ifndef LOGICUSERBASIC_H_
#define LOGICUSERBASIC_H_

#include "LogicInc.h"

enum VipType
{
	VT_NORMAL = 0,
	VT_QQ_YELLOW = 1,
	VT_QQ_YELLOW_YEAR = 2,
	VT_QQ_BLUE = 4,
	VT_QQ_BLUE_YEAR = 8,
	VT_QQ_SUPER_BULE = 16,
};

class CLogicUserBasic
{
public:
	//autoAlt: 是否自动从互通平台查找用户
	int GetUid(unsigned &uid, PlatformType platform, const string &openid, bool autoAlt = true);

	int LoginPlatformUser(
			PlatformType platform,
			OPUserInfo &userInfo,
			const OPFriendList &friendList,
			const string &iopenid,
			unsigned &uid,
			bool &isNewUser );

	int GetFriendsJson(unsigned uid, PlatformType platform, Json::Value &friends, bool encode = false);

	int GetUserBasic(unsigned uid, PlatformType platform, DataUserBasic &user);

	//no include: friends
	int GetUserBasicLimit(unsigned uid, PlatformType platform, DataUserBasic &users);

	//不返回friends，自动切换互通平台
	int GetUserBasicLimitSmart(unsigned uid, PlatformType platform, DataUserBasic &user);

	int GetFriends(unsigned uid, PlatformType platform, UidList &friends);
	bool IsFriend(unsigned uid, PlatformType platform, unsigned fid);
	int GetBlacksJson(unsigned uid, PlatformType platform, Json::Value &blacks);
	int GetOidFriends(unsigned uid, PlatformType platform, string &oid_friends);
	int SetOidFriends(unsigned uid, PlatformType platform, const string &oid_friends);

	int RemoveUserMapping(unsigned uid);

	int SetUserBasic(unsigned uid, bool newUser, PlatformType platform, OPUserInfo &userInfo, const OPFriendList &friendList);

	int GetOpenKey(unsigned uid, string &openid, string &openkey);
	int GetWorldUserInfo(unsigned uid, int platform,string &signature,string &figure_url,string &name);
	int SetWorldUserInfo(unsigned uid, int platform,const string &signature);
	int OldServerToNewServerReward(unsigned uid, string &openId);
	int CheckBlackUser(const string &openid,bool &black);

};

#endif /* LOGICUSERBASIC_H_ */
