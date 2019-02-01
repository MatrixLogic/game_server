/*
 * LogicPlatformUser.h
 *
 *  Created on: 2011-12-14
 *      Author: dada
 */

#ifndef LOGICPLATFORMUSER_H_
#define LOGICPLATFORMUSER_H_

#include "LogicInc.h"

class CLogicPlatformUser
{
public:
	int SetUserData(const string &openid, PlatformType platform, const Json::Value &data);
	int GetUserData(const string &openid, PlatformType platform, Json::Value &data);	//不存在自动增加空数据
	int RemoveUserData(const string &openid, PlatformType platform);

	int AddPendingMessage(const string &openid, PlatformType platform, uint64_t messageId);
	int OnRegisterUser(const string &openid, PlatformType platform, unsigned uid);
};

#endif /* LOGICPLATFORMUSER_H_ */
