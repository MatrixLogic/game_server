/*
 * LogicUserMappingBasic.h
 *
 *  Created on: 2011-7-11
 *      Author: dada
 */

#ifndef LogicUserMapping_H_
#define LogicUserMapping_H_

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

class CLogicUserMapping
{
public:
	//autoAlt: 是否自动从互通平台查找用户
	int GetUid(unsigned &uid, PlatformType platform, const string &openid, bool autoAlt = true);

	int LoginPlatformUser(
			PlatformType platform,
			OPUserInfo &userInfo,
			const string &iopenid,
			unsigned &uid,
			bool &isNewUser,
			string& via);

	int SetUserMapping(unsigned uid, bool newUser, PlatformType platform, OPUserInfo &userInfo, string& via);

};

#endif /* LogicUserMapping_H_ */
