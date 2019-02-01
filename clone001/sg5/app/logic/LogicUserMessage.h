/*
 * LogicUserMessage.h
 *
 *  Created on: 2013-4-18
 *      Author: Administrator
 */

#ifndef LOGICUSERMESSAGE_H_
#define LOGICUSERMESSAGE_H_

#include "LogicInc.h"

class CLogicUserMessage {
public:
	int AddUserMessage(const DataUserMessage &data);
	int SetUserMessage(const DataUserMessage &data);
	int SetUserMessageStatus(unsigned uid, uint64_t message_id, int status);
	int GetUserMessage(unsigned uid, uint64_t message_id, DataUserMessage &data);
	int GetUserMessage(unsigned uid, int type, int status,
			unsigned startTime, vector<DataUserMessage> &datas);
	int GetUserMessageForTime(unsigned uid, int type, unsigned startTime, vector<DataUserMessage> &datas);
	int GetUserMessageForTime(unsigned uid, int type, int toward, unsigned startTime, vector<DataUserMessage> &datas);
};

#endif /* LOGICUSERMESSAGE_H_ */
