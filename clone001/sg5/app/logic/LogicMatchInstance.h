/*
 * LogicMatchInstance.h
 *
 *  Created on: 2013-5-21
 *      Author: Administrator
 */

#ifndef LOGICMATCHINSTANCE_H_
#define LOGICMATCHINSTANCE_H_

#include "LogicInc.h"

class CLogicMatchInstance {

public:
	int GetLocalMatchInstance(unsigned instid,Json::Value &data);
	int SetLocalMatchInstance(unsigned instid,const Json::Value &data);
	int GetAllServerMatchInstance(unsigned instid,Json::Value &data);
	int SetAllServerMatchInstance(unsigned instid,const Json::Value &data);
private:
	int Load(unsigned instanceId, const string &filePath, Json::Value &data);
	int Save(unsigned instanceId, const string &filePath, const Json::Value &data);
};

#endif /* LOGICMATCHINSTANCE_H_ */
