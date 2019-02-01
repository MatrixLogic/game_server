#ifndef LOGICUSERTEXT_H_
#define LOGICUSERTEXT_H_

#include "LogicInc.h"

class CLogicUserText
{

public:
	int SetUserData(unsigned uid, const Json::Value &data);
	int GetUserData(unsigned uid, Json::Value &data);	//不存在自动增加空数据
	int RemoveUserData(unsigned uid);

};

#endif /* LOGICUSERTEXT_H_ */
